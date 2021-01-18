#pragma once

#include "BaseCommand.hpp"

class AddCommand: public BaseCommand
{
private:
    int numArgs;
    char** args;
public:
    AddCommand(int argc, char* argv[]);
    ~AddCommand();

    virtual int execute();
    virtual void help();
};
