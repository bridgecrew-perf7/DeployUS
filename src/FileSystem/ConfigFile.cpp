#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const char* filepath)
{
    this->filepath = filepath;

    //1. Verify that file exists
    auto configfilepath = fs::path(filepath);
    if(!fs::exists(configfilepath))
    {
        throw std::runtime_error("Error: Configuration file does not exists.");
    }

    //2. Parsing. Sets config attribute
    parseYAML();

    //3. Verify that config is valid
    if(this->isYAMLInvalid())
    {
        throw std::runtime_error("Error: Configuration file is not valid.");
    }


}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::parseYAML()
/*
    Fills attribut fields with contents of config file.
*/
{
   this->config = YAML::LoadFile(this->filepath.c_str());
}

bool const ConfigFile::isYAMLInvalid()
/*
    Returns true if the YAML file is invalid.
    Returns false if it is a valid YAML file
*/
{
    bool ret = false;

    //Only allowed 1 project name
    if(this->getProjectName().size() != 1)
    {
        ret = true;
    }

    //Must compile at least 1 file
    if(this->getCompileList().size() < 1)
    {
        ret = true;
    }

    return ret;
}

vector<string> const ConfigFile::getProjectName()
/*
    Returns project name.
*/
{
    return vectorizeYAMLNode(this->config[CONFIG_FILE_PROJECT]);
}

vector<string> const ConfigFile::getCompileList()
/*
    Returns list of filepaths to compile.
*/
{
    vector<string> toCompile;
    char item[5];
    for(int i = 0; i < this->config[CONFIG_FILE_COMPILE].size(); i++ )
    {
        sprintf(item,"f%d",i + 1);
        for(auto elem: vectorizeYAMLNode(this->config[CONFIG_FILE_COMPILE][i][item]))
        {
            toCompile.push_back(elem);
        }
    }
    return toCompile;
}

vector<string> const ConfigFile::getDepLibVars()
/* 
    Returns vector of dependencies from environment variables
*/
{
    return vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_LIBRARY][CONFIG_FILE_VARS]);
}

vector<string> const ConfigFile::getDepLibList()
/*
    Returns list of libraries to link.
*/
{
    return vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_LIBRARY][CONFIG_FILE_LIBS]);
}
vector<string> const ConfigFile::getDepInclVars()
/* 
    Returns vector of dependencies from environment variables
*/
{
    return vectorizeYAMLNode(this->config[CONFIG_FILE_DEP_INCL][CONFIG_FILE_VARS]);
}

string const ConfigFile::toString()
// Returns string representation of fields
{
    stringstream out;
    
    //1. Project
    out << CONFIG_FILE_PROJECT << ": " << this->getProjectName()[0] << "\n";
    
    //2. Compile List
    out << CONFIG_FILE_COMPILE << ":\n";
    for(auto filepath: this->getCompileList())
    {
        out << " " << filepath << '\n';
    }

    if(this->getDepLibList().size() > 0 || this->getDepLibVars().size() > 0)
    {
        out << CONFIG_FILE_DEP_LIBRARY << ": \n";

        //3. Dependencies vars List
        if(this->getDepLibVars().size() > 0)
        {
            out << " " << CONFIG_FILE_VARS << ":\n";
            for(auto var: this->getDepLibVars())
            {
                out << "  " << var << '\n';
            }
        }

        //4. Dependencies libraries List
        if(this->getDepLibList().size() > 0)
        {   out << " " << CONFIG_FILE_LIBS << ":\n";
            for(auto libpath: this->getDepLibList())
            {
                out << "  " << libpath << '\n';
            }
        }
    }


    //5. Includes List
    if(this->getDepInclVars().size() > 0)
    {
        out << CONFIG_FILE_DEP_INCL << ": \n";
        out << " " << CONFIG_FILE_VARS << ":\n";
        for(auto var: this->getDepInclVars())
        {
            out << "  " << var << '\n';
        }
    }
    
    return out.str();
}

/*
    ====================================
    Global functions definitions
    ====================================
*/

vector<string> const vectorizeYAMLNode(const YAML::Node node)
//Returns vector of string contained in Node. Must be the last containing node.
{
    vector<string> vectorized;
    switch (node.Type())
    {
        case YAML::NodeType::Scalar : 
            vectorized.push_back(node.as<string>());
            break;
        case YAML::NodeType::Sequence:
            for(int i = 0; i < node.size(); i++ )
                vectorized.push_back(node[i].as<string>());
            break;
        default:
            break;
    }
    return vectorized;
}
