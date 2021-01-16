#include "HelpCommand.hpp"
#include <iostream>

HelpCommand::HelpCommand(BaseCommand* bcptr)
{
    this->bc = bcptr;
}

HelpCommand::~HelpCommand()
{
}

void HelpCommand::execute() {
    bc->help();
}


