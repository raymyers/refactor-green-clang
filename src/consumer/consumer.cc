#include "consumer.h"

#include "../finder/integervariablefinder.h"
#include "../transformer/functioncalltransformer.h"
#include "../transformer/renametransformer.h"

XConsumer::XConsumer(clang::ASTContext &context, const CommandOptions &opts)
    : options(opts)
{
}

void XConsumer::HandleTranslationUnit(clang::ASTContext &context)
{
    rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

    if (options.isRenameCommand) {
        // Use RenameTransformer for rename operations
        RenameTransformer renameTransformer(context, rewriter, 
                                          options.oldName, options.newName, 
                                          options.targetLine);
        renameTransformer.start();
        renameTransformer.print(llvm::outs());
    } else {
        // Default behavior: use FunctionCallTransformer
        FunctionCallTransformer fntransformer(context, rewriter);
        fntransformer.start();
        fntransformer.print(llvm::outs());
    }

    // IntegerVariableFinder intFinder(context);
    // intFinder.start();

    auto buffer = rewriter.getRewriteBufferFor(context.getSourceManager().getMainFileID());

    if (buffer != nullptr)
        buffer->write(llvm::outs());
}
