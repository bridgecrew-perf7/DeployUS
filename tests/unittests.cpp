#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include <argParser.h>
#include <commands/BaseCommand.hpp>
#include <commands/HelpCommand.hpp>
#include <commands/InitCommand.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>


namespace fs = boost::filesystem;
using namespace std;

TEST_CASE("Init Command") 
{
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

}

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


	// Redirecting cout to its saved buffer
	cout.rdbuf(sbuf);
}
