#pragma once
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

extern const fs::path CONFIG1_PATH;
extern const fs::path CONFIG2_PATH;
extern const fs::path CONFIG3_PATH;

extern const fs::path BADCONFIG1_PATH; //No compilation units
extern const fs::path BADCONFIG2_PATH; //No project name
extern const fs::path BADCONFIG3_PATH; //Compilation file does not exists


//Methods
template<typename T>
bool vectorcompare(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
    //Check size
    if(vec1.size() != vec2.size())
        return false;

    //Sorting then comparing element-wise
    std::vector<T> vec1_copy(vec1);
    std::vector<T> vec2_copy(vec2);
    sort(vec1_copy.begin(), vec1_copy.end());
    sort(vec2_copy.begin(), vec2_copy.end());

    return vec1_copy == vec2_copy;
}


int clearIntermediate();