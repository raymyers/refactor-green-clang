#include "consumer.h"

#include "../finder/integervariablefinder.h"
#include "../transformer/comprehensive_transformer.h"
#include "../transformer/functioncalltransformer.h"
#include "../transformer/rename_transformer.h"

XConsumer::XConsumer(clang::ASTContext &context) : hasRenameOptions(false)
{
}

XConsumer::XConsumer(clang::ASTContext &context, const std::string &renameFrom, const std::string &renameTo,
                     const std::string &renameType)
    : renameFrom(renameFrom), renameTo(renameTo), renameType(renameType), hasRenameOptions(true)
{
}

void XConsumer::HandleTranslationUnit(clang::ASTContext &context)
{
    rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

    if (hasRenameOptions && !renameFrom.empty() && !renameTo.empty())
    {
        // Use rename transformer when specific rename is requested
        RenameTransformer transformer(context, rewriter, renameFrom, renameTo, renameType);
        transformer.start();
        transformer.print(llvm::outs());
    }
    else
    {
        // Use comprehensive analysis when no specific rename is requested
        ComprehensiveTransformer transformer(context, rewriter);
        transformer.start();
        transformer.print(llvm::outs());
    }

    auto buffer = rewriter.getRewriteBufferFor(context.getSourceManager().getMainFileID());

    if (buffer != nullptr)
        buffer->write(llvm::outs());
}
