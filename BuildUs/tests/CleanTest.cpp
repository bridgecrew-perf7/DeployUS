#include "catch2.hpp"
#include <Common/Common.hpp>
#include "unittestscommon.hpp"
#include <FileSystem/ConfigFile.hpp>
#include "FileSystem/BuildUSCache.hpp"
#include "GCCDriver/GCCDriver.hpp"
#include "BuildUS.hpp"
#include <iostream>

namespace fs = boost::filesystem;

void setupCleanTest()
{
    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);
}

void teardownCleanTest()
{
    std::cout.clear();
    std::cerr.clear();
}

TEST_CASE("CLEAN_SUCCESS")
{
    setupCleanTest();

    ConfigFile* cf;
    GCCDriver* gcc;
    string progname = "app1";
    fs::path configpath;

    //Ensuring there are no intermediate files
    REQUIRE_NOTHROW(fs::remove_all(BUILDUS_CACHE_INTERMEDIATE_FOLDER));
    REQUIRE_NOTHROW(fs::remove_all(progname));

    SECTION("PROG1") {configpath = CONFIG_PROG1_PATH;}
    SECTION("PROG2") {configpath = CONFIG_PROG2_PATH;}

    REQUIRE_NOTHROW(cf = new ConfigFile(configpath));
    REQUIRE_NOTHROW(gcc = new GCCDriver(cf, true));

    //Compile, link
    REQUIRE(gcc->compile() == 0);
    REQUIRE(gcc->link() == 0);
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER));
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE));
    REQUIRE(fs::exists(BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE));
    REQUIRE(fs::exists(progname));

    //Clean! and verify that it was successful
    REQUIRE(BuildUS::clean() == 0);
    REQUIRE(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER));
    REQUIRE(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE));
    REQUIRE(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE));
    REQUIRE(!fs::exists(progname));

    teardownCleanTest();
}