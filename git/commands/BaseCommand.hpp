#pragma once
#include <string>

//Base class for commands

// AB - overkill
//      pourquoi faire une architecture hyper compliqu�e pour 3 ou 4 commandes
//      Si tu aurais 10-20+ commandes, l�, oui....mais pas 3-4
//      comme dit dans l'�nonc� : PAS DE COMMAND DESIGN PATTERN
//      Avec une solution comme ca, tu r�gles des probl�mes que tu n'as pas et tu rajoutes une complexit� au code qui est innutile

/* 
    SV: L'énoncé dit que le command design pattern est superlu et inutile; Il ne l'interdit pas. 
        Personellement, ce design pattern m'aide a m'organiser en structurant mon code.
        Ca encapsule mon code avec les variables / les constantes necessaires pour chaque commande. 

        Si je n'avais pas fait de design pattern, j'aurais surment séparer les commandes dans des différents
        fichiers, ce qui serait revenu quasi-ment au même.
*/

class BaseCommand
{
    public:
        BaseCommand(/* args */);
        virtual ~BaseCommand();

        //Method to be overwritten
        virtual int execute();
        virtual void help();
};
