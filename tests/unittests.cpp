#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include "unittests.hpp"
#include <common.hpp>
#include <argParser.h>
#include <filesystem/GitFilesystem.hpp>
#include <filesystem/GitIndexFile.hpp>
#include <commands/BaseCommand.hpp>
#include <commands/HelpCommand.hpp>
#include <commands/InitCommand.hpp>
#include <commands/AddCommand.hpp>
#include <commands/CommitCommand.hpp>
#include <commands/CheckoutCommand.hpp>
#include <objects/GitCommit.hpp>
#include <objects/GitBlob.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
using namespace std;

#define TESTFILE_NUMBERS_TXT  			"testfolder1/numbers.txt"
#define TESTFILE_NUMBERS_2_TXT  		"testfolder1/numbers2.txt"
#define TESTFILE_LETTERS_TXT  			"testfolder1/letters.txt"
#define TESTFILE_A_TXT        			"testfolder1/testfolder2/a.txt"
#define TESTFILE_B_TXT        			"testfolder1/testfolder2/b.txt"
#define TESTFILE_NONEXISTANT_TXT        "testfolder1/testfolder2/nonexistant.txt"


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

	//4. TESTING COMMMITCOMMAND HELP (from ./gitus commit --help)
	for(int i = 0; i < buffSize; i++) buffer[i] = '\0'; //clearing buffer
	CheckoutCommand* checkoutcmd = new CheckoutCommand(argc,argv);
	helpcmd = new HelpCommand(checkoutcmd);
	helpcmd->execute();
	stream.read(buffer, buffSize);
	stream.clear();
	expected = "usage: gitus checkout <commitID>\n";
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

TEST_CASE("Add_Command_Failing")
{
	//Removing .git folder
	fs::remove_all(".git");
	fs::remove_all(TESTFILE_B_TXT); //Removing B file

	//Disabling cout
	cout.setstate(ios_base::failbit);

	/*Test1: Add letters.txt for the first time. Fail because no .git folder*/
	string letterspath(TESTFILE_LETTERS_TXT);
	char* argvLetters[3] = {program_invocation_name, strdup("add"), strdup(letterspath.c_str())};
	int argcLetters= 3;
	BaseCommand* addcmd = parse_args(argcLetters,argvLetters);
	REQUIRE(addcmd->execute() != 0); //Error: not .git folder

	//create .git folder	
	char* argv[2] = {program_invocation_name, strdup("init")};
	int argc = 2;

	//Testing message that .git folder already exists
	InitCommand* initcmd = new InitCommand(argc,argv);
	initcmd->execute() == 0;

	/*=======================*/
	/*Add letters.txt for the first time */
	addcmd->execute();

	/*=======================*/
	/*Add numbers.txt for the first time */
	string numberspath(TESTFILE_NUMBERS_TXT);
	char* argvNumber[3] = {program_invocation_name, strdup("add"), strdup(numberspath.c_str())};
	int argcNumber= 3;
	BaseCommand* addcmd2 = parse_args(argcNumber,argvNumber);
	addcmd2->execute();

	/*Add numbers2.txt for the first time. Same content as numbers.txt */
	char* argvNumber2[3] = {program_invocation_name, strdup("add"), strdup(TESTFILE_NUMBERS_2_TXT)};
	argcNumber= 3;
	BaseCommand* addcmd2_numbers2 = parse_args(argcNumber,argvNumber2);
	addcmd2_numbers2->execute();

	/*Test1. Add B.txt, modify file and add again, there should only be one reference of B in index file*/
	execSystemCommand(string("echo qwety > ") + TESTFILE_B_TXT);
	char* argvB[3] = {program_invocation_name, strdup("add"), strdup(TESTFILE_B_TXT)};
	argcNumber= 3;
	BaseCommand* addcmdB= parse_args(argcNumber,argvB);
	addcmdB->execute();
	REQUIRE(addcmdB->execute() != 0); //Should fail as file already inside index

	/*======================*/
	//Test2: Adding numbers.txt again to see it fail
	REQUIRE(addcmd2->execute() != 0);
	/*======================*/

	/*======================*/
	//Test3: Adding a non existant file to see it fail
	string nonexistantpath(TESTFILE_NONEXISTANT_TXT);
	char* argvnonexistant[3] = {program_invocation_name, strdup("add"), strdup(nonexistantpath.c_str())};
	int argcnonexistant= 3;
	AddCommand* addcmd3 = new AddCommand(argcnonexistant,argvnonexistant);
	REQUIRE(addcmd3->execute() != 0);
	/*======================*/

	//Test4: fail when not specifying a file
	char* argvnofile[2] = {program_invocation_name, strdup("add")};
	int argcnofile= 2;
	AddCommand* addcmd4 = new AddCommand(argcnofile,argvnofile);
	REQUIRE(addcmd4->execute() != 0);
	/*======================*/

	//Removing .git folder
	fs::remove_all(".git");
	fs::remove_all(TESTFILE_B_TXT); //Removing B file

	//Last test: fail when no .git folder
	REQUIRE(addcmd3->execute() != 0);
	/*======================*/


	// Redirecting cout to its saved buffer
	cout.clear();


}

