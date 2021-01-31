#pragma once
#include <objects/GitBlob.hpp>
#include <list>
#include <utility>
#include <string>
#include <boost/tokenizer.hpp>

const char GITINDEXFILE_INTER_SEPERATOR = '\n';
const char GITINDEXFILE_INTRA_SEPERATOR = '\0';

typedef std::list<std::pair<string,GitBlob>> ListBlobs;
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