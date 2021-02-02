#pragma once
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>

namespace fs = boost::filesystem;

class GCCDriver
{
    private:
        ConfigFile* config; 
        BuildUSCache cache;
        bool isSysCmdSilent;

    public:
        GCCDriver(ConfigFile* _config, bool silentSysCmd = false);
        static GCCDriver* safeFactory(ConfigFile* _config, bool silentSysCmd = false);
        ~GCCDriver();

        //Creation of executable steps
        int compile();

        //Compilation methods
        StringPairList toCompile();
};

namespace GCCDriverUtils
{
    const string GCC_COMPILER = "g++";
    
    //Helper functions
    string generateCompilationCommand(ConfigFile* config, fs::path filepath, fs::path destination);
}
