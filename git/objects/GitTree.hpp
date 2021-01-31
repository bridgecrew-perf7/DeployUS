#pragma once
#include "GitObjectCommon.hpp"
#include <common.hpp>
#include <map>
#include <list>
#include <string>
#include <boost/filesystem.hpp>

const char GITTREE_OBJECT_SEPERATOR_INTER = '\0';
const char GITTREE_OBJECT_SEPERATOR_INTRA = '\n';
const string GITTREE_OBJECT_TREE_NAME = "tree";
const string GITTREE_OBJECT_BLOB_NAME = "blob";

namespace fs = boost::filesystem;

class GitTree
{
    private:
        string sha1hash;        //What the file will be named in the .git/objects folder
        string filecontents;    //What is to be stored in the .git/objects folder

        std::map<string, GitTree*> *branches;                  //Other Trees for subdirectories (dirname,GitTree object). The GitTree object contaisn the SHA1
        std::list<std::pair<string, string>>  *leaves;         //Files in folder represented by tree (filename, sha1)

        void initialize();
    public:
        GitTree();
        GitTree(const string& rootSHA1);
        static GitTree* createGitTreeFromIndexFile();
        ~GitTree();

        //Tree manipulation function
        void    addBlob(const string& filepath, const string& sha1hash);
        int     hasBlob(string filepath, string hash);
        void    updateFromIndex();
        int     rmTrackedFiles(fs::path directory);
        int     restoreTrackedFiles(fs::path directory);
        void    sort();
        int     addInObjects();

        //String generation
        string generateHash();
        string generateContents();

        //Getters
        string getSHA1Hash() {return sha1hash;};
};