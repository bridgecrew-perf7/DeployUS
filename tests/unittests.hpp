#include <iostream>
#include <string>
#include <commands/BaseCommand.hpp>
#include <argParser.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <common.hpp>

using namespace std;
namespace fs = boost::filesystem;

void clearBuffer(char* buffer, int size)
// Replaces all contents of buffer with the null terminating character
{
	for(int i = 0; i < size; size++) buffer[i] = '\0';
}


string execSystemCommand(string command) {
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

string testSuccessfulAddFile(string filepath)
//Tests successful addition of file. Return SHA of object
{
   char* argv[3] = {program_invocation_name, strdup("add"), strdup(filepath.c_str())};
	int argc= 3;
	BaseCommand* addcmd = parse_args(argc,argv);
	REQUIRE(addcmd->execute() == 0);

	//Reading letters.txt file
    string contents = readFile(filepath);

	//Finding SHA1 of file. SHA1 must be 40 bytes in size.
	string header = "blob ";
	header += to_string(contents.size());
	header += '\0';
	string sha1 = generateSHA1(header + contents);
	REQUIRE(sha1.size() == 40);

	//Hash must be the same as real git
	string gitResultSHA = execSystemCommand(string("git hash-object ") + filepath);
	boost::trim_right(gitResultSHA);
	REQUIRE(gitResultSHA.compare(sha1) == 0);
	
	//Folder and file must exists
	auto folderPath = fs::path(GitFilesystem::getObjectsPath()).append(sha1.substr(0,2));
	REQUIRE(fs::is_directory(folderPath));
	auto filePath = folderPath.append(sha1.substr(2,38));
	REQUIRE(fs::is_regular_file(filePath));

   return sha1;
}