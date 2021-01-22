#pragma once
#include "BaseCommand.hpp"
#include <iostream>
using namespace std;

class UnknownCommand: public BaseCommand
{
private:
public:
    UnknownCommand();
    ~UnknownCommand();

    inline virtual int execute() {cout<<"Unknown command.\n"; return 1;};
};
