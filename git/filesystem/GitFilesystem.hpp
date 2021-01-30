#pragma once
#include <string>
#include <boost/filesystem.hpp>

// AB - des defines c'est du C, pas du c++
#define GITFILESYSTEM_DOTGIT_FOLDER_NAME ".git"
#define GITFILESYSTEM_OBJECTS_FOLDER_NAME "objects"
#define GITFILESYSTEM_INDEX_FILE_NAME "index"
#define GITFILESYSTEM_HEAD_FILE_NAME "HEAD"
#define GITFILESYSTEM_TOPCOMMIT_FILE_NAME "TOP_COMMIT"

namespace fs = boost::filesystem;

// AB - classe completement inutile
//      fait un namespace

class GitFilesystem
{
    private:
        static fs::path dotgitFolderPath;

    public:
        GitFilesystem(); // AB - pourquoi avoir un ctor?
        ~GitFilesystem(); // AB - même question pour dtor

        // AB - poruquoi inline? As tu une bonne raison?
        //      Le compilateur est meilleur que toi pour déterminer ca

        // AB - pourquoi pas juste avoir fait des constantes?
        inline static fs::path getDotGitPath() {return fs::path(dotgitFolderPath);};
        inline static fs::path getObjectsPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_OBJECTS_FOLDER_NAME);};
        inline static fs::path getIndexPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_INDEX_FILE_NAME);};
        inline static fs::path getHEADPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_HEAD_FILE_NAME);};
        inline static fs::path getTOPCOMMITPath() {return fs::path(dotgitFolderPath).append(GITFILESYSTEM_TOPCOMMIT_FILE_NAME);};
};


// AB - aucune définition???
fs::path findDotGitFolder();