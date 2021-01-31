#include "AddCommand.hpp"
#include <objects/GitBlob.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>

namespace fs = boost::filesystem;

//Sends usage message to stdout. Always returns 0;
int AddCommand::help() 
{
    std::cout << "usage: gitus add <pathspec>\n";
    return 0;
}

//Adds file to the object folder.
//Updates index file.
//Returns:  non-zero if an error occured
//          zero otherwise
int AddCommand::execute(int argc, char* argv[]) {
    //1. Verify that a git folder has been initiated
    if (!fs::exists(GitFilesystem::getDotGitPath()) || !fs::is_directory(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: No git repository has been found.\n";
        return 1;
    }
    
    //2. Verify that there is one file to be added.
    if (argc < 3)
    {
        std::cout << "Error: no filepath specified.\n";
        return 1;
    }
    else if (argc > 3)
    {
        std::cout << "Warning: only adding first file specified.\n";
    }

    //3. Verify that the file specified exists or --help 
    string argument = argv[2]; 
    if(argument.compare(Common::HELP_PARAM) == 0)      //3.1 option --help is not a file!
        return help();
    else if (!fs::is_regular_file(argument))           //3.2 Any other argument is considered a file
    {
        std::cout << "Error: File does not exists.\n";
        return 1;
    }

    //4. Create GitBlob object
    GitBlob gitblob(argument);

    //5. Verify that the file can be added
    if( gitblob.isinIndex())
    {
        std::cout << "Error: File is already staged.\n";
        return 1;
    }
    if(gitblob.isTracked())
    {
        std::cout << "Error: File is already being tracked\n";
        return 1;
    }

    //6. Add blob file in object directory
    gitblob.addInObjects();

    //7. Add reference to blob file in the index file.
    if (gitblob.addInIndex())
    {
        std::cout << "Error: File could not be added in Index file.\n";
        return 1;
    }
    

    return 0;
}
