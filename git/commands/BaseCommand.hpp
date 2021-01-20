#pragma once

#include <string>
#include <filesystem/GitFilesystem.hpp>

//Base class for commands

class BaseCommand: public GitFilesystem
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
