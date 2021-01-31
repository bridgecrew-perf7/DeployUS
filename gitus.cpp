#include <common.hpp>
#include "commands/HelpCommand.hpp"
#include "commands/InitCommand.hpp"
#include "commands/AddCommand.hpp"
#include "commands/CommitCommand.hpp"
#include "commands/CheckoutCommand.hpp"
#include <iostream>

const char* INIT_CMD = "init";
const char* ADD_CMD = "add";
const char* COMMIT_CMD = "commit";
const char* CHECKOUT_CMD = "checkout";

namespace GitUS
{
    int help()
    //Prints Gitus usage. Always returns 0 for successful execution.
    {
        std::cout   <<    "usage: gitus <command> [<args>]\n"
                    <<    "These are common gitus commands used in various situations:\n"
                    <<    "init Create an empty Git repository or reinitialize an existing one\n"
                    <<    "add Add file contents to the index\n"
                    <<    "commit Record changes to the repository\n";
        return 0;
    }

}

int main(int argc, char *argv[])
{
    int ret;

	//General HELP
	if(argc == 1) return GitUS::help();

	//POSSIBLE ACTIONS include init, add, commit, checkout
    string command = argv[1];
	if (command.compare(Common::HELP_PARAM)  == 0) return GitUS::help();
	else if( command.compare(INIT_CMD) == 0) ret = InitCommand::execute(argc, argv);
	else if( command.compare(ADD_CMD) == 0) ret = AddCommand::execute(argc, argv);
	else if( command.compare(COMMIT_CMD)  == 0) ret = CommitCommand::execute(argc, argv);
	else if( command.compare(CHECKOUT_CMD) == 0) ret = CheckoutCommand::execute(argc, argv);
	else
	{
		std::cout << "Unknown command\n";
        return 1;
	}

	return ret;
}