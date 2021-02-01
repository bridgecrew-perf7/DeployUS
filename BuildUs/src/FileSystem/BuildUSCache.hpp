#pragma once
#include <boost/filesystem.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <Common/Common.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace fs = boost::filesystem;
using namespace std;

#define BUILDUS_CACHE_INTERMEDIATE_FOLDER fs::path("intermediate")
#define BUILDUS_CACHE_INTERMEDIATE_FILE BUILDUS_CACHE_INTERMEDIATE_FOLDER.append(".cache")
#define BUILDUS_CACHE_INTER_SEP '\n'
#define BUILDUS_CACHE_INTRA_SEP '\0'

class BuildUSCache
{
private:
    fs::path configParentPath;
    StringStringMap cached;

    void readCacheOnDisk();
    void writeCacheToDisk();
public:
    BuildUSCache(const fs::path& configDirectory);
    BuildUSCache();
    ~BuildUSCache();

    StringPairList const getFileForMinimalCompilation(const StringPairList& filesForCompilation);
    void updateCompiled(const StringPairList& filesCompiled);
};

namespace BuildUSCacheHelper{
    string getCacheToken(stringstream& bytestream);
}
