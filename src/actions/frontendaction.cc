#include "frontendaction.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>

#include "../consumer/consumer.h"

XFrontendAction::XFrontendAction() : hasRenameOptions(false)
{
}

XFrontendAction::XFrontendAction(const std::string &renameFrom, const std::string &renameTo,
                                 const std::string &renameType)
    : renameFrom(renameFrom), renameTo(renameTo), renameType(renameType), hasRenameOptions(true)
{
}

std::unique_ptr<clang::ASTConsumer> XFrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                                                       llvm::StringRef inFile)
{
    if (hasRenameOptions)
    {
        return std::unique_ptr<clang::ASTConsumer>(
            new XConsumer(compiler.getASTContext(), renameFrom, renameTo, renameType));
    }
    else
    {
        return std::unique_ptr<clang::ASTConsumer>(new XConsumer(compiler.getASTContext()));
    }
}
