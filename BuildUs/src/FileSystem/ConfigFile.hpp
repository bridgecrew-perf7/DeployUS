#pragma once
#include <Common/Common.hpp>
#include <yaml-cpp/yaml.h> //https://github.com/jbeder/yaml-cpp.git. MIT licence
#include <boost/filesystem.hpp>


const string CONFIG_FILE_PROJECT = "project";
const string CONFIG_FILE_COMPILE = "compile";
const string CONFIG_FILE_LIBS = "libs";
const string CONFIG_FILE_VARS = "var";
const string CONFIG_FILE_DEP_LIBRARY = "deps_library";
const string CONFIG_FILE_DEP_INCL = "deps_include";

namespace fs = boost::filesystem;

/* This class models the .buildus file in YAML format*/
class ConfigFile
{
private:
    fs::path        configPath;
    string          projectName;
    StringPairList  compileList;
    string          depLibVar;
    StringList      depLibList;
    string          depInclVar;

    //YAML config related methods and attributes
    YAML::Node config; 
    void parseYAML(std::stringstream& bytestream);

    void verifyCompilationUnitsExists();
    bool const isConfigInvalid(string& err);
public:
    //Can throw an error! (ex: If configfile does not exists)
    ConfigFile(fs::path filepath);

    //Will not throw an error
    static ConfigFile* safeFactory(fs::path filepath);
    ~ConfigFile();

    //To string function
    string const toString();

    //getters
    string          const getProjectName() {return this->projectName;};
    StringPairList  const getCompileList() {return this->compileList;};
    string          const getDepLibVar()   {return this->depLibVar;};
    StringList      const getDepLibList()  {return this->depLibList;};
    string          const getDepInclVar()  {return this->depInclVar;};
    fs::path getConfigPath() {return fs::path(this->configPath);};
    fs::path getConfigParentPath() {return fs::path(this->configPath.parent_path());};

};

namespace ConfigFileUtils
{
    const char CONFIG_MAP_SEPERATOR = '\0';
    StringList      const vectorizeYAMLNode(const YAML::Node node);
    StringPairList  const generateCompileList(const YAML::Node node);
}

