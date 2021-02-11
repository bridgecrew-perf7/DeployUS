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
    REQUIRE(cleanIntermediate() == 0);
}

void linkingTeardown()
{
    REQUIRE(cleanIntermediate() == 0);
    std::cout.clear();
}


TEST_CASE("LINKING_SUCCESS")
{
    linkingSetup();
    ConfigFile* cf;
    GCCDriver* gcc;
    fs::path configpath;

    SECTION("PROG1"){configpath = CONFIG_PROG1_PATH;}
    SECTION("PROG2"){configpath = CONFIG_PROG2_PATH;}

    //Load config and compile. This is tested in CompilationTest.cpp
    REQUIRE_NOTHROW(cf = new ConfigFile(configpath));
    REQUIRE_NOTHROW(gcc = new GCCDriver(cf));
    REQUIRE(gcc->compile() == 0);

    //Linking
    REQUIRE(gcc->link() == 0);
    
    //Execution of program
    string programName = cf->getProjectName().at(0);
    string cmd = "./" + programName + " >/dev/null 2>/dev/null";
    REQUIRE(system(cmd.c_str()) == 0);

    linkingTeardown();
}
