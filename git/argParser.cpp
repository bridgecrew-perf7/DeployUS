#include "argParser.h"
#include <iostream>
#include <string.h>
using namespace std;

const char* HELP_PARAM = "--help";

BaseCommand* parse_args(int argc, char* argv[]){
	BaseCommand* ret = new BaseCommand();
	
	//General HELP
	if(argc == 1) return new HelpCommand(new BaseCommand());
	if (strcmp(argv[1], HELP_PARAM) == 0) return new HelpCommand(new BaseCommand());

	//POSSIBLE ACTIONS include init, add, commit, checkout
	if( strcmp(argv[1], "init") == 0) ret = new InitCommand(argc, argv);
	
	//Verify if help is required. 
	if(argc >= 3 && strcmp(argv[2], HELP_PARAM) == 0) ret = new HelpCommand(ret);

	return ret;
}

