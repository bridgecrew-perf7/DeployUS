#include "GitTree.hpp"
#include "GitBlob.hpp"
#include <common.hpp>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

namespace fs = boost::filesystem;

GitTree::GitTree()
{
    initialize();
}

GitTree::GitTree(const string& rootSHA1)
{
    initialize();

    //Read in file referenced by SHA1
    string contents = Common::readGitObject(rootSHA1);
    boost::char_separator<char> sepnewline{string(1,GITTREE_OBJECT_SEPERATOR_INTRA).c_str()};
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
        if(typeObj.compare(GITTREE_OBJECT_BLOB_NAME) == 0) 
        {
            this->addBlob(filepath,sha1);
        }
        else if (typeObj.compare(GITTREE_OBJECT_TREE_NAME) == 0)
        {
            (*branches)[filepath] = new GitTree(sha1);
        }
        
    }

    //Must find SHA1 again
    this->generateHash();
}

GitTree* GitTree::createGitTreeFromIndexFile()
/*
    Reads Index File and returns a valid GitTree object from its contents.
*/
{
    GitTree* tree = new GitTree();
    tree->updateFromIndex();
    return tree;
}

GitTree::~GitTree()
{
    //Sub-directories
    if(branches != nullptr)
    {
        for(auto it = branches->begin(); it != branches->end(); it++)
        {
            delete it->second;
        }
        delete branches;
    }

    //files contained within directories
    if(leaves != nullptr) delete leaves;
}

void GitTree::initialize()
//Initializes member attributes.
{
    // AB - pourquoi utiliser des pointeurs? Ca aurait pu etre fait avec des objets normaux -2
    branches = new std::map<string, GitTree*>();
    leaves = new std::list<std::pair<string, string>>();
}

void GitTree::addBlob(const string& filepath, const string& sha1hash)
//Adds a blob to the appropriate place in the tree.
{
    //If the blob stems from a file in the root folder, add it to the root tree's leaves.
    if(count(filepath.begin(),filepath.end(),'/') == 0)
    {
        std::pair<string, string> blobpair(filepath,sha1hash);
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

std::string GitTree::generateHash()
//Generate SHA1 from tree object
{
    std::stringstream bytestream; //Used to calculate tree hash.

    //1. For deterministic results, must sort tree first
    this->sort();

    //2. Adding references from branches (sub-directories)
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        bytestream << branch->first << branch->second->generateHash();
    }

    //3. Adding references to leaves (files in this directory)
    for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
    {
        bytestream << leaf->first << leaf->second;
    }

    //4. Save hash value of tree
    sha1hash = Common::generateSHA1(bytestream.str());

    return sha1hash;
}

int GitTree::rmTrackedFiles(fs::path parentDirectory)
//Removes tracked files specified by this GitTree obj
//Returns non-zero if an error occurs. Zero otherwise.
{
    //1. Removing sub directories and their contents
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        //Removing folder contents
        branch->second->rmTrackedFiles(fs::path(parentDirectory).append(branch->first));

        //Remove folder if it does not exists
        auto childDirectory = fs::path(parentDirectory).append(branch->first);
        if(fs::exists(childDirectory))
            if(fs::is_empty(childDirectory))
            {
                if(Common::safeRemove(childDirectory))
                {
                    std::cout << "Error: Could not remove " << childDirectory.string() << std::endl;
                    return 1;
                }
            }     
    }

    //2. Removing files in directory
    for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
    {
        fs::path filepath = fs::path(parentDirectory).append(leaf->first);
        
        if(Common::safeRemove(filepath))
        {
            std::cout << "Error: Could not remove " << filepath.string() << std::endl;
            return 1;
        }
    }
    return 0;
}

