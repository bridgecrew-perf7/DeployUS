#include "catch.hpp"
#include <common.hpp>
#include <start.hpp>
#include <boost/filesystem.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <filesystem/GitIndexFile.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;

namespace GitUSTest
{
	void setup()
	{
		//Disabling cout and cerr
		std::cout.setstate(std::ios_base::failbit);
		std::cerr.setstate(std::ios_base::failbit);
		
		//Removing .git folder
		REQUIRE(Common::safeRemoveAll(GitFilesystem::getDotGitPath()) == 0);
	}

	void teardown()
	{
		//Removing .git folder
		REQUIRE(Common::safeRemoveAll(GitFilesystem::getDotGitPath()) == 0);

		//re-enabling cout and cerr
		std::cerr.clear();
		std::cout.clear();
	}

	string execSystemCommand(string command)
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

	string findGenuineGitHash(const fs::path filepath)
	//Returns the sha1 hash that git would have calculated
	{
		//Reading filepath file
		string contents = Common::readFile(filepath);

		//Finding SHA1 of file. SHA1 must be 40 bytes in size.
		string header = "blob ";
		header += std::to_string(contents.size());
		header += '\0';
		string sha1 = Common::generateSHA1(header + contents);
		REQUIRE(Common::isValidSHA1(sha1));
		return sha1;
	}

	string testSuccessfulAddFile(const fs::path filepath)
	//Tests successful addition of file. Return SHA of object
	{
		int argc = 3;
		char* argv[argc] = {program_invocation_name, strdup("add"), strdup(filepath.c_str())};
		REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

		//Hash must be the same as real git
		string sha1 = findGenuineGitHash(filepath);
		string gitResultSHA = execSystemCommand(string("git hash-object ") + filepath.string());
		boost::trim_right(gitResultSHA);
		REQUIRE(gitResultSHA.compare(sha1) == 0);
		
		//Folder and file must exists
		auto folderPath = GitFilesystem::getObjectsPath().append(sha1.substr(0,2));
		REQUIRE(fs::is_directory(folderPath));
		auto filePath = folderPath.append(sha1.substr(2,38));
		REQUIRE(fs::is_regular_file(filePath));

		return sha1;
	}

	void testFileInIndexFile(GitIndexFile indexfile, const fs::path filepath)
	//Tests to see if file pointed by filepath is in the indexfile.
	{
		REQUIRE(indexfile.count(filepath) == 1);		//Only 1 instance
		string SHA1Index = indexfile.getBlobReference(filepath).generateHash();
		REQUIRE(Common::isValidSHA1(SHA1Index));		//Testing sha1 size

		//Finding SHA1 of file. SHA1 must be 40 bytes in size.
		string sha1GIT = findGenuineGitHash(filepath);
		REQUIRE(SHA1Index.compare(sha1GIT) == 0);		//Testing sha1 equals right thing
	}
}