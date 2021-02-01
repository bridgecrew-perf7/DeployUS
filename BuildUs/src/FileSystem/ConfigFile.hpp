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
using namespace std;

/* This class models the .buildus file in YAML format*/
class ConfigFile
{
private:
    fs::path filepath;
    YAML::Node config; 

    //YAML config related methods
    void parseYAML();
    bool const isYAMLInvalid();
public:
    //Can throw an error! (ex: If configfile does not exists)
    ConfigFile(const char* filepath);
    ~ConfigFile();

    string const toString();

    //YAML config related methods
    StringList const getProjectName();
    StringList const getCompileList();
    StringList const getDepLibVars();
    StringList const getDepLibList();
    StringList const getDepInclVars();

    //getters
    inline fs::path getConfigPath() {return fs::path(filepath);};

};

StringList const vectorizeYAMLNode(const YAML::Node node);

