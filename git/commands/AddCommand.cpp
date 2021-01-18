#include "AddCommand.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>


namespace fs = boost::filesystem;
using namespace std;

AddCommand::AddCommand(int argc, char* argv[])
{
    numArgs = argc;
    args = argv;
}

AddCommand::~AddCommand()
{
}

int AddCommand::execute() {
    //1. Verify that there is one file to be added.
    if (numArgs < 3)
    {
        cout << "Error: no filepath specified.";
        return 1;
    }
    else if (numArgs > 3)
    {
        cout << "Warning, only adding first file specified.";
    }
    
    //2. Verify that a git folder has been initiated
    if (!fs::exists("./.git/") || !fs::is_directory("./.git/"))
    {
        cout << "Error: No git repository has been found.";
        return 1;
    }

    //2. Verify that the file specified exists
    if (!fs::is_regular_file(args[2])) 
    {
        cout << "Error: File does not exists.";
        return 1;
    }

    //3. Create GitBlob object
    GitBlob* gitblob = new GitBlob(args[2]);

    //4. Add blob file in object directory
    if( gitblob->addInObjects())
    {
        cout << "Error: File is already added." << endl;
        return 1;
    }

    //5. Add reference to blob file in the index file.
    if (gitblob->addInIndex())
    {
        cout << "Error: File could not be added in Index file." << endl;
        return 1;
    }
    

    return 0;
}

void AddCommand::help() {
    cout << "usage: gitus add <pathspec>\n";
}


/* ================
    GitBlob Class 
   ================
*/

GitBlob::GitBlob(char* path)
{
    //Saving path of file.
    this->relativePath = path;

    //Open file & read all contents
    ifstream file(path, ios::binary|ios::ate);
    ifstream::pos_type pos = file.tellg();
    vector<char>  contentsVector(pos);
	file.seekg(0, ios::beg);
    file.read(&contentsVector[0], pos);
    file.close();

    //Create string from contents
    for(char c: contentsVector) 
        this->filecontents.push_back(c);

    
    //Get SHA-1 digest of header + file content 
    this->sha1hash = generateBlobHash(this->filecontents);
}

int GitBlob::addInObjects()
/*Adds file in the objects folder. Returns 1 if the blob already exists*/
{
    fs::path objects("./.git/objects");

    //1. Seperate SHA1 hash into two parts. First part contains the first two characters, second part contains remaining 38 characters. 
    string folderName = this->sha1hash.substr(0,2);
    string fileName = this->sha1hash.substr(2,38);

    //2. Create folder from two hash characters if it doesn't exists
    auto folderPath = objects.append(folderName);
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
    ofstream blob(filePath.c_str());

    //4. Fill the blob with relevant content. 
    //      In git, this would be compressed.
    //      For ease of correction, contents will be stored in plain text.
    blob << generateBlobContents();
    
    blob.close();
    return 0;
}

string GitBlob::generateBlobContents()
//Generate the content of the object blob
{
    string blobcontents;

    //1. Adding Filename
    blobcontents.append("File name: ");
    blobcontents.append(this->relativePath);
    blobcontents.push_back('\0'); //Adding null character to show string termination
    blobcontents.push_back('\n');

    //2. Adding file size
    blobcontents.append("File size: ");
    blobcontents.append(to_string(this->filecontents.size()));
    blobcontents.push_back('\0'); //Adding null character to show string termination
    blobcontents.push_back('\n');

    //3. Adding file contents
    blobcontents.append("File content: ");
    blobcontents.append(this->filecontents);
    blobcontents.push_back('\0'); //Adding null character to show string termination
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
    indexfile << '\0';

    //3. Add sha1 hash of file.
    indexfile << this->sha1hash << endl;

    indexfile.close();
    return 0;
}