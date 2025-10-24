#ifndef FRONTEND_ACTION_HPP
#define FRONTEND_ACTION_HPP

#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>

#include <memory>
#include <string>
#include <vector>

namespace clang
{
    class CompilerInstance;
}

class XFrontendAction : public clang::ASTFrontendAction
{
  public:
    XFrontendAction();
    XFrontendAction(const std::string &renameFrom, const std::string &renameTo, const std::string &renameType);
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &compiler,
                                                                  llvm::StringRef inFile) override;

  private:
    std::string renameFrom;
    std::string renameTo;
    std::string renameType;
    bool hasRenameOptions;
};

#endif
