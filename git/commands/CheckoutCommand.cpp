#include "CheckoutCommand.hpp"
#include <common.hpp>
#include <objects/GitCommit.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <iostream>
#include <string>

CheckoutCommand::CheckoutCommand(int argc, char* argv[])
{
    numArgs = argc;
    args = argv;
}

CheckoutCommand::~CheckoutCommand()
{
}

int CheckoutCommand::execute()
//Performs checkout. Returns 0 if successful, non-zero otherwise.
{
    //Argument verification
    if(!fs::exists(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: The git repository has not been initiated.\n";
        return 1;
    }
    if(numArgs != 3)
    {
        std::cout << "Error: Invalid usage of command\n";
        help();
        return 1;
    }
    string commitID = args[2];
    if(commitID.size() != 40)
    {
        std::cout << "Error: Not a valid commitID\n";
        return 1;
    }
    if(commitID.find_first_not_of("0123456789abcdefABCDEF") != string::npos)
    {
        std::cout << "Error: commitID is not a hex string.\n";
        return 1;
    }

    string currentCommitID = Common::readFile(GitFilesystem::getHEADPath());
    if(currentCommitID.size() == 0)
    {
        std::cout << "Error: The commit history is empty.\n";
        return 1;
    }

    //Get GitCommit obj described by commitID
    GitCommit* wantedCommitObj = GitCommit::createFromGitObject(commitID);

    //Get Current HEAD commit
    GitCommit* currentCommitObj = GitCommit::createFromGitObject(currentCommitID);

    //1. Remove all presently tracked files
    currentCommitObj->rmTrackedFiles();

    //2. Restore tracked files
    wantedCommitObj->restoreTrackedFiles();

    //3. Place hash of top commit in TOP_COMMIT file. If the top commit is being checked out, remove TOP_COMMIT file
    if(!fs::exists(GitFilesystem::getTOPCOMMITPath()))
    {
        Common::writeFile(GitFilesystem::getTOPCOMMITPath(), currentCommitObj->getSHA1Hash());
    }
    else if (Common::readFile(GitFilesystem::getTOPCOMMITPath()).compare(wantedCommitObj->getSHA1Hash()) == 0)
    {
        fs::remove(GitFilesystem::getTOPCOMMITPath());
    }

    //4. Update HEAD
    Common::writeFile(GitFilesystem::getHEADPath(), wantedCommitObj->getSHA1Hash());

    //Reclaim memory
    delete currentCommitObj;
    delete wantedCommitObj;

    return 0;
}

void CheckoutCommand::help() {
    std::cout << "usage: gitus checkout <commitID>\n";
}
