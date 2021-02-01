#pragma once
#include <algorithm>

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