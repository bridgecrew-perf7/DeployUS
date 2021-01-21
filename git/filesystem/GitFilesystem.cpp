#include "GitFilesystem.hpp"
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;

//static member initialization
fs::path GitFilesystem::dotgitFolderPath = fs::current_path().append(GITFILESYSTEM_DOTGIT_FOLDER_NAME);


GitFilesystem::GitFilesystem()
{
    
}

GitFilesystem::~GitFilesystem()
{
}