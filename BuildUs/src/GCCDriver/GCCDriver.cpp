#include "GCCDriver.hpp"
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>


GCCDriver::GCCDriver(ConfigFile* _config, bool silentSysCmd)
{
    this->config = _config;
    this->isSysCmdSilent = silentSysCmd;
    cache = BuildUSCache(this->config);
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
    StringPairList filesCompiled;
    string errMsg;

    //1. Get files that must be compiled for minimal compilation.
    StringPairList filesToCompile = this->toCompile();
    if(filesToCompile.size() == 0)
    {
        return 0;
    }

    //Compile every file necessary for minimal compilation
    for(auto compileUnit: filesToCompile)
    {
        string outputfilename = compileUnit.first;
        string filepathstr = compileUnit.second;

        std::cout << "Compiling: " << filepathstr << std::endl;
        auto filepath = fs::path(filepathstr);

        //1. Generate source file path
        fs::path sourcefile = this->config->getConfigParentPath().append(filepathstr);
        
        //2. Generate destination path
        fs::path destPath = BUILDUS_CACHE_INTERMEDIATE_FOLDER;
        destPath.append(outputfilename);
        destPath.replace_extension(destPath.extension().string() + COMPILE_OBJECT_EXT);

        //3. Call system command tu run compiler on file
        if(fs::create_directories(destPath.parent_path()))
        {
            errMsg = "Error: Could not write compiled object files. Access denied.\n";
            break;
        }

        //Create command
        string cmd = GCCDriverUtils::generateCompilationCommand(this->config, sourcefile, destPath);
        if(this->isSysCmdSilent) //silence output. Mainly used for unittesting
            cmd += string(" >/dev/null 2>/dev/null");

        //Call command
        if(system(cmd.c_str()))
        {
            errMsg = string("Error: Could not compile ") + sourcefile.string();
            break;
        }
        filesCompiled.push_back(compileUnit);

    }

    //4. Update Cache file
    this->cache.updateCompiled(filesCompiled);

    //5. Throw error if one detected
    if(!errMsg.empty())
    {
        std::cout << errMsg << std::endl;
        return 1;
    }

    return 0;

}

int GCCDriver::link()
//Performs linkage of compiled units with GNU ld
//Returns non-zero if an error occured, zero otherwise
{
    if(!this->cache.mustLink())
    {
        return 0;
    }

    string cmd = "g++ ";

    //1. Append Object files
    for(auto linkUnit: this->config->getCompileList())
    {
        string objectfilename = linkUnit.first;

        //1. Build output file path
        fs::path destPath = BUILDUS_CACHE_INTERMEDIATE_FOLDER;
        destPath.append(objectfilename);
        destPath.replace_extension(destPath.extension().string() + COMPILE_OBJECT_EXT);

        //2. Append to command
        cmd += destPath.string() + " ";
    }

    //2. Add output executable name
    string projectname = this->config->getProjectName();
    cmd += "-o ";
    cmd += projectname;
    cmd += " ";

    //3. Append Library directories
    string libVar = this->config->getDepLibVar();
    if(!libVar.empty())
    {
        string envvarValue = getenv(libVar.c_str());
        cmd += "-L";
        cmd += envvarValue;
        cmd += " ";
    }
    

    //4. Add libraries staticly
    if(this->config->getDepLibList().size() > 0)
    {
        cmd += "-static ";
        for(auto lib: this->config->getDepLibList())
        {
            //Have to remove "lib" from start
            int prefixPos = lib.find(GCCDriverUtils::GCC_LIB_PREFIX);
            if( prefixPos != std::string::npos)
                lib = lib.substr(GCCDriverUtils::GCC_LIB_PREFIX.size());

            //Have to remove ".a" or ".so"
            int extPos = lib.find(GCCDriverUtils::GCC_DOT_A_EXT);
            if( extPos != std::string::npos)
                lib = lib.substr(0,extPos);
            extPos = lib.find(GCCDriverUtils::GCC_DOT_SO_EXT);
            if( extPos != std::string::npos)
                lib = lib.substr(0,extPos);

            cmd += "-l";
            cmd += lib;
            cmd += " ";
        }
    }

    //5, Silence command if necessary
    if(this->isSysCmdSilent) //silence output. Mainly used for unittesting
            cmd += string(" >/dev/null 2>/dev/null");

    //Run command and catch errors
    if(system(cmd.c_str()))
    {
        std::cout << "Error: Could not link files " << std::endl;
        return 1;
    }

    //Update project.cache file
    this->cache.writeProjectCacheToDisk();


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
    string cmd;
    //Compiler
    cmd += GCCDriverUtils::GCC_COMPILER + " ";

    //Include Folder
    string includeDir = config->getDepInclVar();
    if(!includeDir.empty())
    {
        string envVal = getenv(includeDir.c_str());
        cmd += "-I" + envVal + " ";
    }
    

    //Source File
    cmd += "-c ";
    cmd += filepath.string() + " "; 

    //Output
    cmd += "-o ";
    cmd += destination.string() + " "; 

    return cmd;
}
