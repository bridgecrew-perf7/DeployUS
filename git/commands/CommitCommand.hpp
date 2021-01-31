#pragma once
#include <objects/GitCommit.hpp>
using namespace std;


namespace CommitCommand
{
    //Creates commit object and necessary tree objects
    //Clears index file
    //Updates HEAD
    //Returns:  non-zero if an error occured
    //          zero otherwise
    int execute(int argc, char* argv[]);

    //Sends usage message to stdout. Always returns 0;
    int help();

    //Remove content of Index file
    void clearIndex();

    //Update HEAD to contain SHA1 of GitCommit obj
    void updateHEAD(GitCommit* obj);

} // namespace CommitCommand
