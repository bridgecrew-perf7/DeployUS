#include "GitCommit.hpp"
#include "../common.hpp"
#include <string>
#include <sstream>
#include <ctime>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <map>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

using namespace std;

GitCommit::GitCommit(GitTree *tree, string *author, string *message, string* parentSHA1, string* dt)
{
    root = tree;
    commitAuthor = new string(*author);
    msg = new string(*message);
    if(dt != nullptr) commitTime = new string(*dt);
    else commitTime = nullptr;
    if(parentSHA1 != nullptr) parentCommitSHA1 = new string(*parentSHA1);
    else parentSHA1 = nullptr;

    commitcontent = generateCommitContents();
}

GitCommit* GitCommit::createFromGitObject(string sha1)
// Reads file specified by SHA1 and returns a valid GitCommit object of the commit object specified by parentCommitSHA1
{
    string commitContent = readGitObject(sha1);

    //Read in commit file
    boost::char_separator<char> sepnewline{"\n"};
    tokenizer newline{commitContent, sepnewline};
    map<string, string> memberfields;
    for(const auto& line: newline)
    {
        //Split line on null-terminating character
        auto nulltermPos = line.find('\0');
        string fieldname = line.substr(0,nulltermPos);
        string fieldvalue = line.substr(nulltermPos + 1, line.size() - nulltermPos - 1);

        memberfields[fieldname] = fieldvalue;
    }

    GitTree* root = new GitTree(&memberfields["tree"]);
    GitCommit* out = new GitCommit(root,&memberfields["Author"],&memberfields["Message"],&memberfields["Parent"],&memberfields["Time(UTC)"]);
    out->generateCommitSHA1();
    return out;
}

GitCommit::~GitCommit()
{
    
}

string GitCommit::generateCommitContents()
//Generate the content of the commit object to be stored in the .git/objects folder.
{
    stringstream bytestream; //filecontents

    //Adding tree reference
    bytestream << "tree" << COMMIT_OBJECT_SEPERATOR << root->getSHA1Hash() << '\n';

    //If a parent commit exists, add this to the git object file
    if(parentCommitSHA1 != nullptr)
    {
        if(parentCommitSHA1->size() == 40)
            bytestream << "Parent" << COMMIT_OBJECT_SEPERATOR << *parentCommitSHA1 << '\n';
    }

    //Adding author
    bytestream << "Author" << COMMIT_OBJECT_SEPERATOR << *commitAuthor << '\n';

    //Adding message
    bytestream << "Message" << COMMIT_OBJECT_SEPERATOR << *msg << '\n';

    //Adding time
    if(commitTime == nullptr)
    {
        time_t now = time(0);
        tm *gmtm = gmtime(&now);
        char* dt = asctime(gmtm);
        commitTime = new string(dt);
        if(commitTime->back() == '\n')
            *commitTime = commitTime->substr(0,commitTime->size() - 1);
    }
    bytestream << "Time(UTC)" << COMMIT_OBJECT_SEPERATOR << *commitTime << '\n';

    return bytestream.str();
}

string GitCommit::generateCommitSHA1()
//Generates SHA1 from commit content. Returns the SHA1 hash.
{
    if(commitcontent.size() == 0)
        commitcontent = generateCommitContents();

    sha1hash = generateSHA1(commitcontent);
    return sha1hash;
}

void GitCommit::addCommitInObjects()
//Saves commit object to the .git/object folder
{
    addInObjects(sha1hash, commitcontent);
}

GitTree* GitCommit::getRootTree()
{
    return root;
}