/* Commentaires et note

    Fuite de mémoire    -2
    Effet de bord       -2
    Passage par copie   -2
    Mauvais commentaire -2
    Code sur-complexe   -2
    Code mort           -2
    Mauvais accès       -2
    Exception possible  -5
    Pointeurs inutile   -2
    Constance code      -2
    
    Checkout fait +10

Tests
    Manque quelque uns mais très, très peux

    30/30 (25+5)


    Total:  92/100


Commentaires:
    Sommairement, le code est encore très complexe (mais beaucoup moins qu'avant, félicitation pour ca)
    Je vois que tu as fait beaucoup d'effort pour le C++. Belle utilisation de certaines techniques.

    Bon travail.
*/



#include <start.hpp>

int main(int argc, char* argv[])
{
    // AB - pourquoi ne pas avoir transformer argv en vector<string>?
    //      c'est beaucoup plus facile à travailler
    return GitUS::GitUSStart(argc,argv);
}