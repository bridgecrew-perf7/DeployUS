#include "GitTree.hpp"
#include "../common.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

using namespace std;

GitTree::GitTree()
{
    initialize();
}

GitTree::GitTree(string* rootSHA1)
{
    initialize();

    //Read in file referenced by SHA1
    string contents = readGitObject(*rootSHA1);
    boost::char_separator<char> sepnewline{"\n"};
    tokenizer newline{contents, sepnewline};
    for(const auto& line: newline)
    {
        //Split line on null-terminating character
        auto nulltermPos = line.find('\0');
        string typeObj = line.substr(0,nulltermPos);
        string resultant = line.substr(nulltermPos + 1, line.size() - nulltermPos - 1);
        
        nulltermPos = resultant.find('\0');
        string sha1 = resultant.substr(0,nulltermPos);
        string filepath = resultant.substr(nulltermPos + 1, line.size() - nulltermPos - 1);

        //Add reference to the tree
        if(typeObj.compare("blob") == 0) 
        {
            this->addBlob(filepath,sha1);
        }
        else if (typeObj.compare("tree") == 0)
        {
            (*branches)[filepath] = new GitTree(&sha1);
        }
        
    }

    //Must find SHA1 again
    this->generateTreeSHA1();
}

GitTree::~GitTree()
{
    if(branches != nullptr) delete branches;
    if(leaves != nullptr) delete leaves;
}

void GitTree::initialize()
//Initializes member attributes.
{
    branches = new map<string, GitTree*>();
    leaves = new list<pair<string, string>>();
}

void GitTree::addBlob(string filepath, string sha1hash)
//Adds a blob to the appropriate place in the tree.
{
    //If the blob stems from a file in the root folder, add it to the root tree's leaves.
    if(count(filepath.begin(),filepath.end(),'/') == 0)
    {
        pair<string, string> blobpair(filepath,sha1hash);
        this->leaves->push_back(blobpair);
    }
    //Else, add it to the branch associated with the next directory
    else
    {
        //Seperating filepath into directory and remaining filepath
        int firstDirectoryPos = filepath.find('/');
        string firstDirectoryName = filepath.substr(0,firstDirectoryPos);
        string remainingPath = filepath.substr(firstDirectoryPos + 1, filepath.size() - firstDirectoryPos - 1);

        //Creating branch if it does not exists
        if(!branches->count(firstDirectoryName))
            (*branches)[firstDirectoryName] = new GitTree();

        //Recursively checking the branch to add the blob
        branches->at(firstDirectoryName)->addBlob(remainingPath,sha1hash);
    }
}

std::string GitTree::generateTreeSHA1()
//Generate SHA1 from tree object
{
    if(sha1hash.size() == 0)
    {
        stringstream bytestream; //Used to calculate tree hash.

        //Adding references from branches (sub-directories)
        for(auto branch = branches->begin(); branch != branches->end(); branch++)
        {
            bytestream << branch->first << branch->second->generateTreeSHA1();
        }

        //Adding references to leaves (files in this directory)
        for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
        {
            bytestream << leaf->first << leaf->second;
        }

        ///Save hash value of tree
        sha1hash = generateSHA1(bytestream.str());
    }

    return sha1hash;
}

void GitTree::sort()
//Sorts branches and leaves to produce deterministic results
{
    //Sorting files
    leaves->sort();

    //No need to sort folders as the std::map object is already sorted 
}

std::string GitTree::generateTreeContents()
//Formats the content of a tree object
{
    stringstream bytestream; //filecontents

    //Adding references from branches (sub-directories)
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        bytestream << "tree" << TREE_OBJECT_SEPERATOR << branch->second->getSHA1Hash() << TREE_OBJECT_SEPERATOR << branch->first <<'\n';
    }

    //Adding references to leaves (files in this directory)
    for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
    {
        bytestream << "blob" << TREE_OBJECT_SEPERATOR << leaf->second << TREE_OBJECT_SEPERATOR << leaf->first <<'\n';
    }

    //Returning contents
    return bytestream.str();
}

void GitTree::addTreeInObjects()
//Adds all necessary tree objects to the .git/objects folder
{
    //Adding sub-trees in the .git/object folders
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        branch->second->addTreeInObjects();
    }

    //Adds the tree object to the .git/objects folder
    addInObjects(sha1hash, generateTreeContents());
    
}