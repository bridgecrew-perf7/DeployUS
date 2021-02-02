#include "GCCDriver.hpp"
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <FileSystem/BuildUSCache.hpp>
#include <iostream>


GCCDriver::GCCDriver(ConfigFile* _config, bool silentSysCmd)
{
    this->config = _config;
    this->isSysCmdSilent = silentSysCmd;
    cache = BuildUSCache(this->config->getConfigPath().parent_path());
}

GCCDriver* GCCDriver::safeFactory(ConfigFile* _config, bool silentSysCmd)
// Catches all errors. Returns nullptr if an error occured
{
    GCCDriver* out;
    try
    {
        out = new GCCDriver(_config, silentSysCmd);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        out = nullptr;
    }
    return out;       
}

GCCDriver::~GCCDriver()
{

}

StringPairList GCCDriver::toCompile()
//Retreives list of files to compile
{
    return this->cache.getFileForMinimalCompilation(this->config->getCompileList());
}

int GCCDriver::compile()
/*
    Compiles necessary files for minimal 
    Returns 0 if successful, non-zero otherwise
*/
{
    StringPairList filesToCompile = this->toCompile();
    for(auto compileUnit: filesToCompile)
    {
        string outputfilename = compileUnit.first;
        string filepathstr = compileUnit.second;

        std::cout << "Compiling: " << filepathstr << std::endl;
        auto filepath = fs::path(filepathstr);

        //1. Generate source file path
        fs::path sourcefile = config->getConfigPath().parent_path().append(filepathstr);
        
        //2. Generate destination path
        fs::path destPath = BUILDUS_CACHE_INTERMEDIATE_FOLDER;
        destPath.append(outputfilename);
        destPath.replace_extension(destPath.extension().string() + COMPILE_OBJECT_EXT);

        //3. Call system command tu run compiler on file
        fs::create_directories(destPath.parent_path());
        string cmd = GCCDriverUtils::generateCompilationCommand(this->config, sourcefile, destPath);
        
        if(this->isSysCmdSilent) //silence output. Mainly used for unittesting
            cmd += string(" >/dev/null 2>/dev/null");

        if(system(cmd.c_str()))
        {
            std::cout << "Could not compile " << sourcefile << std::endl;
            return 1;
        }
    }

    //4. Update Cache file
    this->cache.updateCompiled(filesToCompile);

    return 0;

}

/*
    ==================================================
                        Helper Functions
    ==================================================
*/

string GCCDriverUtils::generateCompilationCommand(ConfigFile* config, fs::path filepath, fs::path destination)
/*
    Creates command to compile file specified by filepath. 
    Will place the generated assembly object file at destinationpath.
*/
{
    std::stringstream cmd;
    //Compiler
    cmd << GCCDriverUtils::GCC_COMPILER << " ";

    //Include Folder
    for(auto includeDir: config->getDepInclVars())
    {
        cmd << "-I" << getenv(includeDir.c_str()) << " ";
    }

    //Source File
    cmd << "-c ";
    cmd << filepath.string() << " "; 

    //Output
    cmd << "-o ";
    cmd << destination.string() << " "; 

    return cmd.str();
}
