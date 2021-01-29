#include <iostream>
#include <ConfigFile.hpp>
using namespace std;


/*
    Main function. Returns non-zero if an error occurs.
*/
int main(int argc, char const *argv[])
{
    //Verify Arugments
    if(argc != 2)
    {
        cout << "Error: Wrong number of arguments.\n";
        cout << "Usage: ./BuildUS <Config filepath>" <<endl;
        return 1;
    }
    const char* configPath = argv[1];


    //Load Config File
    ConfigFile config(configPath);
    cout << config.toString() << endl;

    return 0;
}
