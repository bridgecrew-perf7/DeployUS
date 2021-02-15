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

const char* Common::HELP_PARAM = "--help"; // AB - pourquoi pas string?
const char Common::INDEX_FILE_DELIMETER_INTER  = '\0';
const char* Common::INDEX_FILE_DELIMETER_INTRA  = "\n";

string Common::generateSHA1(string text)
/* Returns SHA-1 of given text*/
{
    sha1 sha;
    unsigned int hash[5];
    int textLength = text.size();

    //Performing the hashing algorithm
    sha.process_bytes(&text[0], textLength);
	sha.get_digest(hash);

    //Converting to hexadecimal
    std::stringstream bytestream;
    char* result = new char[41];
    for(int i=0; i < 5; i++) 
    {
        bytestream << std::setfill('0') ;
        bytestream << std::setw(sizeof(unsigned int)*2);
        bytestream << std::hex <<hash[i];
    }

    //Storing in result buffer
    bytestream.read(result,40);
    result[40] = '\0'; //null-terminated -- AB - pourquoi est-ce qu'on en a besoin? - un commentaire serait appr�ci�

    return string(result);
}

bool Common::isValidSHA1(string text)
//Returns true if text is 40 bytes and is in hexadecimal form. False otherwise.
{
    // AB - des regex ca fait une plus belle job
    //      mais la validation est appréciée
    return (text.size() == 40) &&  (text.find_first_not_of("0123456789abcdefABCDEF") == string::npos);
}

string Common::readFile(const char* path)
/* Helper function to read all content of file */
{
	std::ifstream file(path, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = file.tellg();
    std::vector<char>  contentsVector(pos);
	file.seekg(0, std::ios::beg);
    file.read(&contentsVector[0], pos);
	file.close();

	//Create string from contents
	string filecontents;
    for(char c: contentsVector) 
        filecontents.push_back(c);
	return filecontents;
}

string Common::readFile(const string path)
{
    return Common::readFile(path.c_str());
}

string Common::readFile(const fs::path path)
{
    return Common::readFile(path.c_str());
}

string Common::readGitObject(const string objSHA1)
// Reads the file in the .git/object folder that corresponds to the sha1 passed in parameter
// Throws runtime exception is file does not exists or not valid SHA.
{
    if(!Common::isValidSHA1(objSHA1)) throw std::runtime_error("Not valid SHA1.");

    string foldername = objSHA1.substr(0,2);
    string filename = objSHA1.substr(2,38);
    fs::path filepath = fs::path(GitFilesystem::getObjectsPath()).append(foldername).append(filename);

    //Return contents if the object exists
    if(!fs::exists(filepath))   throw std::runtime_error("No file with this SHA1.");
    else                       return Common::readFile(filepath.c_str());

}

int Common::writeFile(fs::path path, string text)
//Creates or overwrites content of file specified by path with text. Returns non-zero if failure, 0 if success.
{
    std::fstream file;
	file.open(path.c_str(), std::ios::out);
	if (!file) { // AB - constance accolades -2
        //File not created
        return 1;
	}
	else {
        file << text;
		file.close(); 
	}
	return 0;
}

int Common::safeCreateFolder(fs::path folderpath)
//Safely creates folder. Sends error to stderr if one occurs and returns non-zero.
//Returns:  Non-zero if an error occurs or folder not created, zero otherwise
{
    try
    {
        if(!fs::create_directory(folderpath))
        {
            return 1;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

int Common::safeRemove(fs::path path)
//Safely removes file specified by path. Sends error to stderr if one occurs and returns non-zero.
//Returns:  Non-zero if an error occurs or file not removed, zero otherwise
{
    try
    {
        fs::remove(path);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

int Common::safeRemoveAll(fs::path path)
//Safely removes all subpaths contained at inode specified by path. Sends error to stderr if one occurs and returns non-zero.
//Returns:  Non-zero if an error occurs or file not removed, zero otherwise
{
    try
    {
        fs::remove_all(path);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}