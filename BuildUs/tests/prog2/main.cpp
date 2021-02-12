#include <iostream>
#include "utils.hpp"
#include "boost/filesystem.hpp"
#include <boost/math/constants/constants.hpp>

namespace fs = boost::filesystem;

int main(int argc, const char** argv) {
    //1. General printing
    std::cout << "I wish I invested in: " << utils::getStonks() << std::endl;

    //2. Use of boost filesystem
    if(fs::exists("utils.hpp"))
        std::cout << ARKK << std::endl;

    //3. Use of boost constants GOLDEN RATIO
    std::cout.precision(50);
    std::cout << "Golden ratio: " << boost::math::constants::phi<double>() << std::endl; 
    return 0;
}