#pragma once
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>

namespace fs = boost::filesystem;

namespace GCCDriverUtils
{
    const string GCC_COMPILER = "g++";
    
    //Helper functions
    string generateCompilationCommand(fs::path filepath, fs::path destination);
}

class GCCDriver
{
    private:
        ConfigFile* config; 
        BuildUSCache cache;

    public:
        GCCDriver(ConfigFile* _config);
        static GCCDriver* safeFactory(ConfigFile* _config);
        ~GCCDriver();

        //Creation of executable steps
        int compile();

        //Compilation methods
        StringPairList toCompile();
};
