#include "Common.hpp"
#include "boost/filesystem/fstream.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include <iomanip>
#include <iostream>
#include <cstdarg>

using boost::uuids::detail::sha1;

int readFile(const fs::path& filepath, std::stringstream& contents)
/*
    Updates contents with filecontents.
    Returns non-zero if an error occured. 0 otherwise.
*/
{
    if(!fs::exists(filepath))
    {
        string err;
        err += "Filepath " + filepath.string() + " does not exists.\n";
        std::cout << err;
        return 1;
    }
    //Open file
    fs::ifstream file(filepath);
    //Read all contents
    contents << file.rdbuf();
    //Close file
    file.close();

    return 0;
}

string popenCommand(string command)
/*
    Calls a system command. Returns command output.
*/
{
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      throw std::runtime_error("Can't execute command " + command);
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}

int safePopenCommand(string command, string& stdoutOutput)
// Calls systemCommand and catches error. 
// Stores command output into stdoutOutput 
// Returns non-zero if an error occured. Zero otherwise
{
    try
    {
        popenCommand(command);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
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
    std::stringstream bytestream;
    char* result = new char[41];
    for(int i=0; i < 5; i++) 
    {
        bytestream << std::setfill ('0');
        bytestream << std::setw(sizeof(unsigned int)*2);
        bytestream << std::hex;
        bytestream << hash[i];
    }

    //Storing in result buffer
    bytestream.read(result,40);
    result[40] = '\0'; //null-terminated

    return string(result);
}

bool isValidSHA1(string hash)
//Returns true if is size 40 bytes and contains only hexadecimals. False otherwise
{
    return (hash.size() == 40) && (hash.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos);
}

int writeFile(fs::path path, string text)
//Creates or overwrites content of file specified by path with text. Returns non-zero if failure, 0 if success.
{
    std::fstream file;
	file.open(path.c_str(), std::ios::out);
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

int ThreeStringTupleUtils::countByFunction(ThreeStringTupleList tplList, string sourcefilepath, std::function<string(ThreeStringTuple)> getter )
//Return Number of times sourcefilepath appears in tpl. Requires getter function to access element in ThreeStringTuple
{
    int count = 0;
    for(auto tpl: tplList)
    {
        if(getter(tpl) == sourcefilepath)
            count++;
    }
    return count;
}

StringPairList createStringPairList(int numPairs, string outputfile, string sourcepath, ...)
{
    StringPairList out;
    std::va_list args;
    va_start(args,sourcepath);
    
    //Adding the first compilation file
    std::pair<string,string> p = std::pair<string,string>(outputfile,sourcepath);
    out.push_back(p);

    //Adding subsequent compilation files
    for(int i=0; i < numPairs - 1; i++)
    {
        string output = va_arg(args,string);
        string source = va_arg(args,string);
        p = std::pair<string,string>(output,source);
        out.push_back(p);
    }
    va_end(args);

    return out;
}

StringList createStringList(int num, ...)
{
    StringList out;
    std::va_list args;
    va_start(args,num);

    for(int i = 0; i < num; i++)
    {
        string item = va_arg(args,string);
        out.push_back(item);
    }

    va_end(args);
    return out;
}
