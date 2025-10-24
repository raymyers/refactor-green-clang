#ifndef RENAME_TRANSFORMER_HPP
#define RENAME_TRANSFORMER_HPP

#include "transformer.h"

#include <set>
#include <string>

namespace clang
{
    class ASTContext;
    class Rewriter;
} // namespace clang

class RenameTransformer : public Transformer
{
  private:
    std::string oldName;
    std::string newName;
    int targetLine; // -1 means rename all occurrences
    std::set<std::string> renamedIdentifiers;

  public:
    explicit RenameTransformer(clang::ASTContext &context, clang::Rewriter &rewriter, 
                              const std::string &oldName, const std::string &newName, 
                              int targetLine = -1);

    virtual void start() override;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
    virtual void print(clang::raw_ostream &stream) override;

  private:
    bool shouldRename(const clang::SourceLocation &location) const;
};

#endif