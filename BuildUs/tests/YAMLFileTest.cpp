#include "catch2.hpp"
#include "unittestscommon.hpp"
#include "boost/filesystem.hpp"
#include <Common/Common.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <ConfigFile.hpp>

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