#ifndef COMPREHENSIVE_TRANSFORMER_HPP
#define COMPREHENSIVE_TRANSFORMER_HPP

#include "transformer.h"

#include <map>
#include <set>
#include <string>

namespace clang
{
    class ASTContext;
    class Rewriter;
} // namespace clang

class ComprehensiveTransformer : public Transformer
{
  protected:
    std::set<std::string> user_functions;
    std::set<std::string> variables;
    std::set<std::string> builtin_functions;
    std::map<std::string, unsigned> builtin_ids;

  public:
    explicit ComprehensiveTransformer(clang::ASTContext &context, clang::Rewriter &rewriter);

    virtual void start() override;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
    virtual void print(clang::raw_ostream &stream) override;

  private:
    void handleFunctionCall(const clang::ast_matchers::MatchFinder::MatchResult &result);
    void handleVariableDecl(const clang::ast_matchers::MatchFinder::MatchResult &result);
    void handleVariableRef(const clang::ast_matchers::MatchFinder::MatchResult &result);
};

#endif