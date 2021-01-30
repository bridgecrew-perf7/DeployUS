#pragma once
#include "BaseCommand.hpp"
#include <iostream>

class UnknownCommand: public BaseCommand
{
private:
public:
    UnknownCommand();
    ~UnknownCommand();

    inline virtual int execute() {std::cout<<"Unknown command.\n"; return 1;};
};
