#pragma once
#include <vector>
#include <boost/filesystem.hpp>
#include <sstream>
#include <FileSystem/ConfigFile.hpp>

namespace fs = boost::filesystem;

extern const fs::path CONFIG1_PATH;
extern const fs::path CONFIG2_PATH;
extern const fs::path CONFIG3_PATH;
extern const fs::path CONFIG_FAKE_PATH;

extern const fs::path CONFIG_PROG1_PATH;
extern const fs::path CONFIG_PROG2_PATH;

extern const fs::path BADCONFIG1_PATH; //No compilation units
extern const fs::path BADCONFIG2_PATH; //No project name
extern const fs::path BADCONFIG3_PATH; //Compilation file does not exists


//Methods
int cleanIntermediate();


/* Config Creation */
std::stringstream createGeneralConfigProg(string progfolder);
