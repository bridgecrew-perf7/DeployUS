#pragma once
#include <Common/Common.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const fs::path BUILDUS_CACHE_INTERMEDIATE_FOLDER = fs::path("intermediate");
const fs::path BUILDUS_CACHE_INTERMEDIATE_FILE = fs::path(BUILDUS_CACHE_INTERMEDIATE_FOLDER).append(".cache");
const char BUILDUS_CACHE_INTER_SEP = '\n';
const char BUILDUS_CACHE_INTRA_SEP = '\0';

class BuildUSCache
{
private:
    fs::path configParentPath;
    ThreeStringTupleList cached;

    void readCacheOnDisk();
    void writeCacheToDisk();
public:
    BuildUSCache(const fs::path& configDirectory);
    BuildUSCache();
    ~BuildUSCache();

    StringPairList const getFileForMinimalCompilation(const StringPairList& filesForCompilation);
    int updateCompiled(const StringPairList& filesCompiled);
};

namespace BuildUSCacheUtils{
    string getCacheToken(std::stringstream& bytestream);
}
