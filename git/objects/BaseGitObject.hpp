#pragma once
#include <string>
#include <common.hpp>

// AB - Arrete de penser trop OO.

class BaseGitObject
{
    protected:
        string sha1hash;        //What the file will be named in the .git/objects folder
        string filecontents;    //What is to be stored in the .git/objects folder

    public:
        BaseGitObject();
        ~BaseGitObject();

        //Interfacing with database
        virtual int addInObjects();

        //Generation
        virtual string generateHash();
        virtual string generateContents();

        //Getters
        inline string getSHA1Hash() {return sha1hash;}; 

};