#include "GitBlob.hpp"
#include "GitCommit.hpp"
#include <common.hpp>
#include <boost/filesystem.hpp>
#include <filesystem/GitIndexFile.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <list>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
namespace fs = boost::filesystem;

GitBlob::GitBlob()
{

}

GitBlob::GitBlob(const string path)
{
    //Saving path of file.
    this->relativePath = string(path);
    
    //remove the ./ if present
    if(this->relativePath.substr(0,2).compare("./") == 0)
        this->relativePath = this->relativePath.substr(2, this->relativePath.size() - 2);

    //Open file & read all contents
    this->verabtimFileContent = Common::readFile(path);

    //Get SHA-1 digest of header + file content 
    this->sha1hash = generateHash();

    //Generate blob contents
    this->filecontents = generateContents();
}

GitBlob::GitBlob(const string path,const string verbatimcontents)
{
    //Saving path of file.
    this->relativePath = string(path);
    
    //remove the ./ if present
    if(this->relativePath.substr(0,2).compare("./") == 0)
        this->relativePath = this->relativePath.substr(2, this->relativePath.size() - 2);

    
    this->verabtimFileContent = verbatimcontents;

    //Get SHA-1 digest of header + file content 
    this->sha1hash = generateHash();

    //Generate blob contents
    this->filecontents = generateContents();
}

GitBlob::~GitBlob()
{

}

GitBlob GitBlob::createFromGitObject(const string sha1)
//Create a valid GitBlob object from a file
{
    string blobContent = Common::readGitObject(sha1);


    std::map<string, string> memberfields; //Fieldname,  field value
    while(blobContent.find(GITBLOB_OBJECT_INTER_SEPERATOR) != string::npos)
    {
        int sepIdx = blobContent.find(GITBLOB_OBJECT_INTER_SEPERATOR);
        string line = blobContent.substr(0,sepIdx);
        blobContent = blobContent.substr(sepIdx + GITBLOB_OBJECT_INTER_SEPERATOR.size());

        //Split line on null-terminating character
        auto nulltermPos = line.find(GITBLOB_OBJECT_INTRA_SEPERATOR);
        string fieldname = line.substr(0,nulltermPos);
        string fieldvalue = line.substr(nulltermPos + 1, line.size() - nulltermPos - 1);

        memberfields[fieldname] = fieldvalue;
    }

    //Create new GitBlob object
    return GitBlob(memberfields[GITBLOB_OBJECT_FILENAME_FIELD], memberfields[GITBLOB_OBJECT_FILECONTENTS_FIELD]);
}

string GitBlob::generateContents()
//Generate the content of the object blob
{
    string blobcontents;

    //1. Adding Filename
    blobcontents.append(GITBLOB_OBJECT_FILENAME_FIELD);
    blobcontents.push_back(GITBLOB_OBJECT_INTRA_SEPERATOR);
    blobcontents.append(this->relativePath);
    blobcontents.append(GITBLOB_OBJECT_INTER_SEPERATOR);

    //2. Adding file size
    blobcontents.append(GITBLOB_OBJECT_FILESIZE_FIELD);
    blobcontents.push_back(GITBLOB_OBJECT_INTRA_SEPERATOR);
    blobcontents.append(std::to_string(this->verabtimFileContent.size()));
    blobcontents.append(GITBLOB_OBJECT_INTER_SEPERATOR);

    //3. Adding file contents
    blobcontents.append(GITBLOB_OBJECT_FILECONTENTS_FIELD);
    blobcontents.push_back(GITBLOB_OBJECT_INTRA_SEPERATOR);
    blobcontents.append(this->verabtimFileContent);
    blobcontents.append(GITBLOB_OBJECT_INTER_SEPERATOR);

    return blobcontents;
}

string GitBlob::generateReference()
{
    //Generate text to go in index file
    string blobReference = string(this->relativePath);      //filename
    blobReference += Common::INDEX_FILE_DELIMETER_INTER;    //Delimeter
    blobReference += this->sha1hash;                        //Hash
    blobReference += Common::INDEX_FILE_DELIMETER_INTRA;

    return blobReference;
}

int GitBlob::isinIndex()
//returns non-zero if file is already staged
{
    GitIndexFile index = GitIndexFile();
    return index.contains(this->getRelativePath(), this->getSHA1Hash());
}

int GitBlob::isTracked()
//Returns non-zero if the file is already tacked
{
    string sha1Commit = Common::readFile(GitFilesystem::getHEADPath());
    //No commit = not being tracked!
    if(sha1Commit.size() == 0)
        return 0;

    //Look in the commit's root tree for a reference to the blob
    GitCommit* presentCommit = GitCommit::createFromGitObject(sha1Commit);
    return presentCommit->blobInTree(this->relativePath, this->sha1hash);
}

int GitBlob::addInIndex() 
// Adds reference to file blob to the Index file. Returns non-zero if unsuccessful.
{
    GitIndexFile index = GitIndexFile();
    index.addBlob(*this);
    index.produceIndexFile();
    return 0;
}

int GitBlob::restoreBlob()
// Restores the file defined by the Blob. Returns 0 if successful. Returns non-zero otherwise.
{
    return Common::writeFile(fs::path(this->relativePath),this->verabtimFileContent);
}

string GitBlob::generateHash()
/* Returns SHA-1 of given text*/
{
    string header;
    header += GITBLOB_OBJECT_BLOB_NAME;
    header += " ";
    header += std::to_string(this->verabtimFileContent.size());
    header += '\0';

    return Common::generateSHA1(header + this->verabtimFileContent);
}
