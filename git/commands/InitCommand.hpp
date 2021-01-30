#pragma once

#include "BaseCommand.hpp"

class InitCommand: public BaseCommand
{
private:
    int numArgs;
    char** args;
public:
    InitCommand(int argc, char* argv[]);
    ~InitCommand(); // AB - si tu utilise du polymorphisme, un destructeur virtuel est n�cessaire

    virtual int execute();
    virtual void help();
};
