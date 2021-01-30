#include "GitCommit.hpp"
#include <common.hpp>
#include <string>
#include <sstream>
#include <ctime>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <map>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

GitCommit::GitCommit(GitTree *tree, const string& author, const string& message, const string& parentSHA1, const string& dt)
{
    root = tree;
    commitAuthor = author;
    msg = message;
    commitTime = dt;
    parentCommitSHA1 = parentSHA1;

    filecontents = generateContents();
}

GitCommit::~GitCommit()
{
    if(root != nullptr)
        delete root;
}

GitCommit* GitCommit::createFromGitObject(const string& sha1)
// Reads file specified by SHA1 and returns a valid GitCommit object of the commit object specified by parentCommitSHA1
{
    string commitContent = Common::readGitObject(sha1);

    //Read in commit file
    boost::char_separator<char> sepnewline{ string(1,GITCOMMIT_OBJECT_INTER_SEPERATOR).c_str()};
    tokenizer newline{commitContent, sepnewline};
    std::map<string, string> memberfields;
    for(const auto& line: newline)
    {
        //Split line on null-terminating character
        auto nulltermPos = line.find(GITCOMMIT_OBJECT_INTRA_SEPERATOR);
        string fieldname = line.substr(0,nulltermPos);
        string fieldvalue = line.substr(nulltermPos + 1, line.size() - nulltermPos - 1);

        memberfields[fieldname] = fieldvalue;
    }

    GitTree* root = new GitTree(memberfields[GITTREE_OBJECT_TREE_NAME]);
    GitCommit* out = new GitCommit( root,
                                    memberfields[GITCOMMIT_OBJECT_AUTHOR_FIELD],
                                    memberfields[GITCOMMIT_OBJECT_MSG_FIELD],
                                    memberfields[GITCOMMIT_OBJECT_PARENT_FIELD],
                                    memberfields[GITCOMMIT_OBJECT_TIME_FIELD]);
    out->generateHash();
    return out;
}

void GitCommit::rmTrackedFiles()
// Removes all files tracked in current commit object
{
    root->rmTrackedFiles(GitFilesystem::getDotGitPath().parent_path());
}

void GitCommit::restoreTrackedFiles()
// Restores all files tracked in current commit object
{
    root->restoreTrackedFiles(GitFilesystem::getDotGitPath().parent_path());
}

string GitCommit::generateContents()
//Generate the content of the commit object to be stored in the .git/objects folder.
{
    std::stringstream bytestream; //filecontents

    //Adding tree reference
    bytestream << GITTREE_OBJECT_TREE_NAME << GITCOMMIT_OBJECT_INTRA_SEPERATOR << root->getSHA1Hash() << GITCOMMIT_OBJECT_INTER_SEPERATOR;

    //If a parent commit exists, add this to the git object file
    if(parentCommitSHA1.size() == 40)
    {
        bytestream << GITCOMMIT_OBJECT_PARENT_FIELD << GITCOMMIT_OBJECT_INTRA_SEPERATOR << parentCommitSHA1 << GITCOMMIT_OBJECT_INTER_SEPERATOR;
    }

    //Adding author
    bytestream << GITCOMMIT_OBJECT_AUTHOR_FIELD << GITCOMMIT_OBJECT_INTRA_SEPERATOR << commitAuthor << GITCOMMIT_OBJECT_INTER_SEPERATOR;

    //Adding message
    bytestream << GITCOMMIT_OBJECT_MSG_FIELD << GITCOMMIT_OBJECT_INTRA_SEPERATOR << msg << GITCOMMIT_OBJECT_INTER_SEPERATOR;

    //Adding time
    if(commitTime.size() == 0)
    {
        time_t now = time(0);
        tm *gmtm = gmtime(&now);
        char* dt = asctime(gmtm);
        commitTime = string(dt);
        if(commitTime.back() == '\n')
            commitTime = commitTime.substr(0,commitTime.size() - 1);
    }
    bytestream << GITCOMMIT_OBJECT_TIME_FIELD << GITCOMMIT_OBJECT_INTRA_SEPERATOR << commitTime << GITCOMMIT_OBJECT_INTER_SEPERATOR;

    return bytestream.str();
}

string GitCommit::generateHash()
//Generates SHA1 from commit content. Returns the SHA1 hash.
{
    if(filecontents.size() == 0)
        filecontents = generateContents();

    sha1hash = Common::generateSHA1(filecontents);
    return sha1hash;
}

GitTree* GitCommit::getRootTree()
{
    return root;
}

int GitCommit::blobInTree(string path, string hash)
//Returns non-zero if blob is already in tree
{
    return root->hasBlob(path,hash);
}

