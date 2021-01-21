#include "common.hpp"
#include <filesystem/GitFilesystem.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <iomanip>

namespace fs = boost::filesystem;
using boost::uuids::detail::sha1;
using namespace std;

string generateSHA1(string text)
/* Returns SHA-1 of given text*/
{
    sha1 sha;
    unsigned int hash[5];
    int textLength = text.size();

    //Performing the hashing algorithm
    sha.process_bytes(&text[0], textLength);
	sha.get_digest(hash);

    //Converting to hexadecimal
    stringstream bytestream;
    char* result = new char[41];
    for(int i=0; i < 5; i++) bytestream << setfill ('0') << setw(sizeof(unsigned int)*2) << hex <<hash[i];

    //Storing in result buffer
    bytestream.read(result,40);
    result[40] = '\0'; //null-terminated

    return string(result);
}

string readFile(const char* path)
/* Helper function to read all content of file */
{
	ifstream file(path, ios::binary|ios::ate);
    ifstream::pos_type pos = file.tellg();
    vector<char>  contentsVector(pos);
	file.seekg(0, ios::beg);
    file.read(&contentsVector[0], pos);
	file.close();

	//Create string from contents
	string filecontents;
    for(char c: contentsVector) 
        filecontents.push_back(c);
	return filecontents;
}

string readFile(const string path)
{
    return readFile(path.c_str());
}

string readFile(const fs::path path)
{
    return readFile(path.c_str());
}

string readGitObject(const string objSHA1)
// Reads the file in the .git/object folder that corresponds to the sha1 passed in parameter
{
    if(objSHA1.size() != 40) return nullptr;

    string foldername = objSHA1.substr(0,2);
    string filename = objSHA1.substr(2,38);
    fs::path filepath = fs::path(GitFilesystem::getObjectsPath()).append(foldername).append(filename);

    //Return contents if the object exists
    if(!fs::exists(filepath))  return nullptr;
    else                       return readFile(filepath.c_str());

}

int writeFile(fs::path path, string text)
//Creates or overwrites content of file specified by path with text. Returns non-zero if failure, 0 if success.
{
    fstream file;
	file.open(path.c_str(), ios::out);
	if (!file) {
        //File not created
        return 1;
	}
	else {
        file << text;
		file.close(); 
	}
	return 0;
}
