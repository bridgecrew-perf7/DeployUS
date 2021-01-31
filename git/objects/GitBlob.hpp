#pragma once
#include "GitObjectCommon.hpp"
#include <common.hpp>
#include <string>
#include <list>


const string GITBLOB_OBJECT_BLOB_NAME("blob");
const string GITBLOB_OBJECT_FILENAME_FIELD("Filename: ");
const string GITBLOB_OBJECT_FILESIZE_FIELD("Filesize: ");
const string GITBLOB_OBJECT_FILECONTENTS_FIELD("File contents: ");
const string GITBLOB_OBJECT_INTER_SEPERATOR("\0\n",2);
const char GITBLOB_OBJECT_INTRA_SEPERATOR('\0');

class GitBlob
{
    private: 
        string sha1hash;        //What the file will be named in the .git/objects folder
        string filecontents;    //What is to be stored in the .git/objects folder

        string relativePath;
        string verabtimFileContent;
    public:
        GitBlob();
        GitBlob(const string path);
        GitBlob(const string path,const string filecontents);
        ~GitBlob();

        static GitBlob createFromGitObject(const string sha1); 

        //Getters
        string getVerbatinContents() {return verabtimFileContent;};
        string getRelativePath() {return relativePath;};
        string getSHA1Hash() {return sha1hash;};

        //Interface with database
        int addInIndex();
        int isinIndex();
        int isTracked();
        int restoreBlob();
        int addInObjects() {return GitObjectCommon::addInObjects(this->sha1hash, this->filecontents);};

        //String generation
        string generateReference();
        string generateContents();
        string generateHash();

};