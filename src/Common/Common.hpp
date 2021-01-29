#pragma once
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem.hpp"
#include <sstream>

namespace fs = boost::filesystem;
using namespace std;

/*
Common functions.
*/

stringstream readFile(fs::path filepath);