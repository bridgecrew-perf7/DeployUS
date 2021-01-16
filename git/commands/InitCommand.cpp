#include "InitCommand.hpp"
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;

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
    const fs::path gitDirectory(".git");
    const fs::path objectsDirectory = fs::path(".git").append("objects");
    const fs::path indexDirectory = fs::path(".git").append("index");
    const fs::path headDirectory = fs::path(".git").append("HEAD");
    

    //1. If .git folder already exists, then print to screen that a local git repo already exists. Else, create folder
    if(fs::exists(gitDirectory)) 
    {
        cout << "Error: A git repo already exists here."<< endl;
        return 1;
    }
    else 
    {
        if(!fs::create_directory(gitDirectory))
        {
            cout << "Failed to create .git folder." <<endl;
            return 1;
        }
    }

    //2. Create empty object folder
    if(!fs::create_directory(objectsDirectory))
    {
        cout << "Failed to create "<< objectsDirectory.c_str() << "folder." <<endl;
        return 1;
    }
    
    //3. Create empty files index and HEAD files
    ofstream file(headDirectory.c_str());
    file.close();
    file.open(indexDirectory.c_str());
    file.close();

    return 0;
}

void InitCommand::help() {
    std::cout << "usage: gitus init\n";
}