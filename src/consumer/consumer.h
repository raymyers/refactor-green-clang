#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Rewrite/Core/Rewriter.h>

namespace clang
{
    class ASTContext;
}

class XConsumer : public clang::ASTConsumer
{
  private:
    clang::Rewriter rewriter;
    std::string renameFrom;
    std::string renameTo;
    std::string renameType;
    bool hasRenameOptions;

  public:
    explicit XConsumer(clang::ASTContext &context);
    explicit XConsumer(clang::ASTContext &context, const std::string &renameFrom, const std::string &renameTo,
                       const std::string &renameType);
    virtual void HandleTranslationUnit(clang::ASTContext &context) override;
};

#endif
