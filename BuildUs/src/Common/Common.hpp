#pragma once
#include "boost/filesystem.hpp"
#include <sstream>
#include <vector>
#include <utility>
#include <string>
#include <tuple>
#include <functional>

namespace fs = boost::filesystem;

typedef std::string string;
typedef std::tuple<string,string,string> ThreeStringTuple;
typedef std::vector<ThreeStringTuple> ThreeStringTupleList;
typedef std::vector<string> StringList;
typedef std::vector<std::pair<string,string>> StringPairList;

const string COMPILE_OBJECT_EXT = string(".o");

/*
    Function related to ThreeStringTuple and ThreeStringTupleList
*/
namespace ThreeStringTupleUtils
{
    inline string getOutputFileName(ThreeStringTuple tpl) {return std::get<0>(tpl);}; //SD - passage par reference -2
    inline string getSourceFilePath(ThreeStringTuple tpl) {return std::get<1>(tpl);};
    inline string getSourceSHA1(ThreeStringTuple tpl)     {return std::get<2>(tpl);};
}

/*
    Common functions.
*/

int writeFile(fs::path path, string text);
int readFile(const fs::path& filepath, std::stringstream& contents);

string generateSHA1(string text);
bool isValidSHA1(string hash);

string popenCommand(string command);
int safePopenCommand(string command, string& stdoutOutput);