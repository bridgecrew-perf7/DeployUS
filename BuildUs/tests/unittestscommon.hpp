#include <algorithm>
using namespace std;

template<typename T>
bool vectorcompare(const vector<T>& vec1, const vector<T>& vec2)
{
    //Sorting then comparing element-wise
    vector<T> vec1_copy(vec1);
    vector<T> vec2_copy(vec2);
    sort(vec1_copy.begin(), vec1_copy.end());
    sort(vec2_copy.begin(), vec2_copy.end());
    return vec1_copy == vec2_copy;
}