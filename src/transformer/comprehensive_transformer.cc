#include "comprehensive_transformer.h"

#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/Builtins.h>
#include <clang/Rewrite/Core/Rewriter.h>

ComprehensiveTransformer::ComprehensiveTransformer(clang::ASTContext &context, clang::Rewriter &rewriter)
    : Transformer(context, rewriter)
{
}

void ComprehensiveTransformer::start()
{
    using namespace clang::ast_matchers;

    MatchFinder finder;

    // Match function calls
    auto callExprMatcher = callExpr().bind("callExpr");
    finder.addMatcher(callExprMatcher, this);

    // Match variable declarations
    auto varDeclMatcher = varDecl().bind("varDecl");
    finder.addMatcher(varDeclMatcher, this);

    // Match variable references (DeclRefExpr)
    auto varRefMatcher = declRefExpr(to(varDecl())).bind("varRef");
    finder.addMatcher(varRefMatcher, this);

    finder.matchAST(context);
}

void ComprehensiveTransformer::run(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    if (result.Nodes.getNodeAs<clang::CallExpr>("callExpr"))
    {
        handleFunctionCall(result);
    }
    else if (result.Nodes.getNodeAs<clang::VarDecl>("varDecl"))
    {
        handleVariableDecl(result);
    }
    else if (result.Nodes.getNodeAs<clang::DeclRefExpr>("varRef"))
    {
        handleVariableRef(result);
    }
}

void ComprehensiveTransformer::handleFunctionCall(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const CallExpr *callExpr = result.Nodes.getNodeAs<CallExpr>("callExpr"))
    {
        if (const FunctionDecl *function = callExpr->getDirectCallee())
        {
            auto functionName = function->getNameAsString();

            // Check if it's a builtin
            unsigned builtinID = function->getBuiltinID();
            bool isImplicit = function->isImplicit();
            bool isInSystemHeader = result.SourceManager->isInSystemHeader(function->getSourceRange().getBegin());

            if (builtinID != 0)
            {
                builtin_functions.insert(functionName);
                builtin_ids[functionName] = builtinID;
                // Don't transform builtin functions
                return;
            }

            if (isImplicit || isInSystemHeader)
            {
                // Don't transform system functions
                return;
            }

            // This is a user-defined function - transform it
            rewriter.InsertTextAfter(callExpr->getBeginLoc(), "fn_");

            if (user_functions.count(functionName) == 0)
            {
                // Also rename the function definition
                rewriter.InsertTextAfter(function->getLocation(), "fn_");
                user_functions.insert(functionName);
            }
        }
    }
}

void ComprehensiveTransformer::handleVariableDecl(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const VarDecl *varDecl = result.Nodes.getNodeAs<VarDecl>("varDecl"))
    {
        if (result.SourceManager->isInSystemHeader(varDecl->getSourceRange().getBegin()))
            return;

        auto varName = varDecl->getNameAsString();

        if (varName.empty())
            return;

        variables.insert(varName);

        // For now, just track variables - actual renaming logic can be added later
        // This could be made configurable via command line options
    }
}

void ComprehensiveTransformer::handleVariableRef(const clang::ast_matchers::MatchFinder::MatchResult &result)
{
    using namespace clang;

    if (const DeclRefExpr *declRef = result.Nodes.getNodeAs<DeclRefExpr>("varRef"))
    {
        if (const VarDecl *varDecl = dyn_cast<VarDecl>(declRef->getDecl()))
        {
            if (result.SourceManager->isInSystemHeader(varDecl->getSourceRange().getBegin()))
                return;

            auto varName = varDecl->getNameAsString();

            // For now, just track variable references - actual renaming logic can be added later
            // This could be made configurable via command line options
        }
    }
}

void ComprehensiveTransformer::print(clang::raw_ostream &stream)
{
    stream << "=== COMPREHENSIVE ANALYSIS ===\n";

    stream << "\nUser-defined functions (" << user_functions.size() << "):\n";
    for (const auto &fn : user_functions)
        stream << "  " << fn << "()\n";

    stream << "\nBuilt-in functions (" << builtin_functions.size() << "):\n";
    for (const auto &fn : builtin_functions)
    {
        stream << "  " << fn << "() [ID: " << builtin_ids[fn] << "]\n";
    }

    stream << "\nVariables found (" << variables.size() << "):\n";
    for (const auto &var : variables)
    {
        stream << "  " << var << "\n";
    }

    stream << "\n=== END ANALYSIS ===\n";
}