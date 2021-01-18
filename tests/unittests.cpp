#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include "unittests.hpp"
#include <argParser.h>
#include <commands/BaseCommand.hpp>
#include <commands/HelpCommand.hpp>
#include <commands/InitCommand.hpp>
#include <commands/AddCommand.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
using namespace std;


TEST_CASE("Help Messages") 
{
	char* argv[3] = {program_invocation_name, strdup("init"), strdup("--help")};
	int argc = 3;

	//Catching cout output to see if messages are working
	int buffSize = 300;
	stringstream stream;
	char* buffer = new char[buffSize]();
	// Saving cout's buffer
	streambuf *sbuf = cout.rdbuf();
	// Redirect cout to our stringstream buffer or any other ostream
	cout.rdbuf(stream.rdbuf());

	//1. TESTING BASECOMMAND HELP (from ./gitus --help or ./gitus)
	HelpCommand* helpcmd = new HelpCommand(new BaseCommand());
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	const char* expected =   "usage: gitus <command> [<args>]\n"
                "These are common gitus commands used in various situations:\n"
                "init Create an empty Git repository or reinitialize an existing one\n"
                "add Add file contents to the index\n"
                "commit Record changes to the repository\n";
	REQUIRE( strcmp(expected, buffer) == 0);
	

	//2. TESTING INITCOMMAND HELP (from ./gitus init --help)
	for(int i = 0; i < buffSize; i++) buffer[i] = '\0'; //clearing buffer
	InitCommand* initcmd = new InitCommand(argc,argv);
	helpcmd = new HelpCommand(initcmd);
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	expected = "usage: gitus init\n";
	REQUIRE( strcmp(expected, buffer) == 0);

	//2. TESTING ADDCOMMAND HELP (from ./gitus add --help)
	for(int i = 0; i < buffSize; i++) buffer[i] = '\0'; //clearing buffer
	AddCommand* addcmd = new AddCommand(argc,argv);
	helpcmd = new HelpCommand(addcmd);
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	expected = "usage: gitus add <pathspec>\n";
	REQUIRE( strcmp(expected, buffer) == 0);


	// Redirecting cout to its saved buffer
	cout.rdbuf(sbuf);
}

TEST_CASE("Init Command") 
{
	//Removing .git folder
	fs::remove_all(".git");

	char* argv[2] = {program_invocation_name, strdup("init")};
	int argc = 2;
	
	//Path to needed files/folders
    const fs::path gitDirectory(".git");
    const fs::path objectsDirectory = fs::path(".git").append("objects");
    const fs::path indexDirectory = fs::path(".git").append("index");
    const fs::path headDirectory = fs::path(".git").append("HEAD");

	//Delete .git folder if it exists
	if(fs::exists(".git")) fs::remove_all(".git");

	//Init the .git folder
	BaseCommand* cmd = parse_args(argc,argv);
	cmd->execute();

	//Assertions that proper files/folders exists
	REQUIRE(fs::is_directory(gitDirectory));
	REQUIRE(fs::is_directory(objectsDirectory));
	REQUIRE(fs::is_regular_file(indexDirectory));
	REQUIRE(fs::is_regular_file(headDirectory));

	//Catching cout output to see if messages are working
	int buffSize = 100;
	stringstream stream;
	char* buffer = new char[buffSize]();

	// Saving cout's buffer
	streambuf *sbuf = cout.rdbuf();

	// Redirect cout to our stringstream buffer or any other ostream
	cout.rdbuf(stream.rdbuf());

	//Testing message that .git folder already exists
	InitCommand* initcmd = new InitCommand(argc,argv);
	initcmd->execute();
	stream.getline(buffer, buffSize);
	REQUIRE( strcmp(buffer, "Error: A git repo already exists here.") == 0 );
	
	// Redirecting cout to its saved buffer
	cout.rdbuf(sbuf);

	//Removing .git folder
	fs::remove_all(".git");

}

