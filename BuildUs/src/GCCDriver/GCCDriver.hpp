#pragma once
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>

namespace fs = boost::filesystem;

class GCCDriver
{
    private:
        ConfigFile config; 
        BuildUSCache cache;
        bool isSysCmdSilent;

    public:
        GCCDriver(ConfigFile& _config, bool silentSysCmd = false);
        GCCDriver();
        ~GCCDriver();

        //Creation of executable steps
        int compile();
        int link();

        //Compilation methods
        int toCompile(StringPairList& filesToCompile);
        int mustLink();
};

namespace GCCDriverUtils
{
    const string GCC_COMPILER = "g++";
    const string GCC_LIB_PREFIX = "lib";
    const string GCC_DOT_A_EXT = ".a";
    const string GCC_DOT_SO_EXT = ".so";
    
    //Helper functions
    string generateCompilationCommand(ConfigFile& config, fs::path filepath, fs::path destination);
}