TEST_CASE("Add_Command_Success") 
{
	//Removing .git folder
	fs::remove_all(".git");
	fs::remove_all(TESTFILE_B_TXT); //Removing B file

	//Disabling cout
	cout.setstate(ios_base::failbit);

	/*Test1: Add letters.txt for the first time. Fail because no .git folder*/
	string letterspath(TESTFILE_LETTERS_TXT);
	char* argvLetters[3] = {program_invocation_name, strdup("add"), strdup(letterspath.c_str())};
	int argcLetters= 3;
	BaseCommand* addcmd = parse_args(argcLetters,argvLetters);
	REQUIRE(addcmd->execute() != 0); //Error: not .git folder

	//create .git folder	
	char* argv[2] = {program_invocation_name, strdup("init")};
	int argc = 2;
	InitCommand* initcmd = new InitCommand(argc,argv);
	REQUIRE(initcmd->execute() == 0 );

	/*=======================*/
	/*Test1: Add letters.txt for the first time */
	string lettersSHA1 = testSuccessfulAddFile(letterspath);

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
	string numberspath(TESTFILE_NUMBERS_TXT);
	string numbersSHA1 = testSuccessfulAddFile(numberspath);

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
	/*Test2.1: Add numbers2.txt for the first time. Same content as numbers.txt */
	string numbers2path(TESTFILE_NUMBERS_2_TXT);
	string numbers2SHA1 = testSuccessfulAddFile(numbers2path);

	/*Test2.2: Both numbers.txt and numbers2.txt should have the same sha1 hash*/
	REQUIRE(numbersSHA1.compare(numbers2SHA1) == 0);


	/*======================*/
	/*Test2.3: Add B.txt, modify file and add again, there should only be one reference of B in index file*/
	execSystemCommand(string("echo qwety > ") + TESTFILE_B_TXT);
	char* argvB[3] = {program_invocation_name, strdup("add"), strdup(TESTFILE_B_TXT)};
	int argcNumber= 3;
	BaseCommand* addcmdB= parse_args(argcNumber,argvB);
	REQUIRE(addcmdB->execute() == 0);

	GitIndexFile indexFile = GitIndexFile();
	REQUIRE(indexFile.count(string(TESTFILE_B_TXT)) == 1);

	execSystemCommand(string("echo asdfghjkl; > ") + TESTFILE_B_TXT);
	REQUIRE(addcmdB->execute() == 0); //Should pass as B.txt is replaced in index file

	indexFile = GitIndexFile();
	REQUIRE(indexFile.count(string(TESTFILE_B_TXT)) == 1);

	//Removing .git folder
	fs::remove_all(".git");
	fs::remove_all(TESTFILE_B_TXT); //Removing B file


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
	char* argvadd[argc] = {program_invocation_name, strdup("add"), strdup(TESTFILE_LETTERS_TXT)};
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);
	argvadd[2] = strdup(TESTFILE_A_TXT);
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
	argvadd[2] = strdup(TESTFILE_NUMBERS_TXT);
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