TEST_CASE("Add Command") 
{
	//Removing .git folder
	fs::remove_all(".git");
	char* argv[2] = {program_invocation_name, strdup("init")};
	int argc = 2;

	//Catching cout output to see if messages are working
	int buffSize = 100;
	stringstream stream;
	char* buffer = new char[buffSize]();

	// Saving cout's buffer
	streambuf *sbuf = cout.rdbuf();

	// Redirect cout to our stringstream buffer or any other ostream
	cout.rdbuf(stream.rdbuf());


	//Testing message that .git folder already exists
	InitCommand* initcmd = new InitCommand(argc,argv);
	initcmd->execute();

	/*=======================*/
	/*Test1: Add letters.txt for the first time */
	string letterspath("testfolder1/letters.txt");
	char* argvLetters[3] = {program_invocation_name, strdup("add"), strdup(letterspath.c_str())};
	int argcLetters= 3;
	BaseCommand* addcmd = parse_args(argcLetters,argvLetters);
	addcmd->execute();

	//Reading letters.txt file
    string letterscontents = readFile(letterspath.c_str());

	//Finding SHA1 of file. SHA1 must be 40 bytes in size.
	string header("blob ");
	header += to_string(letterscontents.size());
	header += '\0';
	string lettersSHA1 = generateSHA1(header + letterscontents);
	REQUIRE(lettersSHA1.size() == 40);
	
	//Folder and file must exists
	auto lettersFolderPath = fs::path("./.git/objects/").append(lettersSHA1.substr(0,2));
	REQUIRE(fs::is_directory(lettersFolderPath));
	auto lettersFilePath = lettersFolderPath.append(lettersSHA1.substr(2,38));
	REQUIRE(fs::is_regular_file(lettersFilePath));

	//The file must be referenced at the first line of the Index file.
	string indexContents = readFile("./.git/index");
	int offset = 0;
	int sepPos = indexContents.find('\0',offset);
	string lettersFilenameIndex = indexContents.substr(offset,sepPos-offset);
	REQUIRE(lettersFilenameIndex.compare(letterspath) == 0); 	//Testing file name
	int newlinePos = indexContents.find('\n',offset);
	string lettersSHA1Index = indexContents.substr(sepPos+1,newlinePos - sepPos-1);
	REQUIRE(lettersSHA1Index.compare(lettersSHA1) == 0);		//Testing sha1
	REQUIRE(lettersSHA1Index.size() == 40);						//Testing sha1 size
	/*======================*/


	/*=======================*/
	/*Test2: Add numbers.txt for the first time */
	string numberspath("testfolder1/numbers.txt");
	char* argvNumber[3] = {program_invocation_name, strdup("add"), strdup(numberspath.c_str())};
	int argcNumber= 3;
	BaseCommand* addcmd2 = parse_args(argcNumber,argvNumber);
	addcmd2->execute();

	//Reading letters.txt file
    string numberscontents = readFile(numberspath.c_str());

	//Finding SHA1 of file. SHA1 must be 40 bytes in size.
	header = "blob ";
	header += to_string(numberscontents.size());
	header += '\0';
	string numbersSHA1 = generateSHA1(header + numberscontents);
	REQUIRE(numbersSHA1.size() == 40);
	
	//Folder and file must exists
	auto numbersFolderPath = fs::path("./.git/objects/").append(numbersSHA1.substr(0,2));
	REQUIRE(fs::is_directory(numbersFolderPath));
	auto numbersFilePath = numbersFolderPath.append(numbersSHA1.substr(2,38));
	REQUIRE(fs::is_regular_file(numbersFilePath));

	//The file must be referenced at the first line of the Index file.
	indexContents = readFile("./.git/index");
	offset = indexContents.find('\n');
	sepPos = indexContents.find('\0',offset);
	string numbersFilenameIndex = indexContents.substr(offset+1,sepPos-offset-1);
	REQUIRE(numbersFilenameIndex.compare(numberspath) == 0); 	//Testing file name
	newlinePos = indexContents.find('\n',offset+1);
	string numbersSHA1Index = indexContents.substr(sepPos+1,newlinePos - sepPos-1);
	REQUIRE(numbersSHA1Index.compare(numbersSHA1) == 0);		//Testing sha1
	REQUIRE(numbersSHA1Index.size() == 40);						//Testing sha1 size
	/*======================*/

	/*======================*/
	//Test3: Adding numbers.txt again to see it fail
	addcmd2->execute();
	stream.getline(buffer, buffSize);
	stream.clear();
	REQUIRE( strcmp(buffer, "Error: File is already added.") == 0 );
	clearBuffer(buffer,buffSize);
	/*======================*/

	/*======================*/
	//Test4: Adding a non existant file to see it fail
	string nonexistantpath("testfolder2/nonexistant.txt");
	char* argvnonexistant[3] = {program_invocation_name, strdup("add"), strdup(nonexistantpath.c_str())};
	int argcnonexistant= 3;
	AddCommand* addcmd3 = new AddCommand(argcnonexistant,argvnonexistant);
	addcmd3->execute();
	stream.getline(buffer, buffSize);
	stream.clear();
	REQUIRE(strlen(buffer) == strlen("Error: File does not exists."));
	REQUIRE( strcmp(buffer, "Error: File does not exists.") == 0 );
	clearBuffer(buffer,buffSize);
	/*======================*/

	//Test5: fail when not specifying a file
	char* argvnofile[2] = {program_invocation_name, strdup("add")};
	int argcnofile= 2;
	AddCommand* addcmd4 = new AddCommand(argcnofile,argvnofile);
	addcmd4->execute();
	stream.getline(buffer, buffSize);
	stream.clear();
	REQUIRE( strcmp(buffer, "Error: no filepath specified.") == 0 );
	clearBuffer(buffer,buffSize);
	/*======================*/

	//Removing .git folder
	fs::remove_all(".git");

	//Last test: fail when no .git folder
	addcmd3->execute();
	stream.getline(buffer, buffSize);
	stream.clear();
	REQUIRE( strcmp(buffer, "Error: No git repository has been found.") == 0 );
	clearBuffer(buffer,buffSize);
	/*======================*/


	// Redirecting cout to its saved buffer
	cout.rdbuf(sbuf);

}

