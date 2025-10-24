#include "renametransformer.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::ast_matchers;

RenameTransformer::RenameTransformer(ASTContext &context, Rewriter &rewriter,
                                   const std::string &oldName, const std::string &newName,
                                   int targetLine)
    : Transformer(context, rewriter), oldName(oldName), newName(newName), targetLine(targetLine)
{
}

void RenameTransformer::start()
{
    MatchFinder finder;
    
    // Match variable declarations
    auto varDeclMatcher = varDecl(hasName(oldName)).bind("varDecl");
    
    // Match function declarations
    auto funcDeclMatcher = functionDecl(hasName(oldName)).bind("funcDecl");
    
    // Match parameter declarations
    auto parmDeclMatcher = parmVarDecl(hasName(oldName)).bind("parmDecl");
    
    // Match declaration references (variable/function usage)
    auto declRefMatcher = declRefExpr(to(namedDecl(hasName(oldName)))).bind("declRef");
    
    // Match member expressions (for struct/class members)
    auto memberMatcher = memberExpr(member(hasName(oldName))).bind("memberExpr");

    finder.addMatcher(varDeclMatcher, this);
    finder.addMatcher(funcDeclMatcher, this);
    finder.addMatcher(parmDeclMatcher, this);
    finder.addMatcher(declRefMatcher, this);
    finder.addMatcher(memberMatcher, this);

    finder.matchAST(context);
}

void RenameTransformer::run(const MatchFinder::MatchResult &result)
{
    
    // Handle variable declarations
    if (const auto *varDecl = result.Nodes.getNodeAs<VarDecl>("varDecl"))
    {
        SourceLocation location = varDecl->getLocation();
        if (shouldRename(location))
        {
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("variable declaration");
        }
    }
    
    // Handle function declarations
    else if (const auto *funcDecl = result.Nodes.getNodeAs<FunctionDecl>("funcDecl"))
    {
        SourceLocation location = funcDecl->getLocation();
        if (shouldRename(location))
        {
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("function declaration");
        }
    }
    
    // Handle parameter declarations
    else if (const auto *parmDecl = result.Nodes.getNodeAs<ParmVarDecl>("parmDecl"))
    {
        SourceLocation location = parmDecl->getLocation();
        if (shouldRename(location))
        {
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("parameter declaration");
        }
    }
    
    // Handle declaration references (usage)
    else if (const auto *declRef = result.Nodes.getNodeAs<DeclRefExpr>("declRef"))
    {
        SourceLocation location = declRef->getLocation();
        if (shouldRename(location))
        {
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("identifier reference");
        }
    }
    
    // Handle member expressions
    else if (const auto *memberExpr = result.Nodes.getNodeAs<MemberExpr>("memberExpr"))
    {
        SourceLocation location = memberExpr->getMemberLoc();
        if (shouldRename(location))
        {
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("member access");
        }
    }
}

void RenameTransformer::print(raw_ostream &stream)
{
    if (renamedIdentifiers.empty())
    {
        stream << "No occurrences of '" << oldName << "' found";
        if (targetLine != -1)
        {
            stream << " on line " << targetLine;
        }
        stream << ".\n";
    }
    else
    {
        stream << "Renamed '" << oldName << "' to '" << newName << "' in:\n";
        for (const auto &type : renamedIdentifiers)
        {
            stream << "  - " << type << "\n";
        }
    }
}

bool RenameTransformer::shouldRename(const SourceLocation &location) const
{
    if (targetLine == -1)
    {
        return true; // Rename all occurrences
    }
    
    const SourceManager &sourceManager = context.getSourceManager();
    unsigned line = sourceManager.getSpellingLineNumber(location);
    return line == static_cast<unsigned>(targetLine);
}