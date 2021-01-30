#include <argParser.h>

int main(int argc, char *argv[])
{
    //Determine state of program as described by command line arguments
	BaseCommand* cmd = parse_args(argc, argv);

    //Execute command
    int ret = cmd->execute();
    delete cmd;

    //return with no errors
    return ret;
}



