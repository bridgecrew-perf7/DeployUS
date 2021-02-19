#include "BuildUSCache.hpp"
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <Common/Common.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>



BuildUSCache::BuildUSCache()
{
}

BuildUSCache::BuildUSCache(ConfigFile& configFile)
{
    this->config = configFile;
    this->cached = ThreeStringTupleList();

    if(fs::exists(BuildUSCacheUtils::INTERMEDIATE_FOLDER))
    {
        //Read cached files
        this->readCompileCacheOnDisk();
    }
    else //Intermediate folder does not exists.
    {
        fs::create_directory(BuildUSCacheUtils::INTERMEDIATE_FOLDER);
    }
}

BuildUSCache::~BuildUSCache()
{
}

int BuildUSCache::readCompileCacheOnDisk()
/* 
    Reads and parses the cache file, if it exists.
    Returns non-zero if an error occured. Zero otherwise.
*/
{
    //Verify the file's existence
    if(!fs::exists(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE))
        return 0;

    //1. Build Representation of cache (Parsing)
    std::stringstream cachecontents;
    if(readFile(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE,cachecontents))
    {
        return 1;
    }

    while(!cachecontents.eof())
    {
        //Read in new line
        string outputName = BuildUSCacheUtils::getCacheToken(cachecontents);
        string filepath   = BuildUSCacheUtils::getCacheToken(cachecontents);
        string fileSHA1   = BuildUSCacheUtils::getCacheToken(cachecontents);
        
        //Fill in cache map
        ThreeStringTuple tpl = ThreeStringTuple(outputName,filepath,fileSHA1);
        this->cached.push_back(tpl);

    }

    return 0;
}

int BuildUSCache::writeCompileCacheToDisk()
/*
    Write the cache map to disk.
    Returns non-zero if an error occures, 0 otherwise.
*/
{
    std::stringstream cachecontents;

    //Formating: <outputfile>\0<filepath>\0<SHA1>\n
    for(auto compiledFileDesc: this->cached)
    {
        cachecontents << ThreeStringTupleUtils::getOutputFileName(compiledFileDesc);  //output file
        cachecontents << BuildUSCacheUtils::INTRA_SEP;
        cachecontents << ThreeStringTupleUtils::getSourceFilePath(compiledFileDesc);  //source file path
        cachecontents << BuildUSCacheUtils::INTRA_SEP;
        cachecontents << ThreeStringTupleUtils::getSourceSHA1(compiledFileDesc);      //hash
        cachecontents << BuildUSCacheUtils::INTER_SEP;
    }

    //Flush to disk
    if(writeFile(BuildUSCacheUtils::INTERMEDIATE_COMPILE_CACHE, cachecontents.str()))
    {
        std::cout << "Unable to write .cache file." << std::endl;
        return 1;
    }

    return 0;
}

int const BuildUSCache::mustLink()
//Returns 1 if the project.cache will change.
//  The file will change if a file was compile, if the project name has changed or if the project.file does not exists.
//Returns 0 otherwise
{
    //Load project.cache
    if(fs::exists(BuildUSCacheUtils::INTERMEDIATE_PROJECT_CACHE))
    {
        std::stringstream projectCacheContents;
        if(readFile(BuildUSCacheUtils::INTERMEDIATE_PROJECT_CACHE,projectCacheContents))
        {
            return 1;
        }

        //Get executablePath and SHA1 from disk
        string diskExecutableRelativePath = BuildUSCacheUtils::getCacheToken(projectCacheContents);

        //Get executablePath and SHA1 of config
        string configExecutableRelativePath = this->getExecutablePath().string();


        bool pathHasChanged = diskExecutableRelativePath.compare(configExecutableRelativePath) != 0;
        bool somethingCompiled = this->filesCompiled.size() > 0; 
        bool executableDeleted = !fs::exists(configExecutableRelativePath);
        return (executableDeleted || pathHasChanged || somethingCompiled) ? 1 : 0;
    }

    return 1;
}

