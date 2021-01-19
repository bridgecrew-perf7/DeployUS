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
        string* parentCommitSHA1;
        string* commitAuthor;
        string* msg;
        string* commitTime;

        string commitcontent;

    public:
        GitCommit(GitTree *tree, string *author, string *message, string* parentSHA1, string* dt = nullptr);
        ~GitCommit();

        string generateCommitContents();
        string generateCommitSHA1();
        void addCommitInObjects();

        static GitCommit* createFromGitObject(string sha1);

        GitTree* getRootTree();
        inline string* getParentSHA() {return parentCommitSHA1;};
        inline string* getMsg() {return msg;};
        inline string* getAuthor() {return commitAuthor;};
        inline string* getCommitTime() {return commitTime;};
};

