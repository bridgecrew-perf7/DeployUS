#pragma once
#include "GitObjectCommon.hpp"
#include "GitTree.hpp"
#include <string>

const char GITCOMMIT_OBJECT_INTRA_SEPERATOR = '\0';
const char GITCOMMIT_OBJECT_INTER_SEPERATOR = '\n';
const std::string GITCOMMIT_OBJECT_AUTHOR_FIELD = std::string("Author");
const std::string GITCOMMIT_OBJECT_MSG_FIELD = std::string("Message");
const std::string GITCOMMIT_OBJECT_PARENT_FIELD = std::string("Parent");
const std::string GITCOMMIT_OBJECT_TIME_FIELD = std::string("Time(UTC)");


class GitCommit
{
    private:
        string sha1hash;        //What the file will be named in the .git/objects folder
        string filecontents;    //What is to be stored in the .git/objects folder

        GitTree* root;
        string parentCommitSHA1;
        string commitAuthor;
        string msg;
        string commitTime;

    public:
        GitCommit(GitTree *tree, const string& author, const string& message, const string& parentSHA1, const string& dt = string(""));
        ~GitCommit();

        string generateContents();
        string generateHash();
        int addInObjects();

        int blobInTree(string path, string hash);

        int rmTrackedFiles();
        int restoreTrackedFiles();

        static GitCommit* createFromGitObject(const string& sha1);

        GitTree* getRootTree();
        string getSHA1Hash() {return sha1hash;};
        string getParentSHA() {return parentCommitSHA1;};
        string getMsg() {return msg;};
        string getAuthor() {return commitAuthor;};
        string getCommitTime() {return commitTime;};
};

