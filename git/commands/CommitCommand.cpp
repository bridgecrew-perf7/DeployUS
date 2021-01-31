#include "CommitCommand.hpp"
#include <common.hpp>
#include <objects/GitTree.hpp>
#include <objects/GitCommit.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

namespace fs = boost::filesystem;

CommitCommand::CommitCommand(int argc, char* argv[])
{
    numArgs = argc;
    args = argv;
}
CommitCommand::~CommitCommand()
{
    
}

int CommitCommand::execute()
// Executes command. Returns non-zero if failure to do so.
{
    //1. Verify that a git folder has been initiated
    if (!fs::exists(GitFilesystem::getDotGitPath()) || !fs::is_directory(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: No git repository has been found.\n";
        return 1;
    }
    //2. Argument verification
    if(numArgs != 4)
    {
        std::cout << "Error: Was expecting 2 arguments. Got " << std::to_string(numArgs - 2)<<"."  <<endl;
        help();
        return 1;
    }
    commitMessage = args[2];
    commitAuthor = args[3];

    //2.5. Verify that we are at the TOP_COMMIT
    if(fs::exists(GitFilesystem::getTOPCOMMITPath()))
    {
        std::cout << "Error: HEAD is not at the most recent commit. Please checkout the most recent commit and try again.\n";
        std::cout << "       Most recent commit: " << Common::readFile(GitFilesystem::getTOPCOMMITPath()) << '\n'; 
        return 1;
    }


    //3. Verify that there are staged files
    if(Common::readFile(GitFilesystem::getIndexPath().c_str()).size() == 0)
    {
        std::cout << "Error: There are no staged files. You can stage some files using ./gitus add <pathspec>\n";
        return 1;
    }

    //3.5. Fetch root tree
    GitTree *root = nullptr;
    string parentCommitSHA1 = Common::readFile(GitFilesystem::getHEADPath().c_str());
    if(parentCommitSHA1.size() != 0)
    {
        GitCommit* parentCommit = GitCommit::createFromGitObject(parentCommitSHA1);
        root = parentCommit->getRootTree();
    }
    else root = new GitTree();

    //4. Read the index file.
    string indexContents = Common::readFile(GitFilesystem::getIndexPath().c_str());

    //5. For each line in the index file, insert blob into root tree.
    boost::char_separator<char> sep{"\n"};
    tokenizer tokenized{indexContents, sep};
    for(const auto& token: tokenized)
    {
        //Split line of index file into its filename and hash components according to delimeter. 
        stringstream fileref(token);
        string filePath,fileHash;
        getline(fileref,filePath,Common::INDEX_FILE_DELIMETER);
        getline(fileref,fileHash,Common::INDEX_FILE_DELIMETER);

        //Adding the versioned file to the GitTree
        root->addBlob(filePath, fileHash);
    }

    //6. Sort root tree to produce deterministic results
    root->sort();

    //7. Generate the tree SHA1 hash;
    root->generateHash();
    root->addInObjects(); 

    //8. Create commit object 
    GitCommit* commitobj = new GitCommit(root, commitAuthor, commitMessage, parentCommitSHA1);

    //9. Save commit object to the .git/objects folder
    commitobj->generateHash();
    commitobj->addInObjects();

    //10. Delete content on .git/index file
    clearIndex();

    //11. Update the .git/HEAD file
    updateHEAD(commitobj);

    //Reclaim memory
    delete commitobj;

    return 0;
}

void CommitCommand::help() {
    std::cout << "usage: gitus commit <msg> <author>" << endl;
}

void CommitCommand::clearIndex()
//Clears the content of the .index file
{
    Common::writeFile(GitFilesystem::getIndexPath(),string());
}


void CommitCommand::updateHEAD(GitCommit* obj)
//Insert SHA1 of new commit to the HEAD file
{
    Common::writeFile(GitFilesystem::getHEADPath(), obj->getSHA1Hash());
}