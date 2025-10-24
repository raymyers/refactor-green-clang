#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "actions/frontendaction.h"
#include "consumer/consumer.h"
#include "utils/utils.h"

#include <cstdlib>
#include <sstream>
#include <string>

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;

// Global variables to store command options
CommandOptions g_commandOptions;

// Function to parse rename command arguments
bool parseRenameCommand(int argc, const char **argv)
{
    if (argc < 4)
        return false;

    string command = argv[1];
    if (command != "rename")
        return false;

    g_commandOptions.isRenameCommand = true;

    // Parse oldname (potentially with line number)
    string oldNameArg = argv[2];
    size_t colonPos = oldNameArg.find(':');
    if (colonPos != string::npos)
    {
        g_commandOptions.oldName = oldNameArg.substr(0, colonPos);
        string lineStr = oldNameArg.substr(colonPos + 1);

        // Simple integer parsing without exceptions
        char *endPtr;
        long lineNum = strtol(lineStr.c_str(), &endPtr, 10);
        if (*endPtr != '\0' || lineNum <= 0)
        {
            llvm::errs() << "Invalid line number: " << lineStr << "\n";
            return false;
        }
        g_commandOptions.targetLine = static_cast<int>(lineNum);
    }
    else
    {
        g_commandOptions.oldName = oldNameArg;
    }

    g_commandOptions.newName = argv[3];

    return true;
}

int main(int argc, const char **argv)
{
    // Check if this is a rename command
    vector<const char *> newArgv;
    if (parseRenameCommand(argc, argv))
    {
        // For rename command, adjust argv to skip the rename-specific arguments
        // and pass the remaining arguments to CommonOptionsParser
        newArgv.push_back(argv[0]); // program name
        for (int i = 4; i < argc; ++i)
        {
            newArgv.push_back(argv[i]);
        }
        argc = static_cast<int>(newArgv.size());
        argv = newArgv.data();
    }

    llvm::cl::OptionCategory ctCategory("clang-tool options");
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

        auto xfrontendAction = std::make_unique<XFrontendAction>(g_commandOptions);
        utils::customRunToolOnCodeWithArgs(move(xfrontendAction), sourcetxt, compileArgs, sourceFile);
    }

    return 0;
}
