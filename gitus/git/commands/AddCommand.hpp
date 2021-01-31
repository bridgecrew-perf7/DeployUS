#pragma once

namespace AddCommand
{
    //Adds file to the object folder.
    //Updates index file.
    //Returns:  non-zero if an error occured
    //          zero otherwise
    int execute(int argc, char* argv[]);

    //Sends usage message to stdout. Always returns 0;
    int help();
}