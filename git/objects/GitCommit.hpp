#pragma once
#include "BaseGitObject.hpp"
#include "GitTree.hpp"
#include <string>

#define COMMIT_OBJECT_SEPERATOR '\0'

using namespace std;


class GitCommit: public BaseGitObject
{
    private:
        GitTree* root;
        string* commitAuthor;
        string* msg;

        string commitcontent;

    public:
        GitCommit(GitTree* tree, string *author, string *message);
        ~GitCommit();

        string generateCommitContents();
        string generateCommitSHA1();
        void addCommitInObjects();
};