#include "GitBlob.hpp"
#include "../common.hpp"
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;
using namespace std;

GitBlob::GitBlob(char* path)
{
    //Saving path of file.
    this->relativePath = string(path);
    
    //remove the ./ if present
    if(this->relativePath.substr(0,2).compare("./") == 0)
        this->relativePath = this->relativePath.substr(2, this->relativePath.size() - 2);

    //Open file & read all contents
    this->filecontents = readFile(path);

    //Get SHA-1 digest of header + file content 
    this->sha1hash = generateBlobHash(this->filecontents);
}

string GitBlob::generateBlobContents()
//Generate the content of the object blob
{
    string blobcontents;

    //1. Adding Filename
    blobcontents.append("File name: ");
    blobcontents.append(this->relativePath);
    blobcontents.push_back('\n');

    //2. Adding file size
    blobcontents.append("File size: ");
    blobcontents.append(to_string(this->filecontents.size()));
    blobcontents.push_back('\n');

    //3. Adding file contents
    blobcontents.append("File content: ");
    blobcontents.append(this->filecontents);
    blobcontents.push_back('\n');

    return blobcontents;
}

int GitBlob::addInIndex() 
// Adds reference to file blob to the Index file. Returns non-zero if unsuccessful.
{
    //Fetch the end of the IndexFile
    fs::path indexpath("./.git/index");
    ofstream indexfile(indexpath.c_str(), ios_base::app);

    //1. Add file name
    indexfile << this->relativePath;

    //2. Add a null-terminating character for seperating fields.
    indexfile << INDEX_FILE_DEMLIMETER;

    //3. Add sha1 hash of file.
    indexfile << this->sha1hash << endl;

    indexfile.close();
    return 0;
}

string GitBlob::generateBlobHash(string text)
/* Returns SHA-1 of given text*/
{
    string header = "";
    header += "blob ";
    header += to_string(text.size());
    header += '\0';

    return generateSHA1(header + text);
}

int GitBlob::addBlobInObjects()
// Add blob to the .git/objects folder. Returns non-zero if an error occurs
{
    return addInObjects(getSHA1Hash(),generateBlobContents() );
}