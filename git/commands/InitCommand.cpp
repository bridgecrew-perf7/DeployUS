#include "InitCommand.hpp"
#include <iostream>
#include <string>
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
    //1. If .git folder already exists, then print to screen that a local git repo already exists. Else, create folder
    if(fs::exists(GitFilesystem::getDotGitPath())) 
    {
        std::cout << "Error: A git repo already exists here."<< std::endl;
        return 1;
    }
    else 
    {
        //Create .git folder
        std::string errMsg = "Failed to create .git folder.";
        try
        {
            if(!fs::create_directory(GitFilesystem::getDotGitPath()))
            {
                std::cout << errMsg << std::endl;
                return 1;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cout << errMsg << std::endl;
            return 1;
        }
    }

    //2. Create empty object folder
    std::string errMsg = std::string("Failed to create ") + GitFilesystem::getObjectsPath().string() + std::string("folder.");
    try
    {
        if(!fs::create_directory(GitFilesystem::getObjectsPath()))
        {
            std::cout << errMsg << std::endl;
            return 1;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
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

void InitCommand::help() 
{ 
    std::cout << "usage: gitus init\n";
}