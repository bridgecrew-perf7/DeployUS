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

    //1. If .git folder already exists, then print to screen that a local git repo already exists. Else, create folder
    if(fs::exists(".git")) 
    {
        cout << "Error: A git repo already exists here."<< endl;
        return 1;
    }
    else 
    {
        if(!fs::create_directory(".git"))
        {
            cout << "Failed to create .git folder." <<endl;
            return 1;
        }
    }

    //2. Create empty object folder
    if(!fs::create_directory(".git/objects"))
    {
        cout << "Failed to create .git/objects folder." <<endl;
        return 1;
    }
    
    //3. Create empty files index and HEAD files
    ofstream file(".git/HEAD");
    file.close();
    file.open(".git/index");
    file.close();

    return 0;
}

void InitCommand::help() {
    std::cout   <<    "usage: gitus init\n";
}