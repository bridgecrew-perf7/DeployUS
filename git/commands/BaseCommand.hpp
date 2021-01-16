#pragma once

//Base class for commands

class BaseCommand
{
public:
    BaseCommand(/* args */);
    ~BaseCommand();

    //Method to be overwritten
    virtual int execute();
    virtual void help();
};