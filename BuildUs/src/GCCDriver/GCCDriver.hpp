#pragma once
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>
#include <Common/Common.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

#define GCC_COMPILER "g++"

namespace fs = boost::filesystem;
using namespace std;

class GCCDriver
{
private:
    ConfigFile* config; 
    BuildUSCache cache;

    //Compilation methods
    StringList toCompile();


public:
    GCCDriver(ConfigFile* _config);
    ~GCCDriver();

    //Creation of executable steps
    void compile();
};

//Helper functions
string generateCompilationCommand(fs::path filepath, fs::path destination);
