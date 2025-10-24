#include "rename_transformer.h"

#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Rewrite/Core/Rewriter.h>

RenameTransformer::RenameTransformer(clang::ASTContext &context, clang::Rewriter &rewriter, const std::string &fromName,
                                     const std::string &toName, const std::string &renameType)
    : Transformer(context, rewriter), fromName(fromName), toName(toName), renameType(renameType), renameCount(0)
{
}

void RenameTransformer::start()
{
    using namespace clang::ast_matchers;

    MatchFinder finder;

    if (renameType == "function")
    {
        // Match function calls
        auto callMatcher = callExpr(callee(functionDecl(hasName(fromName)))).bind("call");
        finder.addMatcher(callMatcher, this);

        // Match function declarations
        auto declMatcher = functionDecl(hasName(fromName)).bind("funcDecl");
        finder.addMatcher(declMatcher, this);
    }
    else if (renameType == "variable")
    {
        // Match variable declarations
        auto varDeclMatcher = varDecl(hasName(fromName)).bind("varDecl");
        finder.addMatcher(varDeclMatcher, this);

        // Match variable references
        auto varRefMatcher = declRefExpr(to(varDecl(hasName(fromName)))).bind("varRef");
        finder.addMatcher(varRefMatcher, this);
    }

    finder.matchAST(context);
}

void RenameTransformer::run(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    if (result.Nodes.getNodeAs<clang::CallExpr>("call") || result.Nodes.getNodeAs<clang::FunctionDecl>("funcDecl"))
    {
        handleFunctionRename(result);
    }
    else if (result.Nodes.getNodeAs<clang::VarDecl>("varDecl"))
    {
        handleVariableRename(result);
    }
    else if (result.Nodes.getNodeAs<clang::DeclRefExpr>("varRef"))
    {
        handleVariableRefRename(result);
    }
}

void RenameTransformer::handleFunctionRename(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const CallExpr *call = result.Nodes.getNodeAs<CallExpr>("call"))
    {
        if (const FunctionDecl *funcDecl = call->getDirectCallee())
        {
            if (funcDecl->getNameAsString() == fromName)
            {
                rewriter.ReplaceText(call->getCallee()->getSourceRange(), toName);
                renameCount++;
            }
        }
    }
    else if (const FunctionDecl *funcDecl = result.Nodes.getNodeAs<FunctionDecl>("funcDecl"))
    {
        if (funcDecl->getNameAsString() == fromName)
        {
            rewriter.ReplaceText(funcDecl->getLocation(), fromName.length(), toName);
            renameCount++;
        }
    }
}

void RenameTransformer::handleVariableRename(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const VarDecl *varDecl = result.Nodes.getNodeAs<VarDecl>("varDecl"))
    {
        if (result.SourceManager->isInSystemHeader(varDecl->getSourceRange().getBegin()))
            return;

        if (varDecl->getNameAsString() == fromName)
        {
            rewriter.ReplaceText(varDecl->getLocation(), fromName.length(), toName);
            renameCount++;
        }
    }
}

void RenameTransformer::handleVariableRefRename(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const DeclRefExpr *declRef = result.Nodes.getNodeAs<DeclRefExpr>("varRef"))
    {
        if (const VarDecl *varDecl = dyn_cast<VarDecl>(declRef->getDecl()))
        {
            if (result.SourceManager->isInSystemHeader(varDecl->getSourceRange().getBegin()))
                return;

            if (varDecl->getNameAsString() == fromName)
            {
                rewriter.ReplaceText(declRef->getLocation(), fromName.length(), toName);
                renameCount++;
            }
        }
    }
}

void RenameTransformer::print(clang::raw_ostream &stream)
{
    stream << "=== RENAME REPORT ===\n";
    stream << "Renamed " << renameCount << " instances of '" << fromName << "' to '" << toName
           << "' (type: " << renameType << ")\n";
    stream << "=== END REPORT ===\n";
}