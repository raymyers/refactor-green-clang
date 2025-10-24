#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include <string>

namespace clang
{
    class ASTContext;
}

struct CommandOptions
{
    bool isRenameCommand = false;
    std::string oldName;
    std::string newName;
    int targetLine = -1;
};

class XConsumer : public clang::ASTConsumer
{
  private:
    clang::Rewriter rewriter;
    CommandOptions options;

  public:
    explicit XConsumer(clang::ASTContext &context, const CommandOptions &opts = CommandOptions{});
    virtual void HandleTranslationUnit(clang::ASTContext &context) override;
};

#endif
