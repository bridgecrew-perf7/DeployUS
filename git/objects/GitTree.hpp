#pragma once
#include "BaseGitObject.hpp"
#include <map>
#include <list>
#include <string>
#include <boost/filesystem.hpp>

#define GITTREE_OBJECT_SEPERATOR '\0'
#define GITTREE_OBJECT_TREE_NAME "tree"
#define GITTREE_OBJECT_BLOB_NAME "blob"

namespace fs = boost::filesystem;

class GitTree: public BaseGitObject
{
    private:
        // AB - je n'avais pas demand� de supporter des arbres r�cursifs
        // SV : Oops. Je tentais d'imiter git.  
        std::map<string, GitTree*> *branches;                  //Other Trees (dirname,GitTree object). The GitTree object contaisn the SHA1
        std::list<std::pair<string, string>>  *leaves;              //Files in folder represented by tree (filename, sha1)

        void initialize();
    public:
        GitTree();
        GitTree(const string& rootSHA1);
        ~GitTree();

        //Tree manipulation function
        void addBlob(const string& filepath, const string& sha1hash);
        void rmTrackedFiles(fs::path directory);
        void restoreTrackedFiles(fs::path directory);
        void sort();
        virtual int addInObjects();
        int hasBlob(string filepath, string hash);

        //String generation
        virtual string generateHash();
        virtual string generateContents();
};