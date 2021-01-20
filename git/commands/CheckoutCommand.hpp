#pragma once
#include "BaseCommand.hpp"

class CheckoutCommand: public BaseCommand
{
private:
    int numArgs;
    char** args;

public:
    CheckoutCommand(int argc, char* argv[]);
    ~CheckoutCommand();

    virtual int execute();
    virtual void help();
};