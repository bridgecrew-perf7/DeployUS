#include "catch2.hpp"
#include <Common/Common.hpp>
#include "unittestscommon.hpp"
#include <boost/filesystem.hpp>
#include "GCCDriver/GCCDriver.hpp"
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void setup()
{
    std::cout.setstate(std::ios::failbit); //Don't display anything
    REQUIRE(clearIntermediate() == 0);
}

void teardown()
{
    REQUIRE(clearIntermediate() == 0);
    std::cout.clear();
}


StringPairList TestSuccessfulCompilation(fs::path configpath)
//Compiles all file in config WITHOUT cleaning aftwards.
//Returns list of compiled files
{
    ConfigFile* config = ConfigFile::safeFactory(configpath);
    REQUIRE(config != nullptr);
    GCCDriver* gcc = GCCDriver::safeFactory(config);
    REQUIRE(gcc != nullptr);
    StringPairList toCompile = gcc->toCompile();

    //Compiling
    REQUIRE(gcc->compile() == 0);

    //Verify files have been compiled properly
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER));
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FILE));
    std::stringstream cachestream = readFile(BUILDUS_CACHE_INTERMEDIATE_FILE);  //Reading cache
    for(auto elem: config->getCompileList())
    {
        //Check object file path
        fs::path objectfilename = fs::path(elem.first);
        objectfilename.replace_extension(objectfilename.extension().string() + COMPILE_OBJECT_EXT);
        fs::path objectFilePath = fs::path(BUILDUS_CACHE_INTERMEDIATE_FOLDER).append(objectfilename.string());
        REQUIRE(fs::exists(objectFilePath));

        /*Check that it matches with content of .cache file*/
        string cacheObjectName = BuildUSCacheUtils::getCacheToken(cachestream);
        string cacheSourceFile = BuildUSCacheUtils::getCacheToken(cachestream);
        string cacheSourceSHA1 = BuildUSCacheUtils::getCacheToken(cachestream);
        REQUIRE(cacheObjectName == objectfilename);
        REQUIRE(cacheSourceFile == elem.second);
        //SHA specifics
        string realSHA1 = generateSHA1(readFile(config->getConfigPath().parent_path().append(cacheSourceFile)).str());
        REQUIRE(isValidSHA1(cacheSourceSHA1));
        REQUIRE(isValidSHA1(realSHA1));
        REQUIRE(cacheSourceSHA1 == realSHA1);  //The right SHA1 must be placed inside cache
        
    }

    return toCompile;
}

TEST_CASE("COMPILATION_SUCCESS_NO_DEPS")
{
    setup();

    //Run Config1,2,3
    StringPairList compiled = TestSuccessfulCompilation(CONFIG1_PATH);
    REQUIRE(compiled.size() == 2);
    compiled = TestSuccessfulCompilation(CONFIG2_PATH);
    REQUIRE(compiled.size() == 0);
    compiled = TestSuccessfulCompilation(CONFIG3_PATH);
    REQUIRE(compiled.size() == 0);

    //Since these all compile the same program, and the .cache is not delete in between
    //And the compiled program only has 2 objects compiled, there should only be two lines in .cache
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FILE));
    std::stringstream cache = readFile(BUILDUS_CACHE_INTERMEDIATE_FILE);
    
    //Quickly count lines using for loop
    int numLines = 0;
    string line;
    for(;std::getline(cache,line);numLines++);
    REQUIRE(numLines == 2);

    
    teardown();
}

TEST_CASE("COMPILATION_SUCCESS_WITH_DEPS")
{
    setup();
    ConfigFile* cf;
    string progfolder = "prog2";

    //Config creation
    std::stringstream configcontents = createConfigStreamForProg(progfolder); 
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG_FAKE_PATH,configcontents));

    //GCCDriver creation
    GCCDriver* gcc = GCCDriver::safeFactory(cf);
    REQUIRE(gcc != nullptr);
    REQUIRE(gcc->compile() == 0); //No errors!
    
    teardown();
}


TEST_CASE("COMPILATION_FAILURE")
{
    setup();
    ConfigFile* cf;
    string progfolder;

    //These programs have intentional but different errors
    SECTION("compileissue1")
    {
        progfolder = "prog_compileissue1";
    }
    SECTION("compilerissue2")
    {
        progfolder = "prog_compileissue2";
    }

    //Config creation
    std::stringstream configcontents = createConfigStreamForProg(progfolder); 
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG_FAKE_PATH,configcontents));

    //GCCDriver creation
    GCCDriver* gcc = GCCDriver::safeFactory(cf, true);
    REQUIRE(gcc != nullptr);
    REQUIRE(gcc->compile() != 0); //Error when compiling!

    teardown();
}