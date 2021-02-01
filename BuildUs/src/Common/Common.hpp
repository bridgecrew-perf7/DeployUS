#pragma once
#include "boost/filesystem.hpp"
#include <sstream>
#include <vector>
#include <utility>
#include <string>
#include <map>

namespace fs = boost::filesystem;

typedef std::string string;
typedef std::map<string,string> StringStringMap;
typedef std::vector<string> StringList;
typedef std::vector<std::pair<string,string>> StringPairList;

/*
    Common functions.
*/

int writeFile(fs::path path, string text);
std::stringstream readFile(fs::path filepath);

string systemCommand(string command);
string generateSHA1(string text);
