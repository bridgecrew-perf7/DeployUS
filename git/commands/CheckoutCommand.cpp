#include "CheckoutCommand.hpp"
#include <common.hpp>
#include <objects/GitCommit.hpp>
#include <iostream>
#include <string>

using namespace std;

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
    if(numArgs != 3)
    {
        cout << "Error: Invalid usage of command\n";
        help();
        return 1;
    }
    string commitID = args[2];
    if(commitID.size() != 40)
    {
        cout << "Error: Not a valid commitID\n";
        return 1;
    }
    if(commitID.find_first_not_of("0123456789abcdefABCDEF") != string::npos)
    {
        cout << "Error: commitID is not a hex string.\n";
        return 1;
    }

    //Get GitCommit obj described by commitID
    GitCommit* wantedCommitObj = GitCommit::createFromGitObject(commitID);

    //Get Current HEAD commit
    string currentCommitID = readFile(getHEADPath());
    GitCommit* currentCommitObj = GitCommit::createFromGitObject(currentCommitID);

    //1. Remove all presently tracked files
    currentCommitObj->rmTrackedFiles();

    //2. Restore tracked files
    wantedCommitObj->restoreTrackedFiles();

    //3. Place hash of top commit in TOP_COMMIT file. If the top commit is being checked out, remove TOP_COMMIT file
    if(!fs::exists(GitFilesystem::getTOPCOMMITPath()))
    {
        writeFile(GitFilesystem::getTOPCOMMITPath(), currentCommitObj->getSHA1Hash());
    }
    else if (readFile(GitFilesystem::getTOPCOMMITPath()).compare(wantedCommitObj->getSHA1Hash()) == 0)
    {
        fs::remove(GitFilesystem::getTOPCOMMITPath());
    }

    //4. Update HEAD
    writeFile(GitFilesystem::getHEADPath(), wantedCommitObj->getSHA1Hash());

    return 0;
}

void CheckoutCommand::help() {
    cout << "usage: gitus checkout <commitID>\n";
}
