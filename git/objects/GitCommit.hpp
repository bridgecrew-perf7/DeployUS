#pragma once
#include "BaseGitObject.hpp"
#include "GitTree.hpp"
#include <string>

const char GITCOMMIT_OBJECT_INTRA_SEPERATOR = '\0';
const char GITCOMMIT_OBJECT_INTER_SEPERATOR = '\n';
const std::string GITCOMMIT_OBJECT_AUTHOR_FIELD = std::string("Author");
const std::string GITCOMMIT_OBJECT_MSG_FIELD = std::string("Message");
const std::string GITCOMMIT_OBJECT_PARENT_FIELD = std::string("Parent");
const std::string GITCOMMIT_OBJECT_TIME_FIELD = std::string("Time(UTC)");


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

