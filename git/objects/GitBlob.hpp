#pragma once
#include "BaseGitObject.hpp"
#include <string>

#define GITBLOB_OBJECT_BLOB_NAME "blob"
#define GITBLOB_OBJECT_FILENAME_FIELD "Filename: "
#define GITBLOB_OBJECT_FILESIZE_FIELD "Filesize: "
#define GITBLOB_OBJECT_FILECONTENTS_FIELD "File contents: "
#define GITBLOB_OBJECT_INTER_SEPERATOR '\n'
#define GITBLOB_OBJECT_INTRA_SEPERATOR '\0'

using namespace std;

class GitBlob: public BaseGitObject
{
    private: 
        string relativePath;
        string verabtimFileContent;

    public:
        GitBlob(const char* path);
        GitBlob(const string path,const string filecontents);
        ~GitBlob();

        static GitBlob createFromGitObject(const string sha1); 

        //Getters
        inline string getVerbatinContents() {return verabtimFileContent;};

        //Interface with database
        int addInIndex();
        int restoreBlob();

        //String generation
        virtual string generateContents();
        virtual string generateHash();

};