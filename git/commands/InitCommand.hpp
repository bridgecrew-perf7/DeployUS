#pragma once

#include "BaseCommand.hpp"

class InitCommand: public BaseCommand
{
private:
    
public:
    InitCommand(int argc, char* argv[]);
    ~InitCommand();

    virtual void execute();
    virtual void help();
};
