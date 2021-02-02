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
    StringList expectedProjectName;
    expectedProjectName.push_back(string("app1"));
    StringPairList expectedCompileFiles;
    std::pair<string,string> elem1(string("f1"), string("../prog1/main.cpp"));
    std::pair<string,string> elem2(string("f2"), string("../prog1/utils.cpp"));
    expectedCompileFiles.push_back(elem1);
    expectedCompileFiles.push_back(elem2);

    //Valid ConfigFiles
    fs::path configpath;
    SECTION("CONFIG1") {configpath = CONFIG1_PATH;}
    SECTION("CONFIG2") {configpath = CONFIG2_PATH;}
    SECTION("CONFIG3") {configpath = CONFIG3_PATH;}
    
    //Testing config
    REQUIRE_NOTHROW(cf = new ConfigFile(configpath));
    REQUIRE(vectorcompare(cf->getProjectName(), expectedProjectName));
    REQUIRE(vectorcompare(cf->getCompileList(), expectedCompileFiles));
    delete cf;

}

TEST_CASE("Generate_Valid_Config_From_Function")
//Goal of test: see if config files created from a stringstream is the same as if it came from file.
{
    //Setup
    ConfigFile* cfreal;
    ConfigFile* cf;

    //Generate config object
    std::stringstream configcontents = createConfigStreamForProg("prog1");

    //No errors must occur when creating object
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG2_PATH,configcontents));
    
    //Compare with a real config file
    REQUIRE_NOTHROW(cfreal = new ConfigFile(CONFIG2_PATH));
    REQUIRE(cf->getConfigPath() == cfreal->getConfigPath());
    REQUIRE(cf->getProjectName() == cfreal->getProjectName());
    REQUIRE(cf->getCompileList() == cfreal->getCompileList());
    REQUIRE(cf->getDepInclVars() == cfreal->getDepInclVars());
    REQUIRE(cf->getDepLibList() == cfreal->getDepLibList());
    REQUIRE(cf->getDepLibVars() == cfreal->getDepLibVars());

    delete cf;
    delete cfreal;

}



TEST_CASE("Invalid_YAML")
{
   ConfigFile* cf;

   //Invalid ConfigFiles
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG1_PATH)); //No project name
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG2_PATH)); //No compilation unit
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG3_PATH)); //Compilation unit does not exists
}