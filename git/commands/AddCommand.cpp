#include "AddCommand.hpp"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>

namespace fs = boost::filesystem;
using boost::uuids::detail::sha1;
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
    //1. Verify that there is one file to be added.
    if (numArgs < 3)
    {
        cout << "Error: no filepath specified.";
        return 1;
    }
    else if (numArgs > 3)
    {
        cout << "Warning, only adding first file specified.";
    }
    
    //2. Verify that the file specified exists
    if (!fs::is_regular_file(args[2])) 
    {
        cout << "Error: File does not exists.";
        return 1;
    }

    //3. Find SHA-1 of file
    sha1 sha;
    sha.process_bytes("Shawn", 5);
    cout << sha. <<endl;

    return 0;
}

void AddCommand::help() {
    std::cout << "usage: gitus add <pathspec>\n";
}