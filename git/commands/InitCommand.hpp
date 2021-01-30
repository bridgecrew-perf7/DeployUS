#pragma once

#include "BaseCommand.hpp"

class InitCommand: public BaseCommand
{
private:
    int numArgs;
    char** args;
public:
    InitCommand(int argc, char* argv[]);
    virtual ~InitCommand();

    virtual int execute();
    virtual void help();
};
