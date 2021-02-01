#include "GCCDriver.hpp"


GCCDriver::GCCDriver(ConfigFile* _config)
{
    this->config = _config;
    cache = BuildUSCache(this->config->getConfigPath().parent_path());
}

GCCDriver* GCCDriver::safeFactory(ConfigFile* _config)
// Catches all errors. Returns nullptr if an error occured
{
    GCCDriver* out;
    try
    {
        out = new GCCDriver(_config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        out = nullptr;
    }
    return out;       
}

GCCDriver::~GCCDriver()
{

}

StringPairList GCCDriver::toCompile()
//Retreives list of files to compile
{
    return this->cache.getFileForMinimalCompilation(this->config->getCompileList());
}

void GCCDriver::compile()
/*
    Compiles necessary files for minimal recompilation
*/
{
    StringPairList filesToCompile = this->toCompile();
    for(auto compileUnit: filesToCompile)
    {
        string filepathstr = compileUnit.second;
        cout << "Compiling: " << filepathstr << endl;
        auto filepath = fs::path(filepathstr);

        //1. Generate source file path
        fs::path sourcefile = config->getConfigPath().parent_path().append(filepathstr);
        
        //2. Generate destination path
        fs::path destPath = BUILDUS_CACHE_INTERMEDIATE_FOLDER;
        for(auto folder = filepath.begin(); folder != filepath.end(); folder ++)
        {
            string folderstr = folder->string();
            if(folder == filepath.begin())
                folderstr += ".dir";
            destPath.append(folderstr);
        }
        destPath.replace_extension(destPath.extension().string() + ".o");

        //3. Call system command tu run compiler on file
        fs::create_directories(destPath.parent_path());
        string cmd = generateCompilationCommand(sourcefile, destPath);
        systemCommand(cmd);
    }

    //4. Update Cache file
    this->cache.updateCompiled(filesToCompile);

}

/*
    ==================================================
                        Helper Functions
    ==================================================
*/

string generateCompilationCommand(fs::path filepath, fs::path destination)
/*
    Creates command to compile file specified by filepath. 
    Will place the generated assembly object file at destinationpath.
*/
{
    stringstream cmd;
    cmd << GCC_COMPILER << " ";
    cmd << "-c ";
    cmd << filepath.string() << " ";
    cmd << "-o ";
    cmd << destination.string() << " ";

    return cmd.str();
}
