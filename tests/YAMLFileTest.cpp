#include "catch2.hpp"
#include "unittestscommon.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <ConfigFile.hpp>

using namespace std;

#define CONFIG1_PATH "config/config1.buildus"
#define CONFIG2_PATH "config/config2.buildus"
#define CONFIG3_PATH "config/2projectnames.buildus"

#define BADCONFIG1_PATH "config/badconfig1.buildus"
#define BADCONFIG2_PATH "config/badconfig2.buildus"


TEST_CASE("Valid_YAML")
{
    ConfigFile* cf;
    vector<string> expectedProjectName;
    expectedProjectName.push_back(string("app1"));
    vector<string> expectedCompileFiles;
    expectedCompileFiles.push_back(string("../prog1/main.cpp"));
    expectedCompileFiles.push_back(string("../prog1/util.cpp"));

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
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG1_PATH));
   REQUIRE_THROWS(cf = new ConfigFile(BADCONFIG2_PATH));

}