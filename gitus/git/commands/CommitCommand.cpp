#include "CommitCommand.hpp"
#include <common.hpp>
#include <objects/GitTree.hpp>
#include <objects/GitCommit.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
namespace fs = boost::filesystem;


int CommitCommand::execute(int argc, char* argv[])
// Executes command. Returns non-zero if failure to do so.
{
    //1. Verify if help is needed
    if(argc > 2)
    {
        string option = argv[2];
        if(option.compare(Common::HELP_PARAM) == 0)
            return help();
    }

    //2. Verify that a git folder has been initiated
    if (!fs::exists(GitFilesystem::getDotGitPath()) || !fs::is_directory(GitFilesystem::getDotGitPath()))
    {
        std::cout << "Error: No git repository has been found.\n";
        return 1;
    }

    //3. Argument verification
    if(argc != 4)
    {
        std::cout << "Error: Was expecting 2 arguments. Got " << std::to_string(argc - 2)<<"."  <<endl;
        help();
        return 1;
    }
    string commitMessage = argv[2];
    string commitAuthor = argv[3];

    //4. Verify that we are at the TOP_COMMIT. This file is related to the checkout command.
    if(fs::exists(GitFilesystem::getTOPCOMMITPath()))
    {
        std::cout << "Error: HEAD is not at the most recent commit. Please checkout the most recent commit and try again.\n";
        std::cout << "       Most recent commit: " << Common::readFile(GitFilesystem::getTOPCOMMITPath()) << '\n'; 
        return 1;
    }

    //5. Verify that there are staged files
    string indexPathstr = GitFilesystem::getIndexPath().string();
    if(Common::readFile(indexPathstr).size() == 0)
    {
        std::cout << "Error: There are no staged files.\n";
        return 1;
    }

    //6. Fetch root tree. The root tree represents the repo directory with the tracked files. Untracked files are not included.
    GitTree *root = nullptr; //AB - je n'aime pas mais je comprend...
                             //     regarde pour utiliser des "smart pointeurs" 
    string parentCommitSHA1 = Common::readFile(GitFilesystem::getHEADPath());
    if(parentCommitSHA1.size() != 0)
    {
        GitCommit* parentCommit = GitCommit::createFromGitObject(parentCommitSHA1);
        root = parentCommit->getRootTree();
        root->updateFromIndex();
        // AB - et BOOM, fuite de mémoire sur parentCommit -2
    }
    else root = GitTree::createGitTreeFromIndexFile();

    //11. Create commit object and add it the the object file
    // AB - aucune raison d'être un pointeur.
    GitCommit* commitobj = new GitCommit(root, commitAuthor, commitMessage, parentCommitSHA1);
    if(commitobj->addInObjects())
    {
        std::cout << "Error: Could not commit files.\n";
        return 1;
    }

    //13. Delete content of .git/index file
    clearIndex();

    //14. Update the .git/HEAD file
    updateHEAD(commitobj);

    //Reclaim memory
    delete commitobj; //Will also delete GitTree* root obj - AB - bon commentaire

    return 0;
}

int CommitCommand::help()
//Sends usage message to stdout. Always returns 0;
{
    std::cout << "usage: gitus commit <msg> <author>" << endl;
    return 0;
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