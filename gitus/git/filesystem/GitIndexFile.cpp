#include "GitIndexFile.hpp"
#include "GitFilesystem.hpp"
#include <common.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

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
    std::stringstream bytestream(indexContents);
    while(!bytestream.eof())
    {
        //Read line
        string line;
        while(!bytestream.eof())
        {
            char c = bytestream.get();
            if(c == GITINDEXFILE_INTER_SEPERATOR)
                break;
            line.push_back(c);
        }

        //Split line on null-terminating character
        auto nulltermPos = line.find(GITINDEXFILE_INTRA_SEPERATOR);
        if(nulltermPos == std::string::npos) //Line does not contain a valid entry
            break;
        string filepath = line.substr(0,nulltermPos);
        string hash = line.substr(nulltermPos + 1);

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

int GitIndexFile::addBlob(GitBlob blob) // AB - passage par copie voulue? -2
//Adds blob to internal representation of index file. Returns non-zero if unsuccessful
// AB - mauvais commentaire. Retourne seulement 0. -2
{
    string relativePath = blob.getRelativePath();
    
    //Check to see that file is not already in index file
    for (auto it = blobs.begin(); it != blobs.end(); )
    {
        // AB - tu pourrais utiliser la fonction remove_if
        //      ca évite de faire la boucle manuellement et ca simplifie le code

        if(it->first.compare(relativePath) == 0) // AB - pourquoi pas juste (it->first == relativePath) - Code surcomplexe - 2
        {
            it = blobs.erase(it); // AB - l'assignation sert à quoi?
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
    for(auto blob: blobs) // AB - cute
    {
        // AB - find_if aurait été bien ici
        if(blob.first.compare(filepath) == 0 && blob.second.getSHA1Hash().compare(hash) == 0)
            return 1;
    }
    return 0;
}

// AB - code mort -2
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

// AB - code mort
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
//Returns number of blobs in index file
{
    return blobs.size();
}
