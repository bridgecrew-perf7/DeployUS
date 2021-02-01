#pragma once
#include "boost/filesystem/fstream.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include "boost/filesystem.hpp"
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <map>

namespace fs = boost::filesystem;
using boost::uuids::detail::sha1;
using namespace std;

typedef map<string,string> StringStringMap;
typedef vector<string> StringList;

/*
    Common functions.
*/

int writeFile(fs::path path, string text);
stringstream readFile(fs::path filepath);

string systemCommand(string command);
string generateSHA1(string text);
