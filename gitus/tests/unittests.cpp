#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include "unittests.hpp"
#include <common.hpp>
#include <start.hpp>
#include <filesystem/GitFilesystem.hpp>
#include <filesystem/GitIndexFile.hpp>
#include <objects/GitCommit.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
using namespace GitUSTest;

const fs::path TESTFILE_NUMBERS_TXT = "testfolder1/numbers.txt";
const fs::path TESTFILE_NUMBERS_2_TXT = "testfolder1/numbers2.txt";
const fs::path TESTFILE_LETTERS_TXT  = "testfolder1/letters.txt";
const fs::path TESTFILE_A_TXT  = "testfolder1/testfolder2/a.txt";
const fs::path TESTFILE_B_TXT  = "testfolder1/testfolder2/b.txt";
const fs::path TESTFILE_NONEXISTANT_TXT = "testfolder1/testfolder2/nonexistant.txt";

const string msg1 = "The Message";
const string author1 = "The Author";
const string msg2 = "The Second Message";
const string author2 = "The Second Author";


TEST_CASE("Help_Messages") 
{
	setup();

	int argc = 3;
	int commandIdx = 1;
	int optionIdx = 2;
	char* argv[argc] = {program_invocation_name, strdup("--help"), strdup("")};

	//Base Help
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

	//Init Help
	argv[commandIdx] = strdup("init");
	argv[optionIdx] = strdup("--help");
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

	//Add Help
	argv[commandIdx] = strdup("add");
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);
	
	//Commit Help
	argv[commandIdx] = strdup("commit");
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

	//Checkout Help
	argv[commandIdx] = strdup("checkout");
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

    teardown();
}

TEST_CASE("Init_Command") 
{
    setup();

    int argc = 2;
	char* argv[2] = {program_invocation_name, strdup("init")};

	//Init the .git folder
	REQUIRE(GitUS::GitUSStart(argc,argv) == 0);

	//Assertions that proper files/folders exists
	REQUIRE(fs::is_directory(GitFilesystem::getDotGitPath()));
	REQUIRE(fs::is_directory(GitFilesystem::getObjectsPath()));
	REQUIRE(fs::is_regular_file(GitFilesystem::getIndexPath()));
	REQUIRE(fs::is_regular_file(GitFilesystem::getHEADPath()));

	//Testing message that .git folder already exists
	REQUIRE(GitUS::GitUSStart(argc,argv) != 0);

    teardown();
}

TEST_CASE("Add_Command_Failing")
{
	setup();

    /*======================*/
	//Test setup: Remove reference of b.txt
	REQUIRE(Common::safeRemoveAll(TESTFILE_B_TXT) == 0);
	/*======================*/

    /*======================*/
	/*Test: Add letters.txt for the first time. Fail because no .git folder*/
	int argcAdd = 3;
	char* argvAdd[argcAdd] = {program_invocation_name, strdup("add"), strdup(TESTFILE_LETTERS_TXT.c_str())};
	REQUIRE(GitUS::GitUSStart(argcAdd,argvAdd) != 0); //Should fail as .git not initiated
    /*======================*/

	//create .git folder
	int argcInit = 2;	
	char* argvInit[2] = {program_invocation_name, strdup("init")};
	REQUIRE(GitUS::GitUSStart(argcInit,argvInit) == 0);
	
	/*Add letters.txt for the first time */
	GitUS::GitUSStart(argcAdd,argvAdd);


    /*======================*/
	/*Test:Add letters.txt for the second time. Should fail as already staged. */
	REQUIRE(GitUS::GitUSStart(argcAdd,argvAdd) != 0);
	/*======================*/
	
	/*======================*/
	/*Test: Commit files, and try to add file again. Should fail as file is already tracked and no changes has been made*/
	int argcCommit = 4;
	char* argvCommit[] = {program_invocation_name, strdup("commit"), strdup("The Message"), strdup("The Author")};
	REQUIRE(GitUS::GitUSStart(argcCommit,argvCommit) == 0);
	REQUIRE(GitUS::GitUSStart(argcAdd,argvAdd) != 0);
	/*======================*/

	 /*======================*/
	/*Test: Adding a folder, not a file*/
	argvAdd[argcAdd - 1] = strdup(TESTFILE_LETTERS_TXT.parent_path().c_str()); 
	REQUIRE(GitUS::GitUSStart(argcAdd,argvAdd) != 0); //Should fail, not a regular file
	/*======================*/

	/*======================*/
	//Test3: Adding a non existant file to see it fail
	argvAdd[argcAdd - 1] = strdup(TESTFILE_NONEXISTANT_TXT.c_str()); 
	REQUIRE(GitUS::GitUSStart(argcAdd,argvAdd) != 0); //Should fail as file does not exists
	/*======================*/

	//Test4: fail when not specifying a file
	int argcnofile= 2;
	char* argvnofile[argcnofile] = {program_invocation_name, strdup("add")};
	REQUIRE(GitUS::GitUSStart(argcnofile,argvnofile) != 0); //Should fail as no file specified
	/*======================*/

    //General Teardown
    REQUIRE(Common::safeRemoveAll(TESTFILE_B_TXT) == 0);
	teardown();

}

