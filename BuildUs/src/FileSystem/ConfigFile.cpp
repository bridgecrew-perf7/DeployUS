#include "ConfigFile.hpp"
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

ConfigFile::ConfigFile(fs::path filepath)
//Can throw std::runtime_error! 
{
    this->configPath = filepath;

    //1. Verify that file exists
    auto configfilepath = fs::path(filepath);
    if(!fs::exists(configfilepath))
    {
        string msg = string("Error: File ") + filepath.string() + string(" does not exists.");
        throw std::runtime_error(msg);
    }

    //2. Read files
    std::stringstream configcontents = readFile(configfilepath);

    //3. Initialize members
    initialize(configcontents);
}

ConfigFile::ConfigFile(fs::path simulatedConfigPath, std::stringstream& bytestream)
//Create Config from bytestream. Usefull for unittesting
{
    this->configPath = simulatedConfigPath;
    initialize(bytestream);
}

void ConfigFile::initialize(std::stringstream& bytestream)
//Common code to all constructors
{
    //1. Parsing. Sets config attribute
    this->parseYAML(bytestream);

    //2. Verify that config is valid
    string err;
    if(this->isConfigInvalid(err))
    {
        throw std::runtime_error(err);
    }

    //3. Verify that all compilation units exists
    //   Note: This function can throw std::runtime_error
    this->verifyCompilationUnitsExists();
}

ConfigFile* ConfigFile::safeFactory(fs::path filepath)
//Catches all errors. Returns nulltpr if an error occured
{
    ConfigFile* out;
    try
    {
        out = new ConfigFile(filepath);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        out = nullptr;
    }
    return out;
}

static ConfigFile* safeFactory(fs::path simulatedConfigPath, std::stringstream& bytestream)
//Catches all error. Returns nullptr if an error occured
{
    ConfigFile* out;
    try
    {
        out = new ConfigFile(simulatedConfigPath, bytestream);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        out = nullptr;
    }
    return out;
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::parseYAML(std::stringstream& bytestream)
/*
    Fills attribut fields with contents of config file.
*/
{
    auto extractFirstString = [&](StringList list) {return list.size() > 0 ? list.at(0) : string();};
    //Saving YAML config
    this->config = YAML::Load(bytestream);

    //Filling attributes
    StringList temp;
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[CONFIG_FILE_PROJECT]);
    this->projectName = extractFirstString(temp) ; // Only 1 name
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_LIBRARY][CONFIG_FILE_VARS]);
    this->depLibVar   =  extractFirstString(temp); // Only 1 var
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_INCL][CONFIG_FILE_VARS]);
    this->depInclVar  = extractFirstString(temp);  // Only 1 var
    
    this->compileList = ConfigFileUtils::generateCompileList(this->config[CONFIG_FILE_COMPILE]);
    this->depLibList  =  ConfigFileUtils::vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_LIBRARY][CONFIG_FILE_LIBS]);
}

void ConfigFile::verifyCompilationUnitsExists()
// Checks to see if all filepaths from compile lists exists.
// Throws runtime error if it is the case.
{
    for(auto elem: this->getCompileList())
    {
        fs::path compileunitPath = this->getConfigParentPath().append(elem.second);
        if(!fs::exists(compileunitPath))
        {
            string msg = string("Error: Compilation unit ") + elem.second + string(" does not exists.");
            throw std::runtime_error(msg);
        }
    }
}

bool const ConfigFile::isConfigInvalid(string& err)
/*
    Returns true if the Config file is invalid.
    Returns false if it is a valid Config file
*/
{
    //Only allowed 1 project name
    if(this->getProjectName().empty())
    {
        err = "Error: Must have a project name.";
        return true;
    }

    //Must compile at least 1 file
    if(this->getCompileList().size() < 1)
    {
        err = "Error: Must have at least one file to compile.";
        return true;
    }

    //All environment variables must exists
    if(!this->getDepInclVar().empty())
    {
        if(getenv(this->getDepInclVar().c_str()) == NULL)
        {
            err = "Error: Include variable is not an environment variable.";
            return true;
        }
    }
    if(!this->getDepLibVar().empty())
    {
        if(getenv(this->getDepLibVar().c_str()) == NULL)
        {
            err = "Error: Library variable is not an environment variable.";
            return true;
        }
    }

    return false;
}


