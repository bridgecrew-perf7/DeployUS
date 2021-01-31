#include "GitIndexFile.hpp"
#include "GitFilesystem.hpp"
#include <common.hpp>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>

GitIndexFile::GitIndexFile()
{
    blobs = parseIndexFile();
}

GitIndexFile::~GitIndexFile()
{

}

ListBlobs GitIndexFile::parseIndexFile()
//Returns datastructure that describes present state of index file
{
    ListBlobs indexBlobs = ListBlobs();

    //Read Indexfile
    string indexContents = Common::readFile(GitFilesystem::getIndexPath());

    //Parsing...
    boost::char_separator<char> sepnewline{ string(1,GITINDEXFILE_INTER_SEPERATOR).c_str()};
    tokenizer newline{indexContents, sepnewline};
    for(const auto& line: newline)
    {
        //Split line on null-terminating character
        auto nulltermPos = line.find(GITINDEXFILE_INTRA_SEPERATOR);
        string filepath = line.substr(0,nulltermPos);
        string hash = line.substr(nulltermPos + 1, line.size() - nulltermPos - 1);

        indexBlobs.push_back(std::pair<string, GitBlob>(filepath, GitBlob::createFromGitObject(hash)));
    }

    return indexBlobs;

}

void GitIndexFile::produceIndexFile()
//Generates clean new IndexFile
{
    //generate output string
    string output("");
    for(auto blob: blobs)
    {
        output += blob.first;
        output += GITINDEXFILE_INTRA_SEPERATOR;
        output += blob.second.generateHash();
        output += GITINDEXFILE_INTER_SEPERATOR;
    }

    //Fetch the end of the IndexFile
    std::ofstream out_indexfile(GitFilesystem::getIndexPath().c_str(), std::ios_base::out);
    out_indexfile << output;

    out_indexfile.close();
}

int GitIndexFile::addBlob(GitBlob blob)
//Adds blob to internal representation of index file. Returns non-zero if unsuccessful
{
    string relativePath = blob.getRelativePath();
    
    //Check to see that file is not already in index file
    for (auto it = blobs.begin(); it != blobs.end(); ) 
    {
        if(it->first.compare(relativePath) == 0)
        {
            it = blobs.erase(it);
            break;
        }
        else
            it++;
    }

    //We are now guaranteed that there is no reference in GitIndexFile for this blob, we can add it.
    blobs.push_back(std::pair<string,GitBlob>(relativePath,blob));

    return 0;
}

int GitIndexFile::contains(const string filepath, const string hash)
//Looks if filepath is contained inside the index file.
//Returns non-zero if the filepath is inside the index file.
//Returns zero otherwise.
{
    for(auto blob: blobs)
    {
        if(blob.first.compare(filepath) == 0 && blob.second.getSHA1Hash().compare(hash) == 0)
            return 1;
    }
    return 0;
}

int GitIndexFile::count(const string filepath)
//Returns the number of occurance of filepath
{
    int count = 0;
    for(auto blob: blobs)
    {
        if(blob.first.compare(filepath) == 0)
            count++;
    }
    return count;
}

int GitIndexFile::count(const fs::path filepath)
//Returns the number of occurance of filepath
{
    return count(filepath.string());
}

GitBlob GitIndexFile::getBlobReference(const string filepath)
//Returns Gitblob that has same path as in argument
//If doesn't exists, returns empty GitBlob
{
    for(auto blob: blobs)
    {
        if(blob.first.compare(filepath) == 0)
            return blob.second;
    }

    //Return a null reference
    return GitBlob();
}

GitBlob GitIndexFile::getBlobReference(const fs::path filepath)
//Returns Gitblob that has same path as in argument
//If doesn't exists, returns empty GitBlob
{
    return getBlobReference(filepath.string());
}

int GitIndexFile::size()
//Returns number of blobs in infex file
{
    return blobs.size();
}
