#pragma once
#include <string>
#include <boost/filesystem.hpp>

#define INDEX_FILE_DEMLIMETER '\0'

namespace fs = boost::filesystem;
using namespace std;

string generateSHA1(string text);

string readFile(const char* path);
string readFile(const string path);
string readFile(const fs::path path);
string readGitObject(const string objSHA1);