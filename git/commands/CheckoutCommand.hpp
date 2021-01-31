#pragma once
#include <objects/GitCommit.hpp>

namespace CheckoutCommand
{
    //Removes tracked files from current HEAD commit
    //Restores files in checked out commit
    //Updates HEAD
    //Creates TOP_COMMIT file that will store reference of most recent commit. Used in Detached HEAD mode
    //Returns:  non-zero if an error occured
    //          zero otherwise
    int execute(int argc, char* argv[]);

    //Sends usage message to stdout. Always returns 0;
    int help();

    int updateTOPCOMMIT(GitCommit* wantedCommitObj, GitCommit* currentCommitObj);

} // namespace CheckoutCommand