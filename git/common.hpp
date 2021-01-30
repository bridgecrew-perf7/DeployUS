#pragma once
#include <string>
#include <boost/filesystem.hpp>

#define INDEX_FILE_DEMLIMETER '\0'

namespace fs = boost::filesystem;
typedef std::string string;

namespace Common
{
    string generateSHA1(string text);

    string readFile(const char* path);
    string readFile(const string path);
    string readFile(const fs::path path);
    string readGitObject(const string objSHA1);

    int writeFile(fs::path path, string text);
}
