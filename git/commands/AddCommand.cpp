#include "AddCommand.hpp"
#include <objects/GitBlob.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;

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
        cout << "Error: No git repository has been found.\n";
        return 1;
    }
    
    //2. Verify that there is one file to be added.
    if (numArgs < 3)
    {
        cout << "Error: no filepath specified.\n";
        return 1;
    }
    else if (numArgs > 3)
    {
        cout << "Warning, only adding first file specified.\n";
    }

    //3. Verify that the file specified exists
    if (!fs::is_regular_file(args[2])) 
    {
        cout << "Error: File does not exists.\n";
        return 1;
    }

    //4. Create GitBlob object
    GitBlob* gitblob = new GitBlob(args[2]);

    //5. Add blob file in object directory
    if( gitblob->addInObjects() )
    {
        cout << "Error: File is already added.\n";
        return 1;
    }

    //6. Add reference to blob file in the index file.
    if (gitblob->addInIndex())
    {
        cout << "Error: File could not be added in Index file.\n";
        return 1;
    }
    

    return 0;
}

void AddCommand::help() {
    cout << "usage: gitus add <pathspec>\n";
}
