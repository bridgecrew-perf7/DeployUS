#pragma once
#include "BaseGitObject.hpp"
#include <map>
#include <list>
#include <string>

#define TREE_OBJECT_SEPERATOR '\0'

class GitTree: public BaseGitObject
{
    private:
        std::map<std::string, GitTree> *branches;                             //Other Trees
        std::list<std::pair<std::string, std::string>>  *leaves;              //Files in folder represented by tree

    public:
        GitTree();
        ~GitTree();

        //Tree manipulation function
        void addBlob(std::string filepath, std::string sha1hash);
        std::string generateTreeSHA1();
        std::string generateTreeContents();
        void sort();
        void addTreeInObjects();
};