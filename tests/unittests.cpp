#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include "unittests.hpp"
#include <common.hpp>
#include <argParser.h>
#include <commands/BaseCommand.hpp>
#include <commands/HelpCommand.hpp>
#include <commands/InitCommand.hpp>
#include <commands/AddCommand.hpp>
#include <commands/CommitCommand.hpp>
#include <objects/GitCommit.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
using namespace std;


TEST_CASE("Help_Messages") 
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

	//3. TESTING ADDCOMMAND HELP (from ./gitus add --help)
	for(int i = 0; i < buffSize; i++) buffer[i] = '\0'; //clearing buffer
	AddCommand* addcmd = new AddCommand(argc,argv);
	helpcmd = new HelpCommand(addcmd);
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	expected = "usage: gitus add <pathspec>\n";
	REQUIRE( strcmp(expected, buffer) == 0);

	//4. TESTING COMMMITCOMMAND HELP (from ./gitus commit --help)
	for(int i = 0; i < buffSize; i++) buffer[i] = '\0'; //clearing buffer
	CommitCommand* commitcmd = new CommitCommand(argc,argv);
	helpcmd = new HelpCommand(commitcmd);
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	expected = "usage: gitus commit <msg> <author>\n";
	REQUIRE( strcmp(expected, buffer) == 0);


	// Redirecting cout to its saved buffer
	cout.rdbuf(sbuf);
}

TEST_CASE("Init_Command") 
{
	//Disabling cout
	cout.setstate(ios_base::failbit);

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

	//Testing message that .git folder already exists
	InitCommand* initcmd = new InitCommand(argc,argv);
	REQUIRE(initcmd->execute() != 0) ;

	//Removing .git folder
	fs::remove_all(".git");

	//Reenabling cout
	cout.clear();

}

TEST_CASE("Add_Command") 
{
	//Disabling cout
	cout.setstate(ios_base::failbit);

	//Removing .git folder
	fs::remove_all(".git");
	char* argv[2] = {program_invocation_name, strdup("init")};
	int argc = 2;

	//Testing message that .git folder already exists
	InitCommand* initcmd = new InitCommand(argc,argv);
	REQUIRE(initcmd->execute() == 0 );

	/*=======================*/
	/*Test1: Add letters.txt for the first time */
	string letterspath("testfolder1/letters.txt");
	char* argvLetters[3] = {program_invocation_name, strdup("add"), strdup(letterspath.c_str())};
	int argcLetters= 3;
	BaseCommand* addcmd = parse_args(argcLetters,argvLetters);
	REQUIRE(addcmd->execute() == 0);

	//Reading letters.txt file
    string letterscontents = readFile(letterspath.c_str());

	//Finding SHA1 of file. SHA1 must be 40 bytes in size.
	string header("blob ");
	header += to_string(letterscontents.size());
	header += '\0';
	string lettersSHA1 = generateSHA1(header + letterscontents);
	REQUIRE(lettersSHA1.size() == 40);
	
	//Folder and file must exists
	auto lettersFolderPath = fs::path(GitFilesystem::getObjectsPath()).append(lettersSHA1.substr(0,2));
	REQUIRE(fs::is_directory(lettersFolderPath));
	auto lettersFilePath = lettersFolderPath.append(lettersSHA1.substr(2,38));
	REQUIRE(fs::is_regular_file(lettersFilePath));

	//The file must be referenced at the first line of the Index file.
	string indexContents = readFile(GitFilesystem::getIndexPath().c_str());
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
	auto numbersFolderPath = fs::path(GitFilesystem::getObjectsPath()).append(numbersSHA1.substr(0,2));
	REQUIRE(fs::is_directory(numbersFolderPath));
	auto numbersFilePath = numbersFolderPath.append(numbersSHA1.substr(2,38));
	REQUIRE(fs::is_regular_file(numbersFilePath));

	//The file must be referenced at the first line of the Index file.
	indexContents = readFile(GitFilesystem::getIndexPath().c_str());
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
	REQUIRE(addcmd2->execute() != 0);
	/*======================*/

	/*======================*/
	//Test4: Adding a non existant file to see it fail
	string nonexistantpath("testfolder2/nonexistant.txt");
	char* argvnonexistant[3] = {program_invocation_name, strdup("add"), strdup(nonexistantpath.c_str())};
	int argcnonexistant= 3;
	AddCommand* addcmd3 = new AddCommand(argcnonexistant,argvnonexistant);
	REQUIRE(addcmd3->execute() != 0);
	/*======================*/

	//Test5: fail when not specifying a file
	char* argvnofile[2] = {program_invocation_name, strdup("add")};
	int argcnofile= 2;
	AddCommand* addcmd4 = new AddCommand(argcnofile,argvnofile);
	REQUIRE(addcmd4->execute() != 0);
	/*======================*/

	//Removing .git folder
	fs::remove_all(".git");

	//Last test: fail when no .git folder
	REQUIRE(addcmd3->execute() != 0);
	/*======================*/


	// Redirecting cout to its saved buffer
	cout.clear();

}

