#include "catch2.hpp"
#include "unittestscommon.hpp"
#include "boost/filesystem.hpp"
#include <Common/Common.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <ConfigFile.hpp>
#include <iostream>

namespace fs = boost::filesystem;

TEST_CASE("Valid_YAML")
{
    ConfigFile* cf;
    string expectedProjectName = "app1";
    StringPairList expectedCompileFiles;
    std::pair<string,string> elem1(string("f1"), string("../prog1/main.cpp"));
    std::pair<string,string> elem2(string("f2"), string("../prog1/utils.cpp"));
    expectedCompileFiles.push_back(elem1);
    expectedCompileFiles.push_back(elem2);

    //Valid ConfigFiles
    fs::path configpath;
    SECTION("CONFIG1") {configpath = CONFIG_LIBS_DNE_PATH;}
    SECTION("CONFIG2") {configpath = CONFIG_PROG1_PATH;}
    SECTION("CONFIG3") {configpath = CONFIG_2PROJECTNAMES_PATH;}
    
    //Testing config
    REQUIRE_NOTHROW(cf = new ConfigFile(configpath));
    REQUIRE(cf->getProjectName() == expectedProjectName);
    REQUIRE(cf->getCompileList() == expectedCompileFiles);
    delete cf;

}

TEST_CASE("VALID_PROGRAM")
{
    ConfigFile* cf;
    string expectedProjectName;
    StringPairList expectedCompileFiles;
    fs::path configpath;

    SECTION("PROG1")
    {
        expectedProjectName = "app1";
        std::pair<string,string> elem1(string("f1"), string("../prog1/main.cpp"));
        std::pair<string,string> elem2(string("f2"), string("../prog1/utils.cpp"));
        expectedCompileFiles.push_back(elem1);
        expectedCompileFiles.push_back(elem2);
        configpath = CONFIG_PROG1_PATH;

    }
    SECTION("PROG2")
    {
        expectedProjectName = "app1";
        std::pair<string,string> elem1(string("f1"), string("../prog2/main.cpp"));
        std::pair<string,string> elem2(string("f2"), string("../prog2/utils.cpp"));
        expectedCompileFiles.push_back(elem1);
        expectedCompileFiles.push_back(elem2);
        configpath = CONFIG_PROG2_PATH;
    }

    //Testing config
    REQUIRE_NOTHROW(cf = new ConfigFile(configpath));
    REQUIRE(cf->getProjectName() == expectedProjectName);
    REQUIRE(cf->getCompileList() == expectedCompileFiles);
    delete cf;
}

TEST_CASE("Invalid_YAML")
{
   ConfigFile* cf;

   //Invalid ConfigFiles
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG1_PATH)); //No compilation unit
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG2_PATH)); //No project name
}

TEST_CASE("FILES_OR_VARIABLES_DNE")
{
    fs::path configpath;
    SECTION("Include_Variable_DNE") 
    {
        configpath = CONFIG_INCL_VAR_DNE_PATH;
    }
    SECTION("Library_Variable_DNE") 
    {
        configpath = CONFIG_LIBS_VAR_DNE_PATH;
    }
    SECTION("COMPILATION_UNITS_DNE")
    {
        configpath = BADCONFIG3_PATH;
    }

    ConfigFile* cf;
    REQUIRE_THROWS(cf = new ConfigFile(configpath));
}