string const ConfigFile::toString()
// Returns string representation of fields.
// Mainly a debugging function, in order to visualize the fields of the ConfigFile class.
// Should not be used during normal executable operation.
{
    YAML::Emitter emitter;
    emitter << this->config;
    return string(emitter.c_str());
}

/*
    ====================================
    Helper functions definitions
    ====================================
*/

StringList const ConfigFileUtils::vectorizeYAMLNode(const YAML::Node node)
//Returns vector of string contained in Node. Must be the last containing node.
{
    StringList vectorized;
    switch (node.Type())
    {
        case YAML::NodeType::Scalar : 
            vectorized.push_back(node.as<string>());
            break;
        case YAML::NodeType::Sequence:
            for(int i = 0; i < node.size(); i++ )
                vectorized.push_back(node[i].as<string>());
            break;
        case YAML::NodeType::Map:
            //Special case. We now have two strings.
            //The map key and its value.
            //We will concatenate the two with a \0 character
            {
                std::map<string,string> nodemap = node.as<std::map<string,string>>();
                for(auto nodeItem: nodemap)
                {
                    string key   = nodeItem.first;
                    string value = nodeItem.second;
                    string concatenated = key + CONFIG_MAP_SEPERATOR + value;
                    vectorized.push_back(concatenated);
                }
            }
            break;
        default:
            break;
    }
    return vectorized;
}

StringPairList const ConfigFileUtils::generateCompileList(const YAML::Node node)
//Creates list of compilation units
{
    StringPairList toCompile;
    for(int i = 0; i < node.size(); i++ )
    {
        for(auto elem: ConfigFileUtils::vectorizeYAMLNode(node[i]))
        {
            int nullPos = elem.find(CONFIG_MAP_SEPERATOR);
            string outputPath = elem.substr(0,nullPos);
            string inputPath = elem.substr(nullPos+1);
            std::pair<string, string> compileUnit(outputPath,inputPath);
            toCompile.push_back(compileUnit);
        }
    }
    return toCompile;
}

/* Creates the content of a valid YAML config file. Useful for unit testing*/
std::stringstream ConfigFileUtils::createValidYAML( string          projectName,
                                                    StringPairList  compileList,
                                                    string          depLibVar,
                                                    StringList      depLibList,
                                                    string          depInclVar)
{
    std::stringstream out;
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    //1. Project
    emitter << YAML::Key << CONFIG_FILE_PROJECT;
    emitter << YAML::Value << projectName;
    
    //2. Compile List
    emitter << YAML::Key << CONFIG_FILE_COMPILE;
    emitter << YAML::Value;
    emitter << YAML::BeginSeq;
    for(auto compileUnit: compileList)
    {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << compileUnit.first;
        emitter << YAML::Value << compileUnit.second;
        emitter << YAML::EndMap;
    } 
    emitter << YAML::EndSeq;
    

    if(depLibList.size() > 0 || !depLibVar.empty())
    {
        emitter << YAML::Key << CONFIG_FILE_DEP_LIBRARY;
        emitter << YAML::Value;
        emitter << YAML::BeginMap;
        //3. Dependencies vars List
        if(!depLibVar.empty())
        {
            
            emitter << YAML::Key << CONFIG_FILE_VARS;
            emitter << YAML::Value << depLibVar;
        }

        //4. Dependencies libraries List
        if(depLibList.size() > 0)
        {   
            emitter << YAML::Key << CONFIG_FILE_LIBS;
            emitter << YAML::Value;
            emitter << YAML::BeginSeq;
            for(auto lib: depLibList)
            {
                emitter << lib;
            }
            emitter << YAML::EndSeq;
        }
        emitter << YAML::EndMap;
    }

    //5. Includes List
    if(!depInclVar.empty())
    {
        emitter << YAML::Key << CONFIG_FILE_DEP_INCL;
        emitter << YAML::Value;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << CONFIG_FILE_VARS;
        emitter << YAML::Value << depInclVar;
        emitter << YAML::EndMap;
    } 

    emitter << YAML::EndMap;
    out << emitter.c_str();
    return out;
}