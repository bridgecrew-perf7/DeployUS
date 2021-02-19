#pragma once
#include <Common/Common.hpp>
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class BuildUSCache
{
private:
    ConfigFile config;
    ThreeStringTupleList cached;
    StringPairList filesCompiled;

    const fs::path getExecutablePath(); 

    int readCompileCacheOnDisk();
    int writeCompileCacheToDisk();

public:
    BuildUSCache(ConfigFile& config);
    BuildUSCache();
    ~BuildUSCache();

    int const getFileForMinimalCompilation(const StringPairList& filesForCompilation, StringPairList& filesToCompile);
    int updateCompiled(const StringPairList filesCompiled);

    int const mustLink();
    void const writeProjectCacheToDisk();
};

namespace BuildUSCacheUtils{

    const fs::path INTERMEDIATE_FOLDER = fs::path("intermediate");
    const fs::path INTERMEDIATE_COMPILE_CACHE = fs::path(INTERMEDIATE_FOLDER).append("compile.cache");
    const fs::path INTERMEDIATE_PROJECT_CACHE = fs::path(INTERMEDIATE_FOLDER).append("project.cache");
    const char INTER_SEP = '\n';
    const char INTRA_SEP = '\0';

    string getCacheToken(std::stringstream& bytestream);
}
