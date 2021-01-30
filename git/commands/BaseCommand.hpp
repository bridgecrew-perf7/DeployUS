#pragma once
#include <string>

//Base class for commands

// AB - overkill
//      pourquoi faire une architecture hyper compliqu�e pour 3 ou 4 commandes
//      Si tu aurais 10-20+ commandes, l�, oui....mais pas 3-4
//      comme dit dans l'�nonc� : PAS DE COMMAND DESIGN PATTERN
//      Avec une solution comme ca, tu r�gles des probl�mes que tu n'as pas et tu rajoutes une complexit� au code qui est innutile

class BaseCommand
{
    private:
        std::string sha1hash; // AB - si ce n'est pas utilie partout (genre init), ca ne devrait pas exister
        
    public:
        BaseCommand(/* args */);
        ~BaseCommand();

        //Method to be overwritten
        // AB - � ce niveau l�, fait des fonctions abstraites...
        virtual int execute();
        virtual void help();
};
