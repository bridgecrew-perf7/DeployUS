#pragma once

#include "BaseCommand.hpp"
#include <string>

class AddCommand: public BaseCommand
{
private:
    std::string sha1hash;
    int numArgs;
    char** args;
public:
    AddCommand(int argc, char* argv[]);
    ~AddCommand();

    virtual int execute();
    virtual void help();
};
