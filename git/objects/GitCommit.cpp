#include "GitCommit.hpp"
#include "../common.hpp"
#include <string>
#include <sstream>
#include <ctime>
#include <iostream>

using namespace std;


GitCommit::GitCommit(GitTree *tree, string *author, string *message)
{
    root = tree;
    commitAuthor = author;
    msg = message;

    commitcontent = generateCommitContents();

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

    //Adding author
    bytestream << "Author" << COMMIT_OBJECT_SEPERATOR << commitAuthor << '\n';

    //Adding message
    bytestream << "Message" << COMMIT_OBJECT_SEPERATOR << msg << '\n';

    //Adding time
    time_t now = time(0);
    tm *gmtm = gmtime(&now);
    char* dt = asctime(gmtm);
    bytestream << "Time(UTC)" << COMMIT_OBJECT_SEPERATOR << dt << '\n';

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