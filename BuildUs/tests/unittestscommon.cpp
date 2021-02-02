#include "unittestscommon.hpp"
#include <algorithm>
#include <BuildUS.hpp>
#include <FileSystem/ConfigFile.hpp>

const fs::path CONFIG1_PATH = "config/config1.buildus";
const fs::path CONFIG2_PATH = "config/config2.buildus";
const fs::path CONFIG3_PATH = "config/2projectnames.buildus";
const fs::path CONFIG_FAKE_PATH = "config/nonexistant.buildus";

const fs::path BADCONFIG1_PATH = "config/badconfig1.buildus"; //No compilation units
const fs::path BADCONFIG2_PATH = "config/badconfig2.buildus"; //No project name
const fs::path BADCONFIG3_PATH = "config/badconfig3.buildus"; //Compilation file does not exists



int clearIntermediate()
// Remove intermediae folder
// Returns: 0 for success, non-zero for failure.
{
    int argc = 2;
    const char* argv[argc] = {program_invocation_name, strdup("clean")};
    return BuildUS::start(argc,argv);
}

std::stringstream createConfigStreamForProg(string progfolder)
//Creates common Config file in stringstream format
{
    StringList projectName;
    projectName.push_back(string("app1"));

    StringPairList compileFiles;
    std::pair<string,string> elem1(string("f1"), string("../") + progfolder + string("/main.cpp"));
    std::pair<string,string> elem2(string("f2"), string("../") + progfolder + string("/utils.cpp"));
    compileFiles.push_back(elem1);
    compileFiles.push_back(elem2);

    StringList depLibVars = StringList();
    depLibVars.push_back("BOOST_LIBRARYDIR");

    StringList depLibList = StringList();
    depLibList.push_back("lib1");
    depLibList.push_back("lib2");

    StringList depInclVars = StringList();
    depInclVars.push_back("BOOST_INCLUDEDIR");

    return ConfigFileUtils::createConfigContents(projectName,
                                                compileFiles,
                                                depLibVars,
                                                depLibList,
                                                depInclVars);
}