int GitTree::restoreTrackedFiles(fs::path parentDirectory)
//Restores all tracked files.
//Returns non-zero if an error occured. Returns zero otherwise.
{
    //1. Restoring sub-directories
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        //Restoring folder if it does not exists
        auto childDirectory = fs::path(parentDirectory).append(branch->first);
        if(!fs::exists(childDirectory))
            fs::create_directory(childDirectory);

        //Restoring folder contents
        branch->second->restoreTrackedFiles(fs::path(parentDirectory).append(branch->first));
    }

    //2. Restoring files in directory
    for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
    {
        //Restore Blob
        GitBlob fileBlobObj = GitBlob::createFromGitObject(leaf->second);
        if(fileBlobObj.restoreBlob())
        {
            std::cout << "Error: Could not write file " << fileBlobObj.getRelativePath() << std::endl;
            return 1;
        }
    }

    return 0;
}

void GitTree::sort()
//Sorts branches and leaves to produce deterministic results
{
    //1. Sorting files
    leaves->sort();

    //2. Sorting sub-trees
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        branch->second->sort();
    }
}

std::string GitTree::generateContents()
//Formats the content of a tree object
{
    std::stringstream bytestream; //filecontents

    //1. Adding references from branches (sub-directories)
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        bytestream << GITTREE_OBJECT_TREE_NAME;         //tree
        bytestream << GITTREE_OBJECT_SEPERATOR_INTER;
    bytestream << branch->second->getSHA1Hash();        //hash
        bytestream << GITTREE_OBJECT_SEPERATOR_INTER;
        bytestream << branch->first;                    //dirname
        bytestream << GITTREE_OBJECT_SEPERATOR_INTRA;
    }

    //2. Adding references to leaves (files in this directory)
    for(auto leaf = leaves->begin(); leaf != leaves->end(); leaf++)
    {
        bytestream << GITTREE_OBJECT_BLOB_NAME;         //blob
        bytestream << GITTREE_OBJECT_SEPERATOR_INTER;
        bytestream << leaf->second;                     //hash
        bytestream << GITTREE_OBJECT_SEPERATOR_INTER;
        bytestream << leaf->first;                      //filename
        bytestream << GITTREE_OBJECT_SEPERATOR_INTRA;
    }

    //Returning contents
    return bytestream.str();
}

int GitTree::addInObjects()
//Adds all necessary tree objects to the .git/objects folder. Return 0 if successful. Non-zero otherwise.
{
    //1. Generate SHA1 hash
    string sha1 = this->generateHash();

    //2. Adding sub-trees in the .git/object folders
    for(auto branch = branches->begin(); branch != branches->end(); branch++)
    {
        branch->second->addInObjects();
    }
    //3. Adds the tree object to the .git/objects folder
    this->filecontents = generateContents();

    return GitObjectCommon::addInObjects(sha1, this->filecontents);
}

int GitTree::hasBlob(string filepath, string hash)
//Returns non-zero if the tree contains the blob;
{
    //Find blob if in sub-directory
    int firstDirectoryPos = filepath.find('/');
    if(firstDirectoryPos != string::npos)
    {
        string firstDirectoryName = filepath.substr(0,firstDirectoryPos);
        string remainingPath = filepath.substr(firstDirectoryPos + 1, filepath.size() - firstDirectoryPos - 1);

        if(branches->count(firstDirectoryName))
            return branches->at(firstDirectoryName)->hasBlob(remainingPath,hash);
        else    
            return 0;
    }
    
    //Find blob in this directory
    if((find(leaves->begin(), leaves->end(), std::pair<string,string>(filepath,hash)) != leaves->end()))
        return 1;
    else
        return 0;
}

void GitTree::updateFromIndex()
{
    //1. Read the index file.
    string indexContents = Common::readFile(GitFilesystem::getIndexPath());

    if(indexContents.size() != 0)
    {
        //2. For each line in the index file, insert blob into root tree.
        boost::char_separator<char> sep{Common::INDEX_FILE_DELIMETER_INTRA};
        tokenizer tokenized{indexContents, sep};
        for(const auto& token: tokenized)
        {
            //Split line of index file into its filename and hash components according to delimeter. 
            std::stringstream fileref(token);
            string filePath,fileHash;
            getline(fileref,filePath,Common::INDEX_FILE_DELIMETER_INTER);
            getline(fileref,fileHash,Common::INDEX_FILE_DELIMETER_INTER);

            //Adding the versioned file to the GitTree
            this->addBlob(filePath, fileHash);
        }
    }
}