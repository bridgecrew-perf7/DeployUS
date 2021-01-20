#pragma once
#include <string>
#include <boost/filesystem.hpp>

#define GITFILESYSTEM_DOTGIT_FOLDER_NAME ".git"
#define GITFILESYSTEM_OBJECTS_FOLDER_NAME "objects"
#define GITFILESYSTEM_INDEX_FILE_NAME "index"
#define GITFILESYSTEM_HEAD_FILE_NAME "HEAD"

namespace fs = boost::filesystem;
using namespace std;

class GitFilesystem
{
    private:
        static fs::path dotgitFolderPath;

    public:
        GitFilesystem();
        ~GitFilesystem();

        inline static fs::path getDotGitPath() {return fs::path(dotgitFolderPath);};
        inline static fs::path getObjectsPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_OBJECTS_FOLDER_NAME);};
        inline static fs::path getIndexPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_INDEX_FILE_NAME);};
        inline static fs::path getHEADPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_HEAD_FILE_NAME);};
};


fs::path findDotGitFolder();