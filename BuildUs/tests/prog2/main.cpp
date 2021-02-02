#include <iostream>
#include "utils.hpp"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

int main(int argc, const char** argv) {
    std::cout << "I invested in: " << utils::getStonks() << std::endl;

    if(fs::exists("utils.hpp"))
        std::cout << ARKK << std::endl;
    return 0;
}