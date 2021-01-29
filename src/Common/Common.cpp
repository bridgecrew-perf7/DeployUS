#include "Common.hpp"

stringstream readFile(fs::path filepath)
/*
    Returns stringstream of all characters in file. 
    Throws std::runtime_error if file does not exists.
*/
{
    stringstream contents;
    if(!fs::exists(filepath))
    {
        stringstream msg;
        msg << "Filepath " << filepath.string() << " does not exists.\n";
        throw runtime_error(msg.str());
    }
    //Open file
    fs::ifstream file(filepath);
    //Read all contents
    contents << file.rdbuf();
    //Close file
    file.close();

    return contents;
}

string systemCommand(string command)
/*
    Calls a system command. Returns command output.
*/
{
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return string("popen failed!");
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