TEST_CASE("Checkout_Command")
{
	int argc;
	BaseCommand* cmd;

	//Disabling cout
	cout.setstate(ios_base::failbit);

	//Removing .git folder
	fs::remove_all(".git");

	//Try Checking out with .git folder not initiated
	argc = 3;
	char* argvCheckout[argc] = {program_invocation_name, strdup("checkout"), strdup(generateSHA1(string("Something")).c_str())};
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() != 0);

	//Init
	argc = 2;
	char* argv[argc] = {program_invocation_name, strdup("init")};
	cmd = new InitCommand(argc,argv); 
	REQUIRE(cmd->execute() == 0);

	//Try Checking out with HEAD file empty
	argc = 3;
	argvCheckout[argc-1] = strdup(string("Something").c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() != 0);

	//Add and commit file 1
	argc = 3;
	char* argvadd[argc] = {program_invocation_name, strdup("add"), strdup(TESTFILE_LETTERS_TXT)};
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);
	argc = 4;
	char* argvcommit[argc] = {program_invocation_name, strdup("commit"), strdup("The Message"),strdup("The Author")};
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 0);
	string shaCommit1 = readFile(GitFilesystem::getHEADPath());

	//Add and commit file 2
	argc = 3;
	argvadd[2] = strdup(TESTFILE_A_TXT);
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);
	argc = 4;
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 0);
	string shaCommit2 = readFile(GitFilesystem::getHEADPath());

	//Checkout the first commit
	argc = 3;
	argvCheckout[argc-1] = strdup(shaCommit1.c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() == 0);

	//Verify that the checkout worked
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(readFile(GitFilesystem::getTOPCOMMITPath()).size() == 40);
	REQUIRE(readFile(GitFilesystem::getHEADPath()).compare(shaCommit1) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT)); //Untracked for now
	REQUIRE(!fs::exists(TESTFILE_A_TXT));

	//Add and try to commit file 3 but fail
	argvadd[2] = strdup(TESTFILE_NUMBERS_TXT);
	cmd = new AddCommand(argc,argvadd); 
	REQUIRE(cmd->execute() == 0);
	argc = 4;
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() != 0); //Error
	REQUIRE(readFile(GitFilesystem::getIndexPath()).size() != 0); //Index is not emptied

	//Checkout the top commit
	argc = 3;
	argvCheckout[argc- 1] = strdup(shaCommit2.c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() == 0);
	REQUIRE(!fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(readFile(GitFilesystem::getHEADPath()).compare(shaCommit2) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT)); //Untracked for now
	REQUIRE(fs::exists(TESTFILE_A_TXT));

	//Commit file 3
	argc = 4;
	cmd = new CommitCommand(argc,argvcommit);
	REQUIRE(cmd->execute() == 0);
	string shaCommit3 = readFile(GitFilesystem::getHEADPath());

	//Checkout commit 1 again
	argc = 3;
	argvCheckout[argc-1] = strdup(shaCommit1.c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() == 0);
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(readFile(GitFilesystem::getTOPCOMMITPath()).size() == 40);
	REQUIRE(readFile(GitFilesystem::getHEADPath()).compare(shaCommit1) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_A_TXT));

	//Checkout commit 2
	argc = 3;
	argvCheckout[argc-1] = strdup(shaCommit2.c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() == 0);
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(readFile(GitFilesystem::getTOPCOMMITPath()).size() == 40);
	REQUIRE(readFile(GitFilesystem::getHEADPath()).compare(shaCommit2) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(fs::exists(TESTFILE_A_TXT));

	//Checkout commit 3
	argc = 3;
	argvCheckout[argc-1] = strdup(shaCommit3.c_str());
	cmd = new CheckoutCommand(argc,argvCheckout);
	REQUIRE(cmd->execute() == 0);
	REQUIRE(!fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(readFile(GitFilesystem::getHEADPath()).compare(shaCommit3) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(fs::exists(TESTFILE_A_TXT));

	//Removing .git folder
	fs::remove_all(".git");

	//Reenabling cout
	cout.clear();
	
}

