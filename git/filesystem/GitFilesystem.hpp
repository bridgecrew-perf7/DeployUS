#pragma once
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace GitFilesystem
{
    extern const std::string DOTGIT_FOLDER_NAME;
    extern const std::string OBJECTS_FOLDER_NAME;
    extern const std::string INDEX_FILE_NAME;
    extern const std::string HEAD_FILE_NAME;
    extern const std::string TOPCOMMIT_FILE_NAME;
    extern const fs::path DotGitFolderPath;

    fs::path getDotGitPath();
    fs::path getObjectsPath();
    fs::path getIndexPath();
    fs::path getHEADPath();
    fs::path getTOPCOMMITPath();
};