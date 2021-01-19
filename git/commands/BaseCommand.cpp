#include <iostream>
#include "BaseCommand.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include <iomanip>

using boost::uuids::detail::sha1;
using namespace std;

BaseCommand::BaseCommand(/* args */)
{
}

BaseCommand::~BaseCommand()
{
}

int BaseCommand::execute() {
    return 0;
}

void BaseCommand::help() {
    cout    <<    "usage: gitus <command> [<args>]\n"
            <<    "These are common gitus commands used in various situations:\n"
            <<    "init Create an empty Git repository or reinitialize an existing one\n"
            <<    "add Add file contents to the index\n"
            <<    "commit Record changes to the repository\n";
}


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

string generateBlobHash(string text)
/* Returns SHA-1 of given text*/
{
    string header = "";
    header += "blob ";
    header += to_string(text.size());
    header += '\0';

    return generateSHA1(header + text);
}