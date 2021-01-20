#include "GitBlob.hpp"
#include "../common.hpp"
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;
using namespace std;

GitBlob::GitBlob(const char* path)
{
    //Saving path of file.
    this->relativePath = string(path);
    
    //remove the ./ if present
    if(this->relativePath.substr(0,2).compare("./") == 0)
        this->relativePath = this->relativePath.substr(2, this->relativePath.size() - 2);

    //Open file & read all contents
    this->verabtimFileContent = readFile(path);

    //Get SHA-1 digest of header + file content 
    this->sha1hash = generateHash();

    //Generate blob contents
    this->filecontents = generateContents();
}

string GitBlob::generateContents()
//Generate the content of the object blob
{
    string blobcontents;

    //1. Adding Filename
    blobcontents.append(GITBLOB_OBJECT_FILENAME_FIELD);
    blobcontents.append(this->relativePath);
    blobcontents.push_back(GITBLOB_OBJECT_INTER_SEPERATOR);

    //2. Adding file size
    blobcontents.append(GITBLOB_OBJECT_FILESIZE_FIELD);
    blobcontents.append(to_string(this->verabtimFileContent.size()));
    blobcontents.push_back(GITBLOB_OBJECT_INTER_SEPERATOR);

    //3. Adding file contents
    blobcontents.append(GITBLOB_OBJECT_FILECONTENTS_FIELD);
    blobcontents.append(this->verabtimFileContent);
    blobcontents.push_back(GITBLOB_OBJECT_INTER_SEPERATOR);

    return blobcontents;
}

int GitBlob::addInIndex() 
// Adds reference to file blob to the Index file. Returns non-zero if unsuccessful.
{
    //Fetch the end of the IndexFile
    ofstream indexfile(this->getIndexPath().c_str(), ios_base::app);

    //1. Add file name
    indexfile << this->relativePath;

    //2. Add a null-terminating character for seperating fields.
    indexfile << INDEX_FILE_DEMLIMETER;

    //3. Add sha1 hash of file.
    indexfile << this->sha1hash << endl;

    indexfile.close();
    return 0;
}

string GitBlob::generateHash()
/* Returns SHA-1 of given text*/
{
    string header;
    header += GITBLOB_OBJECT_BLOB_NAME;
    header += " ";
    header += to_string(this->verabtimFileContent.size());
    header += '\0';

    return generateSHA1(header + this->verabtimFileContent);
}