TEST_CASE("Add_Command_Success") 
{
	//General Setup
	setup();
	REQUIRE(Common::safeRemoveAll(TESTFILE_B_TXT) == 0); //Removing B file

	//create .git folder
	int argcInit = 2;	
	char* argvInit[2] = {program_invocation_name, strdup("init")};
	REQUIRE(GitUS::GitUSStart(argcInit,argvInit) == 0);

	/*=======================*/
	/*Test: Add letters.txt for the first time */
	string lettersSHA1 = testSuccessfulAddFile(TESTFILE_LETTERS_TXT);
	REQUIRE(Common::isValidSHA1(lettersSHA1));

	//The file must be referenced in index file
	GitIndexFile indexfile = GitIndexFile();
	testFileInIndexFile(indexfile, TESTFILE_LETTERS_TXT);
	/*======================*/

	/*=======================*/
	/*Test: Add numbers.txt for the first time */
	string numbersSHA1 = testSuccessfulAddFile(TESTFILE_NUMBERS_TXT);

	//The file must be referenced in index file. Letters.txt should still be in index file
	indexfile = GitIndexFile();
	testFileInIndexFile(indexfile, TESTFILE_LETTERS_TXT);
	testFileInIndexFile(indexfile, TESTFILE_NUMBERS_TXT);
	/*======================*/

	/*======================*/
	/*Test: Add numbers2.txt for the first time. Same content as numbers.txt */
	string numbers2SHA1 = testSuccessfulAddFile(TESTFILE_NUMBERS_2_TXT);

	/*Test: Both numbers.txt and numbers2.txt should have the same sha1 hash*/
	REQUIRE(numbersSHA1.compare(numbers2SHA1) == 0);

	//The file must be referenced in index file. Letters.txt and Numbers.txt should still be in index file
	indexfile = GitIndexFile();
	testFileInIndexFile(indexfile, TESTFILE_LETTERS_TXT);
	testFileInIndexFile(indexfile, TESTFILE_NUMBERS_TXT);
	testFileInIndexFile(indexfile, TESTFILE_NUMBERS_2_TXT);


	/*======================*/
	/*Test: Add B.txt, modify file and add again, there should only be one reference of B in index file*/
	REQUIRE(!fs::exists(TESTFILE_B_TXT));
	execSystemCommand(string("echo qwety > ") + TESTFILE_B_TXT.string());
	REQUIRE(fs::exists(TESTFILE_B_TXT));
	testSuccessfulAddFile(TESTFILE_B_TXT);

	execSystemCommand(string("echo \"qwety for the second time \"> ") + TESTFILE_B_TXT.string());
	REQUIRE(fs::exists(TESTFILE_B_TXT));
	testSuccessfulAddFile(TESTFILE_B_TXT);

    indexfile = GitIndexFile();
	testFileInIndexFile(indexfile, TESTFILE_LETTERS_TXT);
	testFileInIndexFile(indexfile, TESTFILE_NUMBERS_TXT);
	testFileInIndexFile(indexfile, TESTFILE_NUMBERS_2_TXT);
	testFileInIndexFile(indexfile, TESTFILE_B_TXT);
	/*======================*/

	//General Teardown
    REQUIRE(Common::safeRemoveAll(TESTFILE_B_TXT) == 0);
	teardown();
}

