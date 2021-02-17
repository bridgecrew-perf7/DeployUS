#include "catch2.hpp"
#include <Common/Common.hpp>
#include "unittestscommon.hpp"
#include <boost/filesystem.hpp>
#include "GCCDriver/GCCDriver.hpp"
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void setupCompilationTest()
{
    std::cout.setstate(std::ios::failbit); //Don't display anything
    REQUIRE(cleanIntermediate() == 0);
}

void teardownCompilationTest()
{
    REQUIRE(cleanIntermediate() == 0);
    std::cout.clear();
}


StringPairList TestSuccessfulCompilation(fs::path configpath)
//Compiles all file in config WITHOUT cleaning aftwards.
//Returns list of compiled files
{
    std::stringstream configcontents;
    string err;
    StringPairList toCompile;

    REQUIRE(readFile(configpath,configcontents) == 0);
    ConfigFile config = ConfigFile(configpath, configcontents);
    REQUIRE(config.isConfigValid(err) == 0);
    GCCDriver gcc = GCCDriver(config);
    REQUIRE(gcc.toCompile(toCompile) == 0);

    //Compiling
    REQUIRE(gcc.compile() == 0);
    StringPairList toCompile_afterCompilation; //Checking that no more files need to compile
    REQUIRE(gcc.toCompile(toCompile_afterCompilation) == 0);
    REQUIRE(toCompile_afterCompilation.size() == 0); //All files have been compiled

    //Verify files have been compiled properly
    REQUIRE(fs::exists(BuildUSCacheUtils::INTERMEDIATE_FOLDER));
    REQUIRE(fs::exists(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE));
    std::stringstream cachestream;
    REQUIRE(readFile(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE,cachestream) == 0);  //Reading cache
    for(auto elem: config.getCompileList())
    {
        //Check object file path
        fs::path objectfilename = fs::path(elem.first);
        objectfilename.replace_extension(objectfilename.extension().string() + COMPILE_OBJECT_EXT);
        fs::path objectFilePath = fs::path(BuildUSCacheUtils::INTERMEDIATE_FOLDER).append(objectfilename.string());
        REQUIRE(fs::exists(objectFilePath));

        /*Check that it matches with content of .cache file*/
        string cacheObjectName = BuildUSCacheUtils::getCacheToken(cachestream);
        string cacheSourceFile = BuildUSCacheUtils::getCacheToken(cachestream);
        string cacheSourceSHA1 = BuildUSCacheUtils::getCacheToken(cachestream);
        REQUIRE(cacheObjectName == objectfilename);
        REQUIRE(cacheSourceFile == elem.second);

        //SHA specifics
        std::stringstream cacheSourceContents;
        fs::path sourcefilepath = config.getConfigParentPath().append(cacheSourceFile);
        REQUIRE(readFile(sourcefilepath,cacheSourceContents) == 0);
        string realSHA1 = generateSHA1(cacheSourceContents.str());
        REQUIRE(isValidSHA1(cacheSourceSHA1));
        REQUIRE(isValidSHA1(realSHA1));
        REQUIRE(cacheSourceSHA1 == realSHA1);  //The right SHA1 must be placed inside cache
        
    }

    return toCompile;
}

TEST_CASE("COMPILATION_SUCCESS_NO_DEPS")
{
    setupCompilationTest();

    //Run Config1,2projectnames as they compile the same program.
    StringPairList compiled = TestSuccessfulCompilation(CONFIG_PROG1_PATH);
    REQUIRE(compiled.size() == 2);

    //Compile the same program! It is a different config file but it compiles the same program. 
    //The intermediate files are already built, therefore there is no need to recompile them. Check that they were not recompiled.
    compiled = TestSuccessfulCompilation(CONFIG_2PROJECTNAMES_PATH);
    REQUIRE(compiled.size() == 0);

    //Since these all compile the same program, and the .cache is not delete in between
    //And the compiled program only has 2 objects compiled, there should only be two lines in .cache
    REQUIRE(fs::exists(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE));
    std::stringstream cache;
    REQUIRE(readFile(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE,cache) == 0);
    
    //Quickly count lines using for loop
    int numLines = 0;
    string line;
    for(;std::getline(cache,line);numLines++);
    REQUIRE(numLines == 2);

    
    teardownCompilationTest();
}

TEST_CASE("COMPILATION_SUCCESS_WITH_DEPS")
{
    setupCompilationTest();
    std::stringstream configcontents;
    string err;
    
    REQUIRE(readFile(CONFIG_PROG2_PATH,configcontents) == 0);
    ConfigFile cf = ConfigFile(CONFIG_PROG2_PATH,configcontents);
    REQUIRE(cf.isConfigValid(err) == 0);

    //GCCDriver creation
    GCCDriver gcc = GCCDriver(cf,true);
    REQUIRE(gcc.compile() == 0); //No errors!
    
    teardownCompilationTest();
}


TEST_CASE("COMPILATION_FAILURE")
{
    setupCompilationTest();
    ConfigFile cf;
    fs::path configpath;
    std::stringstream configcontents;
    string err;

    //These programs have intentional but different errors
    SECTION("compileissue1")
    {
        configpath = CONFIG_COMPILEISSUE1_PATH;
    }
    SECTION("compilerissue2")
    {
        configpath = CONFIG_COMPILEISSUE2_PATH;
    }

    //Config creation
    REQUIRE(readFile(configpath,configcontents) == 0);
    cf = ConfigFile(configpath,configcontents);
    REQUIRE(cf.isConfigValid(err) == 0);

    //GCCDriver creation
    GCCDriver gcc = GCCDriver(cf, true);
    REQUIRE(gcc.compile() != 0); //Error when compiling!

    teardownCompilationTest();
}