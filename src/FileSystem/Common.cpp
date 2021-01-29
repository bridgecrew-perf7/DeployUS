#include "Common.hpp"

stringstream readFile(fs::path filepath)
/*
    Returns stringstream of all characters in file. 
    Throws std::runtime_error if file does not exists.
*/
{
    stringstream contents;
    if(!fs::exists(filepath))
    {
        stringstream msg;
        msg << "Filepath " << filepath.string() << " does not exists.\n";
        throw runtime_error(msg.str());
    }
    //Open file
    fs::ifstream file(filepath);
    //Read all contents
    contents << file.rdbuf();
    //Close file
    file.close();

    return contents;
}