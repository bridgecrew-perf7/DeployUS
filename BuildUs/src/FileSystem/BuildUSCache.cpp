#include "BuildUSCache.hpp"

BuildUSCache::BuildUSCache(const fs::path& configDirectory)
{
    this->configParentPath = configDirectory;
    this->cached = StringStringMap();

    if(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER))
    {
        //Read cached files
        this->readCacheOnDisk();
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

void BuildUSCache::readCacheOnDisk()
/* 
    Reads and parses the cache file, if it exists.
*/
{
    //Verify the file's existence
    if(!fs::exists(BUILDUS_CACHE_INTERMEDIATE_FILE))
        return;

    //1. Build Representation of cache (Parsing)
    stringstream cachecontents = readFile(BUILDUS_CACHE_INTERMEDIATE_FILE);
    while(!cachecontents.eof())
    {
        //Read in new line
        string filepath = BuildUSCacheHelper::getCacheToken(cachecontents);
        string fileSHA1 = BuildUSCacheHelper::getCacheToken(cachecontents);
        
        //Fill in cache map
        this->cached[filepath] = fileSHA1;

    }
}

StringPairList const BuildUSCache::getFileForMinimalCompilation(const StringPairList& filesForCompilation)
/*
    Looks to see if files to compile 
        1. have been modified
        2. have never been compiled. 

    Returns list of files that fit these criterias.
*/
{
    StringPairList filesToCompile;
    for(auto compileUnit: filesForCompilation)
    {
        string filepathstr = compileUnit.second;
        bool mustCompile = false;

        //Never has been compiled
        if(this->cached.count(filepathstr) == 0)
        {
            mustCompile = true;
        }
        else
        {
            fs::path filepath = fs::path(this->configParentPath).append(filepathstr);
            string fileSHA1 = generateSHA1(readFile(filepath).str());

            //SHA of file has changed, must recompile.
            if(this->cached[filepathstr].compare(fileSHA1) != 0)
            {
                mustCompile = true;
            }
        }
        
        if(mustCompile)
            filesToCompile.push_back(compileUnit);
    }
    return filesToCompile;
}

void BuildUSCache::updateCompiled(const StringPairList& filesCompiled)
/*
    Update cached attribut with newly compiled files.
    Each file has its SHA1 computed in order to check if they have changed
*/
{
   for(auto compileUnit: filesCompiled)
   {
        string filepathstr = compileUnit.second;
        stringstream filecontents = readFile(fs::path(this->configParentPath).append(filepathstr));

        //1. Compute SHA1 of file contents to obtain unique id of file version.
        string fileSHA1 = generateSHA1(filecontents.str());

        //2. Add file path and its has to the cache map
        this->cached[filepathstr] = fileSHA1;
   }

   //3. Flush cache to disk
   this->writeCacheToDisk();

}

void BuildUSCache::writeCacheToDisk()
/*
    Write the cache map to disk.
*/
{
    stringstream cachecontents;

    //Formating: <filepath>\0<SHA1>\n
    for(auto compiledFileDesc: this->cached)
    {
        cachecontents << compiledFileDesc.first;
        cachecontents << BUILDUS_CACHE_INTRA_SEP;
        cachecontents << compiledFileDesc.second;
        cachecontents << BUILDUS_CACHE_INTER_SEP;
    }

    //Flush to disk
    if(writeFile(BUILDUS_CACHE_INTERMEDIATE_FILE, cachecontents.str()))
    {
        throw runtime_error("Unable to write .cache file.");
    }

}


/*
    ==============================
            HELPER FUNCTIONS
    ==============================
*/
string BuildUSCacheHelper::getCacheToken(stringstream& bytestream)
//Returns next token
{
    string token;
    while(          bytestream.peek() != BUILDUS_CACHE_INTER_SEP
              &&    bytestream.peek() != BUILDUS_CACHE_INTRA_SEP
              &&    !bytestream.eof())
    {
        token.push_back(bytestream.get());
    }
    
    //Remove seperator
    if(!bytestream.eof())
        bytestream.get();

    return token;
}