#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>

namespace clang
{
    class ASTContext;
    class Rewriter;
} // namespace clang

class Transformer : public clang::ast_matchers::MatchFinder::MatchCallback
{
  protected:
    clang::ASTContext &context;
    clang::Rewriter &rewriter;

  public:
    explicit Transformer(clang::ASTContext &context, clang::Rewriter &rewriter);

    virtual void start() = 0;
    virtual void print(clang::raw_ostream &stream) = 0;
};

#endif
