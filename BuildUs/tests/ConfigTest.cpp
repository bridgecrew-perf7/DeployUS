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
    SECTION("CONFIG1") {configpath = CONFIG1_PATH;}
    SECTION("CONFIG2") {configpath = CONFIG2_PATH;}
    SECTION("CONFIG3") {configpath = CONFIG3_PATH;}
    
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

TEST_CASE("Generate_Valid_Config_From_Function")
//Goal of test: see if config files created from a stringstream is the same as if it came from file.
{
    //Setup
    ConfigFile* cfreal;
    ConfigFile* cf;

    //Generate config object
    std::stringstream configcontents = createGeneralConfigProg("prog1");

    //No errors must occur when creating object
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG2_PATH,configcontents));
    
    //Compare with a real config file
    REQUIRE_NOTHROW(cfreal = new ConfigFile(CONFIG2_PATH));
    REQUIRE(cf->getConfigPath() == cfreal->getConfigPath());
    REQUIRE(cf->getProjectName() == cfreal->getProjectName());
    REQUIRE(cf->getCompileList() == cfreal->getCompileList());
    REQUIRE(cf->getDepInclVar() == cfreal->getDepInclVar());
    REQUIRE(cf->getDepLibList() == cfreal->getDepLibList());
    REQUIRE(cf->getDepLibVar() == cfreal->getDepLibVar());

    delete cf;
    delete cfreal;

}

TEST_CASE("Invalid_YAML")
{
   ConfigFile* cf;

   //Invalid ConfigFiles
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG1_PATH)); //No project name
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG2_PATH)); //No compilation unit
}

TEST_CASE("FILES_OR_VARIABLES_DNE")
{
    //Creating valid yaml, but with an env var that does not exists
    string          progfolder = string("prog1");
    string          projectName = string("app1");
    StringPairList  compileList = createStringPairList(2,   string("f1"), 
                                                            string("../") + progfolder + string("/main.cpp"),
                                                            string("f2"),
                                                            string("../") + progfolder + string("/utils.cpp"));
    StringList      depLibList  = StringList();
    string          depInclVar;
    string          depLibVar;

    SECTION("Include_Variable_DNE") 
    {
        depInclVar = string("THIS_ENV_VAR_DOES_NOT_EXISTS");
        depLibVar  = string();
    }
    SECTION("Library_Variable_DNE") 
    {
        depInclVar = string();
        depLibVar  = string("THIS_ENV_VAR_DOES_NOT_EXISTS");
    }
    SECTION("COMPILATION_UNITS_DNE")
    {
        compileList = createStringPairList(2,   string("f1"), 
                                                string("../THISFILE/DNE.cpp"),
                                                string("f2"),
                                                string("../THISFILE/DNE_ASWELL.cpp"));
    }

    std::stringstream yamlFile =  ConfigFileUtils::createValidYAML(     projectName,
                                                                        compileList,
                                                                        depLibVar,
                                                                        depLibList,
                                                                        depInclVar);

    ConfigFile* cf;
    REQUIRE_THROWS(cf = new ConfigFile("config/config1.buildus", yamlFile));
}