#include <iostream>
#include <argParser.h>
#include <commands/BaseCommand.hpp>
#include <commands/InitCommand.hpp>

// AB - pourquoi utilis� des hpp? Quelle est l'utilit� de ca?


int main(int argc, char *argv[])
{
    //Determine state of program as described by command line arguments
	BaseCommand* cmd = parse_args(argc, argv);

    //Execute command
    
    // AB - pourquoi retourner quelquechose qui n'est jamais utilis�?
    cmd->execute();



    // AB - fuite de m�moire

    //return with no errors
    return 0;
}



