#include "CheckoutCommand.hpp"
#include <common.hpp>
#include <objects/GitCommit.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <iostream>
#include <string>


int CheckoutCommand::execute(int argc, char* argv[])
//Removes tracked files from current HEAD commit
//Restores files in checked out commit
//Updates HEAD
//Creates TOP_COMMIT file that will store reference of most recent commit. Used in Detached HEAD mode
//Returns:  non-zero if an error occured
//          zero otherwise
{
    //Verify if help is wanted
    if(argc > 2)
    {
        string option = argv[2];
        if(option.compare(Common::HELP_PARAM) == 0)
            return help();
    }

    //1. Argument verification
    if(!fs::exists(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: The git repository has not been initiated.\n";
        return 1;
    }
    if(argc != 3)
    {
        std::cout << "Error: Invalid usage of command\n";
        help();
        return 1;
    }

    //2. Get GitCommit obj of commit that will be checkout-ed. 
    string wantedCommitID = argv[2];
    if( !Common::isValidSHA1(wantedCommitID))   //Valid SHA1
    {
        std::cout << "Error: Not a valid commitID\n";
        return 1;
    }
    GitCommit* wantedCommitObj = GitCommit::createFromGitObject(wantedCommitID);

    //3. Get current HEAD commit obj.
    string currentCommitID = Common::readFile(GitFilesystem::getHEADPath());
    if(currentCommitID.size() == 0)
    {
        std::cout << "Error: The commit history is empty.\n";
        return 1;
    }
    GitCommit* currentCommitObj = GitCommit::createFromGitObject(currentCommitID);

    //3. Remove currently tracked files
    if(currentCommitObj->rmTrackedFiles())
    {
        std::cout << "Error: Could not remove files.\n";
        currentCommitObj->restoreTrackedFiles();
        return 1;
    }         

    //4. Restore wanted (i.e. checked out) version of repository
    if(wantedCommitObj->restoreTrackedFiles())
    {
        std::cout << "Error: Could not restore files.\n";
        //Restore state
        wantedCommitObj->rmTrackedFiles();
        currentCommitObj->restoreTrackedFiles();
        return 1;
    }

    //4. Place hash of top commit in TOP_COMMIT file. The existence of the TOP_COMMIT file means we are in Detached HEAD mode
    if(!fs::exists(GitFilesystem::getTOPCOMMITPath()))
    {
        if(Common::writeFile(GitFilesystem::getTOPCOMMITPath(), currentCommitObj->getSHA1Hash()))
        {
            std::cout << "Error: Could not write to TOP_COMMIT file.\n";
            //Restore state
            wantedCommitObj->rmTrackedFiles();
            currentCommitObj->restoreTrackedFiles();
            return 1;
        }
    }
    //Verify if still in Detached HEAD mode
    else 
    {
        string topcommitContents = Common::readFile(GitFilesystem::getTOPCOMMITPath());
        // No longer in Detached HEAD mode. Remove TOP_COMMIT file
        if (topcommitContents.compare(wantedCommitObj->getSHA1Hash()) == 0)
        {
            //Safely remove TOP_COMMIT
            if(Common::safeRemove(GitFilesystem::getTOPCOMMITPath())) //No longer in Detached HEAD mode.
            {
                std::cout << "Error: Could not write to TOP_COMMIT file.\n";
                //Restore state
                wantedCommitObj->rmTrackedFiles();
                currentCommitObj->restoreTrackedFiles();
                Common::writeFile(GitFilesystem::getTOPCOMMITPath(), currentCommitObj->getSHA1Hash());
                return 1;
            }
        }
    }

    //5. Update HEAD
    if(Common::writeFile(GitFilesystem::getHEADPath(), wantedCommitObj->getSHA1Hash()))
    {
        std::cout << "Error: Could not write to HEAD file.\n";
        //Restore state
        wantedCommitObj->rmTrackedFiles();
        currentCommitObj->restoreTrackedFiles();
        Common::writeFile(GitFilesystem::getTOPCOMMITPath(), currentCommitObj->getSHA1Hash());
        return 1;
    }

    //Reclaim memory
    delete currentCommitObj;
    delete wantedCommitObj;

    return 0;
}

int CheckoutCommand::help()
//Sends usage message to stdout. Always returns 0;
{
    std::cout << "usage: gitus checkout <commitID>\n";
    return 0;
}