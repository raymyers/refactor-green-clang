#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "actions/frontendaction.h"
#include "utils/utils.h"

#include <string>

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;

// Command line options
static llvm::cl::OptionCategory ctCategory("clang-tool options");

static llvm::cl::opt<std::string> RenameFrom("rename-from", llvm::cl::desc("Name to rename from"),
                                             llvm::cl::value_desc("old_name"), llvm::cl::cat(ctCategory));

static llvm::cl::opt<std::string> RenameTo("rename-to", llvm::cl::desc("Name to rename to"),
                                           llvm::cl::value_desc("new_name"), llvm::cl::cat(ctCategory));

static llvm::cl::opt<std::string> RenameType("rename-type",
                                             llvm::cl::desc("Type of entity to rename (function, variable)"),
                                             llvm::cl::value_desc("type"), llvm::cl::init("function"),
                                             llvm::cl::cat(ctCategory));

int main(int argc, const char **argv)
{
    auto expectedParser = CommonOptionsParser::create(argc, argv, ctCategory);
    if (!expectedParser)
    {
        llvm::errs() << expectedParser.takeError();
        return -1;
    }

    CommonOptionsParser &optionsParser = expectedParser.get();
    for (auto &sourceFile : optionsParser.getSourcePathList())
    {
        if (utils::fileExists(sourceFile) == false)
        {
            llvm::errs() << "File: " << sourceFile << " does not exist!\n";
            return -1;
        }

        auto sourcetxt = utils::getSourceCode(sourceFile);
        auto compileCommands = optionsParser.getCompilations().getCompileCommands(getAbsolutePath(sourceFile));

        std::vector<std::string> compileArgs = utils::getCompileArgs(compileCommands);
        compileArgs.push_back("-I" + utils::getClangBuiltInIncludePath(argv[0]));
        for (auto &s : compileArgs)
            llvm::outs() << s << "\n";

        std::unique_ptr<XFrontendAction> xfrontendAction;

        if (!RenameFrom.empty() && !RenameTo.empty())
        {
            xfrontendAction = std::make_unique<XFrontendAction>(RenameFrom, RenameTo, RenameType);
        }
        else
        {
            xfrontendAction = std::make_unique<XFrontendAction>();
        }

        utils::customRunToolOnCodeWithArgs(move(xfrontendAction), sourcetxt, compileArgs, sourceFile);
    }

    return 0;
}
