#include <iostream>
#include "BaseCommand.hpp"

BaseCommand::BaseCommand(/* args */)
{
}

BaseCommand::~BaseCommand()
{
}

int BaseCommand::execute() {
    return 0;
}

void BaseCommand::help() {
    std::cout   <<    "usage: gitus <command> [<args>]\n"
                <<    "These are common gitus commands used in various situations:\n"
                <<    "init Create an empty Git repository or reinitialize an existing one\n"
                <<    "add Add file contents to the index\n"
                <<    "commit Record changes to the repository\n";
}