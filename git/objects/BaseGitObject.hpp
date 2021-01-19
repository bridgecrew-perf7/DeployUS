#pragma once
#include <string>

using namespace std;

class BaseGitObject
{
    protected:
        string sha1hash;
    public:
        BaseGitObject();
        ~BaseGitObject();

        int addInObjects(string sha1hash, string filecontent);

        inline string getSHA1Hash() {return sha1hash;}; 

};