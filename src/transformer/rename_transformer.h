#ifndef RENAME_TRANSFORMER_HPP
#define RENAME_TRANSFORMER_HPP

#include "transformer.h"
#include <string>

namespace clang
{
    class ASTContext;
    class Rewriter;
} // namespace clang

class RenameTransformer : public Transformer
{
  public:
    explicit RenameTransformer(clang::ASTContext &context, clang::Rewriter &rewriter, const std::string &fromName,
                               const std::string &toName, const std::string &renameType);

    virtual void start() override;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
    virtual void print(clang::raw_ostream &stream) override;

  private:
    void handleFunctionRename(const clang::ast_matchers::MatchFinder::MatchResult &result);
    void handleVariableRename(const clang::ast_matchers::MatchFinder::MatchResult &result);
    void handleVariableRefRename(const clang::ast_matchers::MatchFinder::MatchResult &result);

    std::string fromName;
    std::string toName;
    std::string renameType;
    int renameCount;
};

#endif