#include "common.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/uuid/detail/sha1.hpp>
#include <iomanip>

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