TEST_CASE("Commit_Command")
{
	//General Setup
	setup();

	//Commit failing for wrong number of args
	int argcCommitWrongArgs = 5;
	char* argvcommitWrongArgs[argcCommitWrongArgs] = {program_invocation_name, strdup("commit"), strdup(msg1.c_str()),strdup(author1.c_str()), strdup("WrongArgs")};
	REQUIRE(GitUS::GitUSStart(argcCommitWrongArgs,argvcommitWrongArgs) != 0);

	int argcCommitWrongArgs2 = 3;
	char* argvcommitWrongArgs2[argcCommitWrongArgs] = {program_invocation_name, strdup("commit"), strdup(msg1.c_str())};
	REQUIRE(GitUS::GitUSStart(argcCommitWrongArgs2,argvcommitWrongArgs2) != 0);

	//Commit Failing because not .git folder
	int argcCommit = 4;
	char* argvcommit[argcCommit] = {program_invocation_name, strdup("commit"), strdup(msg1.c_str()),strdup(author1.c_str())};
	REQUIRE(GitUS::GitUSStart(argcCommit,argvcommit) != 0);

	//create .git folder
	int argcInit = 2;	
	char* argvInit[2] = {program_invocation_name, strdup("init")};
	REQUIRE(GitUS::GitUSStart(argcInit,argvInit) == 0);

	//Commit Failing because of no staged files
	REQUIRE(GitUS::GitUSStart(argcCommit,argvcommit) != 0);

	/*=======================*/
	/*Test: Add letters.txt for the first time */
	string lettersSHA1 = testSuccessfulAddFile(TESTFILE_LETTERS_TXT);

	//The file must be referenced in index file
	GitIndexFile indexfile = GitIndexFile();
	testFileInIndexFile(indexfile, TESTFILE_LETTERS_TXT);
	/*======================*/
	/*======================*/
	//Commit Files
	REQUIRE(GitUS::GitUSStart(argcCommit,argvcommit) == 0); //Successful since a file is staged
	/*======================*/
	// Verify empty index file
	indexfile = GitIndexFile();
	REQUIRE(indexfile.size() == 0);
	/*======================*/
	/*======================*/
	// HEAD contains SHA of commit
	string commitSHA1 = Common::readFile(GitFilesystem::getHEADPath());
	REQUIRE(Common::isValidSHA1(commitSHA1));  
	/*======================*/
	/*======================*/
	//3. Verify Commit
	GitCommit* commitobj1 = GitCommit::createFromGitObject(commitSHA1);
	REQUIRE(Common::isValidSHA1(commitobj1->getSHA1Hash()));  
	REQUIRE(commitobj1->getSHA1Hash().compare(commitSHA1) == 0);
	REQUIRE(commitobj1->getMsg().compare(msg1) == 0);
	REQUIRE(commitobj1->getAuthor().compare(author1) == 0);
	REQUIRE(commitobj1->getCommitTime().size() > 0);
	REQUIRE(commitobj1->getParentSHA().size() == 0); //No parent as initial commit
	/*======================*/
	/*======================*/
	//Adding a second file
	string numbersSHA1 = testSuccessfulAddFile(TESTFILE_NUMBERS_TXT);

	//Committing new file
	int argcCommit2 = 4;
	char* argvCommit2[argcCommit2] = {program_invocation_name, strdup("commit"), strdup(msg2.c_str()),strdup(author2.c_str())};
	REQUIRE(GitUS::GitUSStart(argcCommit2,argvCommit2) == 0);

	//4. Verify empty index file
	indexfile = GitIndexFile();
	REQUIRE(indexfile.size() == 0);

	//5. HEAD contains SHA of commit
	string commitSHA2 = Common::readFile(GitFilesystem::getHEADPath());
	REQUIRE(Common::isValidSHA1(commitSHA2));
	
	// //6. Verify Commit
	GitCommit* commitobj2 = GitCommit::createFromGitObject(commitSHA2);
	REQUIRE(Common::isValidSHA1(commitobj2->getSHA1Hash()));
	REQUIRE(commitobj2->getSHA1Hash().compare(commitSHA2) == 0);
	REQUIRE(commitobj2->getMsg().compare(msg2) == 0);
	REQUIRE(commitobj2->getAuthor().compare(author2) == 0);
	REQUIRE(commitobj2->getCommitTime().size() > 0);
	REQUIRE(Common::isValidSHA1(commitobj2->getParentSHA()));
	REQUIRE(commitobj2->getParentSHA().compare(commitSHA1) == 0);

	//Commit Failing because of no staged files
	REQUIRE(GitUS::GitUSStart(argcCommit2,argvCommit2) != 0);

	//General Teardown
	teardown();
	
}

