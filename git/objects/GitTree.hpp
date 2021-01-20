#pragma once
#include "BaseGitObject.hpp"
#include <map>
#include <list>
#include <string>

#define GITTREE_OBJECT_SEPERATOR '\0'
#define GITTREE_OBJECT_TREE_NAME "tree"
#define GITTREE_OBJECT_BLOB_NAME "blob"

using namespace std;

class GitTree: public BaseGitObject
{
    private:
        map<string, GitTree*> *branches;                             //Other Trees (dirname,GitTree object). The GitTree object contaisn the SHA1
        list<pair<string, string>>  *leaves;              //Files in folder represented by tree (filename, sha1)

        void initialize();
    public:
        GitTree();
        GitTree(const string& rootSHA1);
        ~GitTree();

        //Tree manipulation function
        void addBlob(const string& filepath, const string& sha1hash);
        void sort();
        virtual int addInObjects();

        //String generation
        virtual string generateHash();
        virtual string generateContents();
};