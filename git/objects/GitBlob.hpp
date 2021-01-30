#pragma once
#include "BaseGitObject.hpp"
#include <string>
#include <list>


#define GITBLOB_OBJECT_BLOB_NAME "blob"
#define GITBLOB_OBJECT_FILENAME_FIELD "Filename: "
#define GITBLOB_OBJECT_FILESIZE_FIELD "Filesize: "
#define GITBLOB_OBJECT_FILECONTENTS_FIELD "File contents: "
#define GITBLOB_OBJECT_INTER_SEPERATOR string("\0\n",2)
#define GITBLOB_OBJECT_INTRA_SEPERATOR '\0'

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
        inline string getRelativePath() {return relativePath;};

        //Interface with database
        int addInIndex();
        int isinIndex();
        int isTracked();
        int restoreBlob();

        //String generation
        string generateReference();
        virtual string generateContents();
        virtual string generateHash();

};