#include "BuildUS.hpp"
#include <iostream>
#include <FileSystem/ConfigFile.hpp>
#include <boost/filesystem.hpp>
#include <GCCDriver/GCCDriver.hpp>

namespace fs = boost::filesystem;

namespace BuildUS
{
    //Constants
    const char* BUILDUS_EXTENSION = ".buildus";


    /*
        Main function. Returns non-zero if an error occurs.
    */
    int start(int argc, const char *argv[])
    {
        //Verify Arugments
        if(argc != 2)
        {
            cout << "Error: Wrong number of arguments.\n";
            cout << "Usage: ./BuildUS <Config filepath>" <<endl;
            return 1;
        }
        fs::path configPath = argv[1];
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
        builder->compile();


        //Reclaim memory
        delete config;
        delete builder;

        return 0;
    }

    int clean()
    //Removes intermediate folder
    {
        return 1;
    }

}