TEST_CASE("Checkout_Command")
{
	//General setup
	setup();

	//Should fail as no .git folder and SHA1 of not a commit object
	int argcCheckout = 3;
	char* argvCheckout[argcCheckout] = {program_invocation_name, strdup("checkout"), strdup(Common::generateSHA1(string("Something")).c_str())};
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) != 0);

	//create .git folder
	int argcInit = 2;	
	char* argvInit[2] = {program_invocation_name, strdup("init")};
	REQUIRE(GitUS::GitUSStart(argcInit,argvInit) == 0);

	//Should fail as HEAD empty and No SHA1 of commit
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) != 0);

	//Add and commit file 1
	string lettersSHA1 = testSuccessfulAddFile(TESTFILE_LETTERS_TXT);
	int argcCommit = 4;
	char* argvCommit[argcCommit] = {program_invocation_name, strdup("commit"), strdup(msg1.c_str()), strdup(author1.c_str())};
	REQUIRE(GitUS::GitUSStart(argcCommit,argvCommit) == 0);
	string shaCommit1 = Common::readFile(GitFilesystem::getHEADPath());

	//Add and commit file 2
	string aSHA1 = testSuccessfulAddFile(TESTFILE_A_TXT);
	argvCommit[argcCommit - 2] = strdup(msg2.c_str());
	argvCommit[argcCommit - 1] = strdup(author2.c_str());
	REQUIRE(GitUS::GitUSStart(argcCommit,argvCommit) == 0);
	string shaCommit2 = Common::readFile(GitFilesystem::getHEADPath());

	//Checkout the first commit
	argcCheckout = 3;
	argvCheckout[argcCheckout-1] = strdup(shaCommit1.c_str());
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) == 0);

	//Verify that the checkout worked
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	string topCommit = Common::readFile(GitFilesystem::getTOPCOMMITPath());
	REQUIRE(Common::isValidSHA1(topCommit));
	REQUIRE(Common::readFile(GitFilesystem::getHEADPath()).compare(shaCommit1) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT)); //Untracked for now
	REQUIRE(!fs::exists(TESTFILE_A_TXT));

	//Add and try to commit file 3 but fail
	string numbersSHA1 = testSuccessfulAddFile(TESTFILE_NUMBERS_TXT);
	REQUIRE(GitUS::GitUSStart(argcCommit,argvCommit) != 0);
	REQUIRE(Common::readFile(GitFilesystem::getIndexPath()).size() != 0); //Index is not emptied

	//Checkout the top commit
	argvCheckout[argcCheckout- 1] = strdup(shaCommit2.c_str());
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) == 0);
	REQUIRE(!fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(Common::readFile(GitFilesystem::getHEADPath()).compare(shaCommit2) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT)); //Not commited for now
	REQUIRE(fs::exists(TESTFILE_A_TXT));
	REQUIRE(Common::readFile(GitFilesystem::getIndexPath()).size() != 0); //Index is not emptied

	//Commit file 3
	REQUIRE(GitUS::GitUSStart(argcCommit,argvCommit) == 0);
	string shaCommit3 = Common::readFile(GitFilesystem::getHEADPath());

	//Checkout commit 1 again
	argvCheckout[argcCheckout-1] = strdup(shaCommit1.c_str());
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) == 0);
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	topCommit = Common::readFile(GitFilesystem::getTOPCOMMITPath());
	REQUIRE(Common::isValidSHA1(topCommit));
	REQUIRE(Common::readFile(GitFilesystem::getHEADPath()).compare(shaCommit1) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_A_TXT));

	//Checkout commit 2
	argvCheckout[argcCheckout-1] = strdup(shaCommit2.c_str());
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) == 0);
	REQUIRE(fs::exists(GitFilesystem::getTOPCOMMITPath()));
	topCommit = Common::readFile(GitFilesystem::getTOPCOMMITPath());
	REQUIRE(Common::isValidSHA1(topCommit));
	REQUIRE(Common::readFile(GitFilesystem::getHEADPath()).compare(shaCommit2) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(!fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(fs::exists(TESTFILE_A_TXT));

	//Checkout commit 3
	argvCheckout[argcCheckout-1] = strdup(shaCommit3.c_str());
	REQUIRE(GitUS::GitUSStart(argcCheckout,argvCheckout) == 0);
	REQUIRE(!fs::exists(GitFilesystem::getTOPCOMMITPath()));
	REQUIRE(Common::readFile(GitFilesystem::getHEADPath()).compare(shaCommit3) == 0);
	REQUIRE(fs::exists(TESTFILE_LETTERS_TXT));
	REQUIRE(fs::exists(TESTFILE_NUMBERS_TXT));
	REQUIRE(fs::exists(TESTFILE_A_TXT));

	//General teardown
	teardown();	
}

