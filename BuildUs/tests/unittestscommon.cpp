#include "unittestscommon.hpp"
#include <algorithm>
#include <BuildUS.hpp>
#include <FileSystem/ConfigFile.hpp>

const fs::path CONFIG1_PATH = "config/config1.buildus";
const fs::path CONFIG2_PATH = "config/config2.buildus";
const fs::path CONFIG3_PATH = "config/2projectnames.buildus";
const fs::path CONFIG_FAKE_PATH = "config/nonexistant.buildus";

const fs::path CONFIG_PROG1_PATH = "config/configprog1.buildus";
const fs::path CONFIG_PROG2_PATH = "config/configprog2.buildus";

const fs::path BADCONFIG1_PATH = "config/badconfig1.buildus"; //No compilation units
const fs::path BADCONFIG2_PATH = "config/badconfig2.buildus"; //No project name
const fs::path BADCONFIG3_PATH = "config/badconfig3.buildus"; //Compilation file does not exists



int cleanIntermediate()
// Remove intermediae folder
// Returns: 0 for success, non-zero for failure.
{
    int argc = 2;
    const char* argv[argc] = {program_invocation_name, strdup("clean")};
    return BuildUS::start(argc,argv);
}

std::stringstream createGeneralConfigProg(string progfolder)
//Creates common Config file in stringstream format
{
    string          projectName = string("app1");
    StringPairList  compileList = createStringPairList(2,   string("f1"), 
                                                            string("../") + progfolder + string("/main.cpp"),
                                                            string("f2"),
                                                            string("../") + progfolder + string("/utils.cpp"));
    string          depLibVars  = string("BOOST_LIBRARYDIR");
    StringList      depLibList  = createStringList(2, string("lib1"), string("lib2"));
    string          depInclVars = string("BOOST_INCLUDEDIR");
    return ConfigFileUtils::createValidYAML(    projectName,
                                                compileList,
                                                depLibVars,
                                                depLibList,
                                                depInclVars);
}
