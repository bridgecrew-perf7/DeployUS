#pragma once
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
typedef std::string string;

namespace Common
{
    extern const char* HELP_PARAM;
    extern const char INDEX_FILE_DELIMETER_INTER;
    extern const char* INDEX_FILE_DELIMETER_INTRA;

    string generateSHA1(string text);
    bool isValidSHA1(string text);

    string readFile(const char* path);
    string readFile(const string path);
    string readFile(const fs::path path);
    string readGitObject(const string objSHA1);

    int writeFile(fs::path path, string text);

    //Safe functions
    int safeCreateFolder(fs::path folderpath);
    int safeRemove(fs::path path);
    int safeRemoveAll(fs::path path);
}
