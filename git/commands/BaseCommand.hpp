#pragma once

#include <string>

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

std::string generateSHA1(std::string text);
std::string generateBlobHash(std::string text);