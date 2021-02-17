#include "catch2.hpp"
#include <Common/Common.hpp>
#include "unittestscommon.hpp"
#include <boost/filesystem.hpp>
#include "GCCDriver/GCCDriver.hpp"
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void linkingSetup()
{
    std::cout.setstate(std::ios::failbit); //Don't display anything
    std::cerr.setstate(std::ios::failbit); //Don't display anything
    REQUIRE(cleanIntermediate() == 0);
}

void linkingTeardown()
{
    REQUIRE(cleanIntermediate() == 0);
    std::cout.clear();
    std::cerr.clear();
}


TEST_CASE("LINKING_SUCCESS")
{
    linkingSetup();
    ConfigFile cf;
    GCCDriver gcc;
    fs::path configpath;
    std::stringstream configcontents;
    string err;

    SECTION("PROG1"){configpath = CONFIG_PROG1_PATH;}
    SECTION("PROG2"){configpath = CONFIG_PROG2_PATH;}

    //Load config and compile. This is tested in CompilationTest.cpp
    REQUIRE(readFile(configpath,configcontents) == 0);
    cf = ConfigFile(configpath, configcontents);
    REQUIRE(cf.isConfigValid(err) == 0);
    gcc = GCCDriver(cf,true);
    REQUIRE(gcc.compile() == 0);

    //Linking
    REQUIRE(gcc.link() == 0);
    
    //Execution of program
    string programName = cf.getProjectName();
    string cmd = "./" + programName + " >/dev/null 2>/dev/null";
    REQUIRE(system(cmd.c_str()) == 0);

    linkingTeardown();
}

TEST_CASE("LINKING_FAILURE")
{
    linkingSetup();
    ConfigFile cf;
    GCCDriver gcc;
    fs::path configpath;
    std::stringstream configcontents;
    string err;

    SECTION("Libs do not exixsts")
    {
        configpath = CONFIG_LIBS_DNE_PATH;
    }
    SECTION("Forward Declaration does not have definition.")
    {
        configpath = CONFIG_LINKERISSUE1_PATH;
    }
    SECTION("Function does not exists in namespace.")
    {
        configpath = CONFIG_LINKERISSUE2_PATH;
    }

    //Compile
    REQUIRE(readFile(configpath,configcontents) == 0);
    cf = ConfigFile(configpath, configcontents);
    REQUIRE(cf.isConfigValid(err) == 0);
    gcc = GCCDriver(cf,true);
    REQUIRE(gcc.compile() == 0);

    //Linking. Must be a failure!
    REQUIRE(gcc.link() != 0);

    linkingTeardown();
}