void const BuildUSCache::writeProjectCacheToDisk()
//Fill project.cache file
{
    std::stringstream out;
    out << this->getExecutablePath().string();
    writeFile(BuildUSCacheUtils::INTERMEDIATE_PROJECT_CACHE, out.str());
}

int const BuildUSCache::getFileForMinimalCompilation(const StringPairList& filesForCompilation, StringPairList& filesToCompile)
/*
    Argument: List of entries in Config File

    Looks to see if files to compile 
        1. have been modified
        2. have never been compiled. 

    Sets argument filesToCompile
    Returns non-zero if an error occured. Zero otherwise.
*/
{
    for(auto compileUnit: filesForCompilation)
    {
        bool needCompilation = true;
        string compileUnitOutputPath = compileUnit.first + COMPILE_OBJECT_EXT;
        string compileUnitFilePath = compileUnit.second;

        std::stringstream compileUnitContents;
        fs::path compileUnitFilePathRelativeToConfig = this->config.getConfigParentPath().append(compileUnitFilePath);
        if(readFile(compileUnitFilePathRelativeToConfig, compileUnitContents))
        {
            return 1;
        }
        string compileUnitSHA1 = generateSHA1(compileUnitContents.str());

        //Compare with every file in cache until hit is found
        for(auto fileInCache : this->cached)
        {
            string outputFilePath = ThreeStringTupleUtils::getOutputFileName(fileInCache);
            string sourceFilePath = ThreeStringTupleUtils::getSourceFilePath(fileInCache);
            string sourceSHA1     = ThreeStringTupleUtils::getSourceSHA1(fileInCache);


            //Determine if compilation necessary
            if(     compileUnitOutputPath == outputFilePath     //Same output path
                &&  compileUnitFilePath   == sourceFilePath     //Same filename
                &&  compileUnitSHA1       == sourceSHA1  )      //Same SHA1 (file has not changed)
            {
                needCompilation = false;
                break;
            }

        }

        //Add the file to compilation list
        if(needCompilation)
            filesToCompile.push_back(compileUnit);
    }

    return 0;
}

int BuildUSCache::updateCompiled(const StringPairList filesCompiled)
/*
    Update cached attribut with newly compiled files.
    Each file has its SHA1 computed in order to check if they have changed

    Returns zero if success, non-zero if unsucessful.
*/
{
    // Saving of files that were compiled.
    this->filesCompiled = filesCompiled;

    for(auto compileUnit: filesCompiled)
    {
        string filepathstr = compileUnit.second;
        std::stringstream filecontents;
        if(readFile(this->config.getConfigParentPath().append(filepathstr), filecontents))
        {
            return 1;
        }

        //1. Compute SHA1 of file contents to obtain unique id of file version.
        string fileSHA1 = generateSHA1(filecontents.str());

        //2. Add file path and its has to the cache vector
        ThreeStringTuple cacheEntry = ThreeStringTuple( compileUnit.first + COMPILE_OBJECT_EXT, //Adding extension to cache
                                                        compileUnit.second,
                                                        fileSHA1
                                                    );

        this->cached.push_back(cacheEntry);
    }

    //3. Flush cache to disk
    return this->writeCompileCacheToDisk();
}

const fs::path BuildUSCache::getExecutablePath()
{
    fs::path execPath = BuildUSCacheUtils::INTERMEDIATE_FOLDER.parent_path().append(this->config.getProjectName());
    return execPath;
}


/*
    ==============================
            HELPER FUNCTIONS
    ==============================
*/
string BuildUSCacheUtils::getCacheToken(std::stringstream& bytestream)
//Returns next token of .cache file.
{
    string token;
    while(          bytestream.peek() != BuildUSCacheUtils::INTER_SEP
              &&    bytestream.peek() != BuildUSCacheUtils::INTRA_SEP
              &&    !bytestream.eof())
    {
        token.push_back(bytestream.get());
    }

    //Advance until valid char
    while(  (      bytestream.peek() == BuildUSCacheUtils::INTER_SEP
              ||   bytestream.peek() == BuildUSCacheUtils::INTRA_SEP
            )
              &&    !bytestream.eof())
    {
        bytestream.get();
    }

    return token;
}