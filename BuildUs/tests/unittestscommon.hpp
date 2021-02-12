#pragma once
#include <vector>
#include <boost/filesystem.hpp>
#include <sstream>
#include <FileSystem/ConfigFile.hpp>

namespace fs = boost::filesystem;

extern const fs::path CONFIG_LIBS_DNE_PATH;
extern const fs::path CONFIG_LIBS_VAR_DNE_PATH;
extern const fs::path CONFIG_INCL_VAR_DNE_PATH;

extern const fs::path CONFIG_LINKERISSUE1_PATH;
extern const fs::path CONFIG_LINKERISSUE2_PATH;

extern const fs::path CONFIG_COMPILEISSUE1_PATH;
extern const fs::path CONFIG_COMPILEISSUE2_PATH;

extern const fs::path CONFIG_2PROJECTNAMES_PATH;

extern const fs::path CONFIG_FAKE_PATH;

extern const fs::path CONFIG_PROG1_PATH;
extern const fs::path CONFIG_PROG2_PATH;

extern const fs::path BADCONFIG1_PATH; //No compilation units
extern const fs::path BADCONFIG2_PATH; //No project name
extern const fs::path BADCONFIG3_PATH; //Compilation file does not exists


//Methods
int cleanIntermediate();
