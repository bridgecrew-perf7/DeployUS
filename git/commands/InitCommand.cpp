#include "InitCommand.hpp"
#include <iostream>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>

namespace fs = boost::filesystem;

InitCommand::InitCommand(int argc, char* argv[])
{
    numArgs = argc;
    args = argv;
}

InitCommand::~InitCommand()
{
}

int InitCommand::execute() {
    //Creation of .git folder with folder .git/objects, and files .git/index, .git/HEAD
    const fs::path gitDirectory(GITFILESYSTEM_DOTGIT_FOLDER_NAME);
    const fs::path objectsDirectory = fs::path(gitDirectory).append(GITFILESYSTEM_OBJECTS_FOLDER_NAME);
    const fs::path indexDirectory = fs::path(gitDirectory).append(GITFILESYSTEM_INDEX_FILE_NAME);
    const fs::path headDirectory = fs::path(gitDirectory).append(GITFILESYSTEM_HEAD_FILE_NAME);
    

    //1. If .git folder already exists, then print to screen that a local git repo already exists. Else, create folder
    if(fs::exists(gitDirectory)) 
    {
        std::cout << "Error: A git repo already exists here."<< std::endl;
        return 1;
    }
    else 
    {
        if(!fs::create_directory(gitDirectory))
        {
            std::cout << "Failed to create .git folder." << std::endl;
            return 1;
        }
    }

    //2. Create empty object folder
    if(!fs::create_directory(objectsDirectory))
    {
        std::cout << "Failed to create "<< objectsDirectory.c_str() << "folder." << std::endl;
        return 1;
    }
    
    //3. Create empty files index and HEAD files
    std::ofstream file(headDirectory.c_str());
    file.close();
    file.open(indexDirectory.c_str());
    file.close();

    return 0;
}

void InitCommand::help() {
    std::cout << "usage: gitus init\n";
}