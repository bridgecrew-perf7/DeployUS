#include "InitCommand.hpp"
#include <iostream>

InitCommand::InitCommand(int argc, char* argv[])
{
}

InitCommand::~InitCommand()
{
}

void InitCommand::execute() {
    
}

void InitCommand::help() {
    std::cout   <<    "usage: gitus init\n";
}