TEST_CASE("Commit_Command")
{
	//Disabling cout
	cout.setstate(ios_base::failbit);

	//Removing .git folder
	fs::remove_all(".git");

	//Commit Failing because not .git folder
	int argc = 4;
	char* argvcommit[argc] = {program_invocation_name, strdup("commit"), strdup("The Message"),strdup("The Author")};
	BaseCommand* cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 1); // Error

	//Init
	argc = 2;
	char* argv[argc] = {program_invocation_name, strdup("init")};
	cmd = new InitCommand(argc,argv); 
	REQUIRE(cmd->execute() == 0);

	//Commit Failing because of no staged files
	argc = 4;
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 1); // Error

	//Add test files
	argc = 3;
	char* argvadd[argc] = {program_invocation_name, strdup("add"), strdup("testfolder1/letters.txt")};
	cmd = new AddCommand(argc,argvadd); 
	cmd->execute();
	argvadd[2] = strdup("testfolder1/testfolder2/a.txt");
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);

	//Commit Files
	argc = 4;
	argvcommit[2] = strdup("The Message");
	argvcommit[3] = strdup("The Author");
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 0);

	//1. Verify empty index file
	REQUIRE(readFile(".git/index").size() == 0); 	

	//2. HEAD contains SHA of commit
	string commitSHA1 = readFile(GitFilesystem::getHEADPath().c_str());
	REQUIRE(commitSHA1.size() == 40);  

	//3. Verify Commit
	GitCommit* commitobj1 = GitCommit::createFromGitObject(commitSHA1);
	REQUIRE(commitobj1->getSHA1Hash().size() == 40);
	REQUIRE(commitobj1->getSHA1Hash().compare(commitSHA1) == 0);
	REQUIRE(commitobj1->getMsg().compare("The Message") == 0);
	REQUIRE(commitobj1->getAuthor().compare("The Author") == 0);
	REQUIRE(commitobj1->getCommitTime().size() > 0);
	REQUIRE(commitobj1->getParentSHA().size() == 0);


	//Adding a third file
	argc = 3;
	argvadd[2] = strdup("testfolder1/numbers.txt");
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);

	//Committing new file
	argc = 4;
	argvcommit[2] = strdup("The Second Message");
	argvcommit[3] = strdup("The Second Author");
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 0);

	//4. Verify empty index file
	REQUIRE(readFile(GitFilesystem::getIndexPath().c_str()).size() == 0); 	

	//5. HEAD contains SHA of commit
	string commitSHA2 = readFile(GitFilesystem::getHEADPath().c_str());
	REQUIRE(commitSHA2.size() == 40);  
	
	//6. Verify Commit
	GitCommit* commitobj2 = GitCommit::createFromGitObject(commitSHA2);
	REQUIRE(commitobj2->getSHA1Hash().size() == 40);
	REQUIRE(commitobj2->getSHA1Hash().compare(commitSHA2) == 0);
	REQUIRE(commitobj2->getMsg().compare("The Second Message") == 0);
	REQUIRE(commitobj2->getAuthor().compare("The Second Author") == 0);
	REQUIRE(commitobj2->getCommitTime().size() > 0);
	REQUIRE(commitobj2->getParentSHA().compare(commitSHA1) == 0);

	//Commit Failing because of no staged files
	argc = 4;
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() != 0); // Error

	//Removing .git folder
	fs::remove_all(".git");

	//Reenabling cout
	cout.clear();
	
}

