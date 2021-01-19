#pragma once
#include "BaseGitObject.hpp"
#include <map>
#include <list>
#include <string>

#define TREE_OBJECT_SEPERATOR '\0'

class GitTree: public BaseGitObject
{
    private:
        std::map<std::string, GitTree*> *branches;                             //Other Trees (dirname,GitTree object). The GitTree object contaisn the SHA1
        std::list<std::pair<std::string, std::string>>  *leaves;              //Files in folder represented by tree (filename, sha1)

        void initialize();
    public:
        GitTree();
        GitTree(std::string* rootSHA1);
        ~GitTree();

        //Tree manipulation function
        void addBlob(std::string filepath, std::string sha1hash);
        std::string generateTreeSHA1();
        std::string generateTreeContents();
        void sort();
        void addTreeInObjects();
};