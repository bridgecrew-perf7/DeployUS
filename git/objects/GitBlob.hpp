#pragma once
#include "BaseGitObject.hpp"
#include <string>

class GitBlob: public BaseGitObject
{
    private: 
        std::string relativePath;
        std::string filecontents;

    public:
        GitBlob(char* path);
        ~GitBlob();

        int addBlobInObjects();
        int addInIndex();
        std::string generateBlobContents();
        std::string generateBlobHash(std::string text);

};