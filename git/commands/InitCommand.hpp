#pragma once

namespace InitCommand
{

    //Displays command usage. Always returns 0.
    int help();

    //Initializes .git folder.
    //Returns : non-zero if an error occurs
    //          zero if successful execution
    int execute(int argc, char* argv[]);
}