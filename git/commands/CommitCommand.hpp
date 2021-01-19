#pragma once

#include "BaseCommand.hpp"
#include "../objects/GitCommit.hpp"
#include <map>
#include <utility>
#include <string>
#include <list>

using namespace std;

class CommitCommand: public BaseCommand
{
    private:
        int numArgs;
        char** args;
        string commitMessage;
        string commitAuthor;

    public:
        CommitCommand(int argc, char* argv[]);
        ~CommitCommand();

        virtual int execute();
        virtual void help();

        void clearIndex();
        void updateHEAD(GitCommit* obj);
};