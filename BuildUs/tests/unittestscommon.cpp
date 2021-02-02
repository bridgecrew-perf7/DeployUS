#include "unittestscommon.hpp"
#include <algorithm>
#include <BuildUS.hpp>

const fs::path CONFIG1_PATH = "config/config1.buildus";
const fs::path CONFIG2_PATH = "config/config2.buildus";
const fs::path CONFIG3_PATH = "config/2projectnames.buildus";

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
