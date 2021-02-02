#include <iostream>
#include "utils.hpp"

int main(int argc, const char** argv) {
    std::cout << "I invested in: " << utils::getStonks() << std::endl;
    return 0;
}