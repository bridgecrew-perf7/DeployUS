#pragma once

#include "BaseCommand.hpp"
#include <string>

class AddCommand: public BaseCommand
{
private:
    int numArgs;
    char** args;
public:
    AddCommand(int argc, char* argv[]);
    ~AddCommand();

    virtual int execute();
    virtual void help();
};

class GitBlob
{
    private: 
        char* relativePath;
        std::string filecontents;
        std::string sha1hash;
        std::string generateBlobContents();
    public:
        GitBlob(char* path);
        ~GitBlob();

        int addInObjects();
        int addInIndex();
};