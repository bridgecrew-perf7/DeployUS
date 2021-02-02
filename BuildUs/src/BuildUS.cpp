#include "BuildUS.hpp"
#include <iostream>
#include <FileSystem/ConfigFile.hpp>
#include <FileSystem/BuildUSCache.hpp>
#include <boost/filesystem.hpp>
#include <GCCDriver/GCCDriver.hpp>

namespace fs = boost::filesystem;

namespace BuildUS
{
    //Constants
    const char* BUILDUS_EXTENSION = ".buildus";
    const char* BUILDUS_CLEAN = "clean";

    /*
        Main function. Returns non-zero if an error occurs.
    */
    int start(int argc, const char *argv[])
    {
        //Verify Arugments
        if(argc != 2)
        {
            std::cout << "Error: Wrong number of arguments.\n";
            std::cout << "Usage: ./BuildUS <Config filepath>" << std::endl;
            return 1;
        }
        fs::path configPath = argv[1];
        
        //Special command: ./BuildUS clean
        if(configPath.compare(BUILDUS_CLEAN) == 0)
            return clean();

        //Check file extension
        if(configPath.extension().compare(BUILDUS_EXTENSION) != 0)
        {
            std::cout << "Error: Config file must have .buildus extension.\n";
            return 1;
        }


        //1. Load Config File
        ConfigFile* config = ConfigFile::safeFactory(configPath);
        if(config == nullptr)
            return 1;
        
        //2. Create GCCDriver Object
        GCCDriver* builder = GCCDriver::safeFactory(config);
        if(builder == nullptr)
            return 1;
        
        //1. Compiling step
        if(builder->compile())
        {
            std::cout << "Error: Could not compile files.\n";
            return 1;
        }


        //Reclaim memory
        delete config;
        delete builder;

        return 0;
    }

    int clean()
    //Removes intermediate folder
    {
        if(fs::exists(BUILDUS_CACHE_INTERMEDIATE_FOLDER))
        {
            try
            {
                fs::remove_all(BUILDUS_CACHE_INTERMEDIATE_FOLDER);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                std::cout << "Error: Could not clean directory.\n";
                return 1;
            }
        }
        return 0;
    }

}
