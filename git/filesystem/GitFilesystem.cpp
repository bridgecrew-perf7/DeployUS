#include "GitFilesystem.hpp"
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;

//static member initialization
fs::path GitFilesystem::dotgitFolderPath = fs::path();

GitFilesystem::GitFilesystem()
{
    if(dotgitFolderPath.empty())
    {
        fs::path possibleGitParentPath = fs::current_path();

        //Check if .git folder is present and save location. Else, check parent folders
        while(!possibleGitParentPath.empty())
        {
            fs::path possibleGitPath = fs::path(possibleGitParentPath).append(GITFILESYSTEM_DOTGIT_FOLDER_NAME);
            if(fs::exists(possibleGitPath))
            {
                dotgitFolderPath = possibleGitPath;
                break;
            }
            else
            {
                //Must we support fetching root of gitus project? 
                //possibleGitParentPath = possibleGitParentPath.parent_path();
                possibleGitParentPath = fs::path();
            }
        }
    }   
    
}

GitFilesystem::~GitFilesystem()
{
}
