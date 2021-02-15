#pragma once
#include <objects/GitBlob.hpp>
#include <list>
#include <utility>
#include <string>
#include <boost/tokenizer.hpp>

// AB - ca ne vaut pas la peine de les mettre dans le .h si c'est du détail uniquement utilisé dans le .cpp
const char GITINDEXFILE_INTER_SEPERATOR = '\n';
const char GITINDEXFILE_INTRA_SEPERATOR = '\0';

typedef std::list<std::pair<string,GitBlob>> ListBlobs; // AB - c'est très C. Les using c'est plus C++ et intuitif
                                                        //    - using ListBlobs = std::list<std::pair<string,GitBlob>>
typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

class GitIndexFile
{
    private:
        ListBlobs blobs;

    public:
        GitIndexFile();
        ~GitIndexFile();

        int addBlob(GitBlob blob);
        GitBlob getBlobReference(const string filepath);
        GitBlob getBlobReference(const fs::path filepath);
        
        void produceIndexFile();
        ListBlobs parseIndexFile();

        int contains(const string filepath, const string hash);
        int count(const string filepath);
        int count(const fs::path filepath);

        int size();
};