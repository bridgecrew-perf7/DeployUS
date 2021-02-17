#include "ConfigFile.hpp"
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

ConfigFile::ConfigFile()
{ 
}

ConfigFile::ConfigFile(fs::path filepath, std::stringstream& configContents)
{
    this->configPath = filepath;

    //1. Parsing. Sets config attribute
    this->parseYAML(configContents);    
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::parseYAML(std::stringstream& bytestream)
/*
    Fills attribut fields with contents of config file.
*/
{
    //Simple lambda function to extract the first string of StrintPairList
    auto extractFirstString = [&](StringList list) {return list.size() > 0 ? list.at(0) : string();};
    
    //Saving YAML config
    this->config = YAML::Load(bytestream);

    //Extracting single element attributes
    StringList temp;
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[ConfigFileUtils::CONFIG_FILE_PROJECT]);
    this->projectName = extractFirstString(temp) ; // Only 1 name
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[ConfigFileUtils::CONFIG_FILE_DEP_LIBRARY][ConfigFileUtils::CONFIG_FILE_VARS]);
    this->depLibVar   =  extractFirstString(temp); // Only 1 var
    temp = ConfigFileUtils::vectorizeYAMLNode(this->config[ConfigFileUtils::CONFIG_FILE_DEP_INCL][ConfigFileUtils::CONFIG_FILE_VARS]);
    this->depInclVar  = extractFirstString(temp);  // Only 1 var
    
    //Multiple element attributs
    this->compileList = ConfigFileUtils::generateCompileList(this->config[ConfigFileUtils::CONFIG_FILE_COMPILE]);
    this->depLibList  =  ConfigFileUtils::vectorizeYAMLNode(this->config[ConfigFileUtils::CONFIG_FILE_DEP_LIBRARY][ConfigFileUtils::CONFIG_FILE_LIBS]);
}

int const ConfigFile::verifyCompilationUnitsExists(string& err)
// Checks to see if all filepaths from compile lists exists.
// Returns 0 if every compilation unit exists, non-zero otherwise
{
    bool allExists = true;
    for(auto elem: this->getCompileList())
    {
        fs::path compileunitPath = this->getConfigParentPath().append(elem.second);
        if(!fs::exists(compileunitPath))
        {
            err += string("Error: Compilation unit ") + elem.second + string(" does not exists.\n");
            allExists = false;
        }
    }

    return allExists? 0 : 1;
}

int const ConfigFile::isConfigValid(string& err)
/*
    Returns 0 if the Config file is invalid.
    Returns non-zero if it is a valid Config file
*/
{
    int isValid = 0;

    //Only allowed 1 project name
    if(this->getProjectName().empty())
    {
        err += "Error: Must have a project name. \n";
        isValid = 1;
    }

    //Must compile at least 1 file
    if(this->getCompileList().size() < 1)
    {
        err += "Error: Must have at least one file to compile.\n";
        isValid = 1;
    }
    else if(this->verifyCompilationUnitsExists(err) != 0)
    {
        isValid = 1;
    }

    //All environment variables must exists
    if(!this->getDepInclVar().empty())
    {
        if(getenv(this->getDepInclVar().c_str()) == NULL)
        {
            err += "Error: Include variable is not an environment variable.\n";
            isValid = 1;
        }
    }
    if(!this->getDepLibVar().empty())
    {
        if(getenv(this->getDepLibVar().c_str()) == NULL)
        {
            err += "Error: Library variable is not an environment variable.\n";
            isValid = 1;
        }
    }
    return isValid;
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