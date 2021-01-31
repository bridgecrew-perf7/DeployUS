#include "InitCommand.hpp"
#include <common.hpp>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>
namespace fs = boost::filesystem;

int InitCommand::help() 
//Displays command usage. Always returns 0.
{
    std::cout << "usage: gitus init\n";
    return 0;
}

int InitCommand::execute(int argc, char* argv[]) 
//Initializes .git folder.
//Returns : non-zero if an error occurs
//          zero if successful execution
{
    //1. Check for help command
    if(argc > 2)
    {
        string option = argv[2];
        if(option.compare(Common::HELP_PARAM) == 0)
            return help();
    }

    //2. If .git folder already exists, then print to screen that a local git repo already exists. Else, create folder
    if(fs::exists(GitFilesystem::getDotGitPath())) 
    {
        std::cout << "Error: A git repo already exists here."<< std::endl;
        return 1;
    }
    else 
    {
        //Create .git folder
        if(Common::safeCreateFolder(GitFilesystem::getDotGitPath()))
        {
            std::cout << "Failed to create .git folder." << std::endl;
            return 1;
        }
    }

    //2. Create empty object folder
    if(Common::safeCreateFolder(GitFilesystem::getObjectsPath()))
    {
        std::string errMsg = std::string("Failed to create ") + GitFilesystem::getObjectsPath().string() + std::string(" folder.");
        std::cout << errMsg << std::endl;
        return 1;
    }
    
    //3. Create empty files index and HEAD files
    std::ofstream fileHEAD(GitFilesystem::getHEADPath().c_str());
    fileHEAD.close();
    std::ofstream fileIndex(GitFilesystem::getIndexPath().c_str());
    fileIndex.close();

    return 0;
}