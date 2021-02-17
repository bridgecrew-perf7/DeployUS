#include "BuildUSCache.hpp"
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <Common/Common.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>


BuildUSCache::BuildUSCache(ConfigFile* configPtr)
{
    this->config = configPtr;
    this->cached = ThreeStringTupleList();

    if(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER))
    {
        //Read cached files
        this->readCompileCacheOnDisk();
    }
    else //Intermediate folder does not exists.
    {
        fs::create_directory(BUILDUS_CACHE_INTERMEDIATE_FOLDER);
    }
}

BuildUSCache::BuildUSCache()
{
}

BuildUSCache::~BuildUSCache()
{
}

void BuildUSCache::readCompileCacheOnDisk()
/* 
    Reads and parses the cache file, if it exists.
*/
{
    //Verify the file's existence
    if(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE))
        return;

    //1. Build Representation of cache (Parsing)
    std::stringstream cachecontents = readFile(BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE);
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
}

void BuildUSCache::writeCompileCacheToDisk()
/*
    Write the cache map to disk. Can throw std::runtime_error;
*/
{
    std::stringstream cachecontents;

    //Formating: <outputfile>\0<filepath>\0<SHA1>\n
    for(auto compiledFileDesc: this->cached)
    {
        cachecontents << ThreeStringTupleUtils::getOutputFileName(compiledFileDesc);  //output file
        cachecontents << BUILDUS_CACHE_INTRA_SEP;
        cachecontents << ThreeStringTupleUtils::getSourceFilePath(compiledFileDesc);  //source file path
        cachecontents << BUILDUS_CACHE_INTRA_SEP;
        cachecontents << ThreeStringTupleUtils::getSourceSHA1(compiledFileDesc);      //hash
        cachecontents << BUILDUS_CACHE_INTER_SEP;
    }

    //Flush to disk
    if(writeFile(BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE, cachecontents.str()))
    {
        throw std::runtime_error("Unable to write .cache file.");
    }

}

bool const BuildUSCache::mustLink()
//Returns true if the project.cache will change.
//  The file will change if the project SHA1 is different, or if the project.file does not exists.
//Returns false otherwise
{
    std::stringstream diskFile;
    
    //Load project.cache
    if(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE))
        return true;
    diskFile = readFile(BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE);

    //Get executablePath and SHA1 from disk
    string diskExecutableRelativePath = BuildUSCacheUtils::getCacheToken(diskFile);
    string diskSHA1 = BuildUSCacheUtils::getCacheToken(diskFile);

    //Get executablePath and SHA1 of config
    string configExecutableRelativePath = this->getExecutablePath().string();
    string configSHA1 = generateSHA1(this->config->toString());


    bool pathHasChanged = diskExecutableRelativePath.compare(configExecutableRelativePath) != 0;
    bool sha1HasChanged = diskSHA1.compare(configSHA1) != 0;
    bool executableDeleted = !fs::exists(configExecutableRelativePath);
    return executableDeleted || pathHasChanged || sha1HasChanged;
}

void const BuildUSCache::writeProjectCacheToDisk()
//Fill project.cache file
{
    std::stringstream out;
    out << this->getExecutablePath().string();
    out << BUILDUS_CACHE_INTRA_SEP;
    out << generateSHA1(this->config->toString());
    writeFile(BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE, out.str());
}

StringPairList const BuildUSCache::getFileForMinimalCompilation(const StringPairList& filesForCompilation)
/*
    Argument: List of entries in Config File

    Looks to see if files to compile 
        1. have been modified
        2. have never been compiled. 

    Returns list of files that fit these criterias.
*/
{
    StringPairList filesToCompile;
    for(auto compileUnit: filesForCompilation)
    {
        bool needCompilation = true;
        string compileUnitOutputPath = compileUnit.first + COMPILE_OBJECT_EXT;
        string compileUnitFilePath = compileUnit.second;

        fs::path compileUnitFilePathRelativeToConfig = this->config->getConfigParentPath().append(compileUnitFilePath);
        string compileUnitSHA1 = generateSHA1(readFile(compileUnitFilePathRelativeToConfig).str());

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

    return filesToCompile;
}

int BuildUSCache::updateCompiled(const StringPairList& filesCompiled)
/*
    Update cached attribut with newly compiled files.
    Each file has its SHA1 computed in order to check if they have changed

    Returns zero if success, non-zero if unsucessful.
*/
{
    for(auto compileUnit: filesCompiled)
    {
        string filepathstr = compileUnit.second;
        std::stringstream filecontents = readFile(this->config->getConfigParentPath().append(filepathstr));

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
    try
    {
        this->writeCompileCacheToDisk();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
   
    return 0;
}

const fs::path BuildUSCache::getExecutablePath()
{
    fs::path execPath = BUILDUS_CACHE_INTERMEDIATE_FOLDER.parent_path().append(this->config->getProjectName());
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
    while(          bytestream.peek() != BUILDUS_CACHE_INTER_SEP
              &&    bytestream.peek() != BUILDUS_CACHE_INTRA_SEP
              &&    !bytestream.eof())
    {
        token.push_back(bytestream.get());
    }

    //Advance until valid char
    while(  (      bytestream.peek() == BUILDUS_CACHE_INTER_SEP
              ||   bytestream.peek() == BUILDUS_CACHE_INTRA_SEP
            )
              &&    !bytestream.eof())
    {
        bytestream.get();
    }

    return token;
}