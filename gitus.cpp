#include <iostream>
#include <argParser.h>
#include <commands/BaseCommand.hpp>
#include <commands/InitCommand.hpp>

int main(int argc, char *argv[])
{
    //Determine state of program as described by command line arguments
	BaseCommand* cmd = parse_args(argc, argv);

    //Execute command
    cmd->execute();

    //return with no errors
    return 0;
}



