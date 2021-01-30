#include "argParser.h"
#include "commands/UnknownCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/InitCommand.hpp"
#include "commands/AddCommand.hpp"
#include "commands/CommitCommand.hpp"
#include "commands/CheckoutCommand.hpp"
#include <iostream>
#include <string.h>

const char* HELP_PARAM = "--help";

BaseCommand* parse_args(int argc, char* argv[]){
	BaseCommand* ret = new BaseCommand();
	
	//General HELP
	if(argc == 1) return new HelpCommand(new BaseCommand());
	if (strcmp(argv[1], HELP_PARAM) == 0) return new HelpCommand(new BaseCommand());

	//POSSIBLE ACTIONS include init, add, commit, checkout
	if( strcmp(argv[1], "init") == 0) ret = new InitCommand(argc, argv);
	else if( strcmp(argv[1], "add") == 0) ret = new AddCommand(argc, argv);
	else if( strcmp(argv[1], "commit") == 0) ret = new CommitCommand(argc, argv);
	else if( strcmp(argv[1], "checkout") == 0) ret = new CheckoutCommand(argc, argv);
	else
	{
		return new UnknownCommand();
	}
	
	//Verify if help is required. 
	if(argc >= 3 && strcmp(argv[2], HELP_PARAM) == 0) ret = new HelpCommand(ret);

	return ret;
}

