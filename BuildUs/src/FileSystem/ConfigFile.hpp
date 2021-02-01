#pragma once
#include <Common/Common.hpp>
#include <yaml-cpp/yaml.h> //https://github.com/jbeder/yaml-cpp.git. MIT licence
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#define CONFIG_FILE_PROJECT "project"
#define CONFIG_FILE_COMPILE "compile"
#define CONFIG_FILE_LIBS "libs"
#define CONFIG_FILE_VARS "var"
#define CONFIG_FILE_DEP_LIBRARY "deps_library"
#define CONFIG_FILE_DEP_INCL "deps_include"

namespace fs = boost::filesystem;

/* This class models the .buildus file in YAML format*/
class ConfigFile
{
private:
    fs::path configPath;
    StringList      projectName;
    StringPairList  compileList;
    StringList      depLibVars;
    StringList      depLibList;
    StringList      depInclVars;

    //YAML config related methods and attributes
    YAML::Node config; 
    void parseYAML();

    void verifyCompilationUnitsExists();
    bool const isYAMLInvalid();
public:
    //Can throw an error! (ex: If configfile does not exists)
    ConfigFile(fs::path filepath);
    static ConfigFile* safeFactory(fs::path filepath);
    ~ConfigFile();

    string const toString();

    //YAML config related methods
    StringList      const getProjectName() {return this->projectName;};
    StringPairList  const getCompileList() {return this->compileList;};
    StringList      const getDepLibVars() {return this->depLibVars;};
    StringList      const getDepLibList() {return this->depLibList;};
    StringList      const getDepInclVars() {return this->depInclVars;};

    //getters
    fs::path getConfigPath() {return fs::path(this->configPath);};

};

namespace ConfigFileHelper
{
    StringList      const vectorizeYAMLNode(const YAML::Node node);
    StringPairList  const generateCompileList(const YAML::Node node);
}

