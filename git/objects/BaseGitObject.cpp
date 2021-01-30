#include "BaseGitObject.hpp"
#include <filesystem/GitFilesystem.hpp>
#include <string>
#include <boost/filesystem.hpp>
#include <fstream>
#include <stdexcept>

namespace fs = boost::filesystem;

BaseGitObject::BaseGitObject()
{
    sha1hash = string("");
    filecontents = string("");
}

BaseGitObject::~BaseGitObject()
{

}

int BaseGitObject::addInObjects()
/*Adds file in the objects folder. Returns 1 if the file already exists*/
{
    //1. Seperate SHA1 hash into two parts. First part contains the first two characters, second part contains remaining 38 characters. 
    string folderName = sha1hash.substr(0,2);
    string fileName = sha1hash.substr(2,38);

    //2. Create folder from two hash characters if it doesn't exists
    auto folderPath = GitFilesystem::getObjectsPath().append(folderName);
    if(! fs::exists(folderPath))
    {
        fs::create_directory(folderPath);
    }

    //3. Create file in folder. Check to see that file does not already exists
    auto filePath = folderPath.append(fileName);
    if(fs::exists(filePath))
    {
        //The file has already been added
        return 1;
    }
    std::ofstream blob(filePath.c_str());

    //4. Fill the blob with relevant content. 
    //      In git, this would be compressed.
    //      For ease of correction, contents will be stored in plain text.
    blob << filecontents;
    
    blob.close();
    return 0;
}

string BaseGitObject::generateHash()
{
    throw std::exception();
    return string("");
}

string BaseGitObject::generateContents()
{
    throw std::exception();
    return string("");
}

