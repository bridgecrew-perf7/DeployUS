#include "HelpCommand.hpp"
#include <iostream>

HelpCommand::HelpCommand(BaseCommand* bcptr)
{
    this->bc = bcptr;
}

HelpCommand::~HelpCommand()
{
}

int HelpCommand::execute() {
    bc->help();
    return 0;
}


