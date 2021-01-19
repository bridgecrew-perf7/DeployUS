#pragma once

#include <string>

//Base class for commands

class BaseCommand
{
    private:
        std::string sha1hash;
        
    public:
        BaseCommand(/* args */);
        ~BaseCommand();

        //Method to be overwritten
        virtual int execute();
        virtual void help();
};
