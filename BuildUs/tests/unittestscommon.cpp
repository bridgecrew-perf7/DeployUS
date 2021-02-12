#include "unittestscommon.hpp"
#include <algorithm>
#include <BuildUS.hpp>
#include <FileSystem/ConfigFile.hpp>

const fs::path CONFIG_LIBS_DNE_PATH = "config/config_libsDNE.buildus";
const fs::path CONFIG_LIBS_VAR_DNE_PATH = "config/config_libsVarDNE.buildus";
const fs::path CONFIG_INCL_VAR_DNE_PATH = "config/config_InclVarDNE.buildus";

const fs::path CONFIG_LINKERISSUE1_PATH = "config/config_linkerissue1.buildus";
const fs::path CONFIG_LINKERISSUE2_PATH = "config/config_linkerissue2.buildus";

const fs::path CONFIG_COMPILEISSUE1_PATH = "config/config_compileissue1.buildus";
const fs::path CONFIG_COMPILEISSUE2_PATH = "config/config_compileissue2.buildus";

const fs::path CONFIG_2PROJECTNAMES_PATH = "config/2projectnames.buildus";
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

