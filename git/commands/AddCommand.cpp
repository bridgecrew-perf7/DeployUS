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
    //1. Verify if help is needed
    if(argc > 2)
    {
        string option = argv[2];
        if(option.compare(Common::HELP_PARAM) == 0)
            return help();
    }

    //2. Verify that a git folder has been initiated
    if (!fs::exists(GitFilesystem::getDotGitPath()) || !fs::is_directory(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: No git repository has been found.\n";
        return 1;
    }
    
    //3. Verify that there is one file to be added.
    if (argc < 3)
    {
        std::cout << "Error: no filepath specified.\n";
        return 1;
    }
    else if (argc > 3)
    {
        std::cout << "Warning: only adding first file specified.\n";
    }

    //4. Verify that the file specified exists
    string argument = argv[2]; 
    if (!fs::is_regular_file(argument))           
    {
        std::cout << "Error: File is not a regular file.\n";
        return 1;
    }

    //5. Create GitBlob object
    GitBlob gitblob(argument);

    //6. Verify that the file can be added
    if(gitblob.isinIndex())
    {
        std::cout << "Error: File is already staged.\n";
        return 1;
    }
    if(gitblob.isTracked())
    {
        std::cout << "Error: File is already being tracked\n";
        return 1;
    }

    //7. Add blob file in object directory
    gitblob.addInObjects();

    //8. Add reference to blob file in the index file.
    if (gitblob.addInIndex())
    {
        std::cout << "Error: File could not be added in Index file.\n";
        return 1;
    }

    return 0;
}
