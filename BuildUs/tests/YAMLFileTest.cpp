#define CATCH_CONFIG_MAIN

#include "catch2.hpp"
#include "unittestscommon.hpp"
#include "boost/filesystem.hpp"
#include <Common/Common.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <ConfigFile.hpp>

namespace fs = boost::filesystem;

const fs::path CONFIG1_PATH = "config/config1.buildus";
const fs::path CONFIG2_PATH = "config/config2.buildus";
const fs::path CONFIG3_PATH = "config/2projectnames.buildus";

const fs::path BADCONFIG1_PATH = "config/badconfig1.buildus"; //No compilation units
const fs::path BADCONFIG2_PATH = "config/badconfig2.buildus"; //No project name
const fs::path BADCONFIG3_PATH = "config/badconfig3.buildus"; //Compilation file does not exists


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
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG1_PATH));
    REQUIRE(vectorcompare(cf->getProjectName(), expectedProjectName));
    REQUIRE(vectorcompare(cf->getCompileList(), expectedCompileFiles));
    delete cf;
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG2_PATH));
    REQUIRE(vectorcompare(cf->getProjectName(), expectedProjectName));
    REQUIRE(vectorcompare(cf->getCompileList(), expectedCompileFiles));
    delete cf;
    REQUIRE_NOTHROW(cf = new ConfigFile(CONFIG3_PATH));
    REQUIRE(vectorcompare(cf->getProjectName(), expectedProjectName));
    REQUIRE(vectorcompare(cf->getCompileList(), expectedCompileFiles));
    delete cf;

}


TEST_CASE("Invalid_YAML")
{
   ConfigFile* cf;

   //Invalid ConfigFiles
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG1_PATH)); //No project name
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG2_PATH)); //No compilation unit
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG3_PATH)); //Compilation unit does not exists
}

TEST_CASE("COMPILATION")
{
    REQUIRE(1==1);
}