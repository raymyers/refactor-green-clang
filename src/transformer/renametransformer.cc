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
    : Transformer(context, rewriter), oldName(oldName), newName(newName), targetLine(targetLine), targetDecl(nullptr)
{
}

void RenameTransformer::start()
{
    MatchFinder finder;
    
    if (targetLine != -1) {
        // Two-pass approach for line-specific renaming:
        // Pass 1: Find the declaration at the target line
        auto allDeclMatcher = namedDecl(hasName(oldName)).bind("targetDecl");
        finder.addMatcher(allDeclMatcher, this);
        finder.matchAST(context);
        
        // Pass 2: Find all references to the target declaration
        if (targetDecl != nullptr) {
            MatchFinder refFinder;
            auto declRefMatcher = declRefExpr().bind("declRef");
            auto memberMatcher = memberExpr().bind("memberExpr");
            
            refFinder.addMatcher(declRefMatcher, this);
            refFinder.addMatcher(memberMatcher, this);
            refFinder.matchAST(context);
        }
    } else {
        // Scope-aware general renaming: Two-pass approach
        // Pass 1: Find the FIRST declaration with the target name
        auto allDeclMatcher = namedDecl(hasName(oldName)).bind("firstDecl");
        finder.addMatcher(allDeclMatcher, this);
        finder.matchAST(context);
        
        // Pass 2: Find all references to the first declaration only
        if (targetDecl != nullptr) {
            MatchFinder refFinder;
            auto declRefMatcher = declRefExpr().bind("declRef");
            auto memberMatcher = memberExpr().bind("memberExpr");
            
            refFinder.addMatcher(declRefMatcher, this);
            refFinder.addMatcher(memberMatcher, this);
            refFinder.matchAST(context);
        }
    }
}

void RenameTransformer::run(const MatchFinder::MatchResult &result)
{
    // Handle declaration finding (Pass 1)
    const char* declBinding = (targetLine != -1) ? "targetDecl" : "firstDecl";
    if (const auto *namedDecl = result.Nodes.getNodeAs<NamedDecl>(declBinding)) {
        bool shouldProcess = false;
        
        if (targetLine != -1) {
            // Line-specific: check if this declaration is at the target line
            SourceLocation location = namedDecl->getLocation();
            shouldProcess = shouldRename(location);
        } else {
            // General: only process the first declaration found
            shouldProcess = (targetDecl == nullptr);
        }
        
        if (shouldProcess) {
            targetDecl = namedDecl;
            renameDeclaration(namedDecl);
        }
    }
    
    // Handle reference finding (Pass 2)
    else if (const auto *declRef = result.Nodes.getNodeAs<DeclRefExpr>("declRef")) {
        if (targetDecl && isReferenceTo(declRef, targetDecl)) {
            SourceLocation location = declRef->getLocation();
            rewriter.ReplaceText(location, oldName.length(), newName);
            renamedIdentifiers.insert("identifier reference");
        }
    }
    
    else if (const auto *memberExpr = result.Nodes.getNodeAs<MemberExpr>("memberExpr")) {
        if (targetDecl && isMemberAccessTo(memberExpr, targetDecl)) {
            SourceLocation location = memberExpr->getMemberLoc();
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

bool RenameTransformer::isReferenceTo(const DeclRefExpr* ref, const Decl* decl) const
{
    return ref->getDecl() == decl;
}

bool RenameTransformer::isMemberAccessTo(const MemberExpr* member, const Decl* decl) const
{
    return member->getMemberDecl() == decl;
}

void RenameTransformer::renameDeclaration(const NamedDecl* namedDecl)
{
    SourceLocation location = namedDecl->getLocation();
    rewriter.ReplaceText(location, oldName.length(), newName);
    
    if (isa<VarDecl>(namedDecl)) {
        renamedIdentifiers.insert("variable declaration");
    } else if (isa<FieldDecl>(namedDecl)) {
        renamedIdentifiers.insert("field declaration");
    } else if (isa<FunctionDecl>(namedDecl)) {
        renamedIdentifiers.insert("function declaration");
    } else if (isa<ParmVarDecl>(namedDecl)) {
        renamedIdentifiers.insert("parameter declaration");
    }
}