#pragma once
#include "BaseGitObject.hpp"
#include "GitTree.hpp"
#include <string>

#define GITCOMMIT_OBJECT_INTRA_SEPERATOR '\0'
#define GITCOMMIT_OBJECT_INTER_SEPERATOR '\n'
#define GITCOMMIT_OBJECT_AUTHOR_FIELD "Author"
#define GITCOMMIT_OBJECT_MSG_FIELD "Message"
#define GITCOMMIT_OBJECT_PARENT_FIELD "Parent"
#define GITCOMMIT_OBJECT_TIME_FIELD "Time(UTC)"


class GitCommit: public BaseGitObject
{
    private:
        GitTree* root;
        string parentCommitSHA1;
        string commitAuthor;
        string msg;
        string commitTime;

    public:
        GitCommit(GitTree *tree, const string& author, const string& message, const string& parentSHA1, const string& dt = string(""));
        ~GitCommit();

        virtual string generateContents();
        virtual string generateHash();

        int blobInTree(string path, string hash);

        void rmTrackedFiles();
        void restoreTrackedFiles();

        static GitCommit* createFromGitObject(const string& sha1);

        GitTree* getRootTree();
        inline string getParentSHA() {return parentCommitSHA1;};
        inline string getMsg() {return msg;};
        inline string getAuthor() {return commitAuthor;};
        inline string getCommitTime() {return commitTime;};
};

