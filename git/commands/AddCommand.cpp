#include "AddCommand.hpp"
#include <objects/GitBlob.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>

namespace fs = boost::filesystem;

AddCommand::AddCommand(int argc, char* argv[])
{
    numArgs = argc;
    args = argv;
}

AddCommand::~AddCommand()
{
}

int AddCommand::execute() {
    //1. Verify that a git folder has been initiated
    if (!fs::exists(GitFilesystem::getDotGitPath()) || !fs::is_directory(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: No git repository has been found.\n";
        return 1;
    }
    
    //2. Verify that there is one file to be added.
    if (numArgs < 3)
    {
        std::cout << "Error: no filepath specified.\n";
        return 1;
    }
    else if (numArgs > 3)
    {
        std::cout << "Warning, only adding first file specified.\n";
    }

    //3. Verify that the file specified exists

    // AB - quand je vois un chiffre, je me pose beaucoup de question (surtout quand c'est utilisé comme un index).
    //      Le but est d'avoir du code auto-documenté. Par exemple:
    auto fileNameToAdd = args[2]; // ca dit exactement ce que c'est et à quoi le chiffre sert.

    if (!fs::is_regular_file(args[2])) 
    {
        std::cout << "Error: File does not exists.\n";
        return 1;
    }

    //4. Create GitBlob object
    GitBlob* gitblob = new GitBlob(args[2]);

    //Verify that the file can be added
    if( gitblob->isinIndex())
    {
        std::cout << "Error: File is already staged.\n";
        return 1;
    }
    if(gitblob->isTracked())
    {
        std::cout << "Error: File is already being tracked\n";
        return 1;
    }

    //5. Add blob file in object directory
    gitblob->addInObjects();

    //6. Add reference to blob file in the index file.
    if (gitblob->addInIndex())
    {
        std::cout << "Error: File could not be added in Index file.\n";
        return 1;
    }
    

    return 0;
}

void AddCommand::help() {
    std::cout << "usage: gitus add <pathspec>\n";
}
