#pragma once
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const fs::path BUILDUS_CACHE_INTERMEDIATE_FOLDER = fs::path("intermediate");
const fs::path BUILDUS_CACHE_INTERMEDIATE_COMPILE_CACHE = fs::path(BUILDUS_CACHE_INTERMEDIATE_FOLDER).append("compile.cache");
const fs::path BUILDUS_CACHE_INTERMEDIATE_PROJECT_CACHE = fs::path(BUILDUS_CACHE_INTERMEDIATE_FOLDER).append("project.cache");
const char BUILDUS_CACHE_INTER_SEP = '\n';
const char BUILDUS_CACHE_INTRA_SEP = '\0';

class BuildUSCache
{
private:
    ConfigFile config;
    ThreeStringTupleList cached;

    const fs::path getExecutablePath(); 

    int readCompileCacheOnDisk();
    int writeCompileCacheToDisk();

public:
    BuildUSCache(ConfigFile& config);
    BuildUSCache();
    ~BuildUSCache();

    int const getFileForMinimalCompilation(const StringPairList& filesForCompilation, StringPairList& filesToCompile);
    int updateCompiled(const StringPairList& filesCompiled);

    bool const mustLink(std::stringstream& compileCacheContents);
    void const writeProjectCacheToDisk();
};

namespace BuildUSCacheUtils{
    string getCacheToken(std::stringstream& bytestream);
}
