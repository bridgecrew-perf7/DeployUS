#include "GitFilesystem.hpp"

const std::string GitFilesystem::DOTGIT_FOLDER_NAME = std::string(".git");
const std::string GitFilesystem::OBJECTS_FOLDER_NAME = std::string("objects");
const std::string GitFilesystem::INDEX_FILE_NAME = std::string("index");
const std::string GitFilesystem::HEAD_FILE_NAME = std::string("HEAD");
const std::string GitFilesystem::TOPCOMMIT_FILE_NAME = std::string("TOP_COMMIT");
const fs::path GitFilesystem::DotGitFolderPath = fs::current_path().append(DOTGIT_FOLDER_NAME);

fs::path GitFilesystem::getDotGitPath() {return fs::path(DotGitFolderPath);};
fs::path GitFilesystem::getObjectsPath() {return fs::path(DotGitFolderPath).append(OBJECTS_FOLDER_NAME);};
fs::path GitFilesystem::getIndexPath() {return fs::path(DotGitFolderPath).append(INDEX_FILE_NAME);};
fs::path GitFilesystem::getHEADPath() {return fs::path(DotGitFolderPath).append(HEAD_FILE_NAME);};
fs::path GitFilesystem::getTOPCOMMITPath() {return fs::path(DotGitFolderPath).append(TOPCOMMIT_FILE_NAME);};