#pragma once

#include "BaseCommand.hpp"

class HelpCommand: public BaseCommand
{
private:
    BaseCommand* bc;
public:
    HelpCommand(BaseCommand* bcptr);
    ~HelpCommand();

    virtual int execute();
};
