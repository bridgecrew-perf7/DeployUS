#pragma once
#include "BaseGitObject.hpp"
#include <string>

#define GITBLOB_OBJECT_BLOB_NAME "blob"
#define GITBLOB_OBJECT_FILENAME_FIELD "Filename: "
#define GITBLOB_OBJECT_FILESIZE_FIELD "Filesize: "
#define GITBLOB_OBJECT_FILECONTENTS_FIELD "File contents: "
#define GITBLOB_OBJECT_INTER_SEPERATOR '\n'

using namespace std;

class GitBlob: public BaseGitObject
{
    private: 
        string relativePath;
        string verabtimFileContent;

    public:
        GitBlob(const char* path);
        ~GitBlob();

        //Interface with database
        int addInIndex();

        //String generation
        virtual string generateContents();
        virtual string generateHash();

};