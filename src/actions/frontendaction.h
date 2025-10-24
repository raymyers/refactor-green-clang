#ifndef FRONTEND_ACTION_HPP
#define FRONTEND_ACTION_HPP

#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>

#include "../consumer/consumer.h"

#include <memory>
#include <string>
#include <vector>

namespace clang
{
    class CompilerInstance;
}

class XFrontendAction : public clang::ASTFrontendAction
{
  private:
    CommandOptions options;

  public:
    explicit XFrontendAction(const CommandOptions &opts = CommandOptions{});
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &compiler,
                                                                  llvm::StringRef inFile) override;
};

#endif
