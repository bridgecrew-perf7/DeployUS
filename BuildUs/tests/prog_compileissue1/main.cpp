#include <iostream>
#include "utils.hpp"

int main(int argc, const char** argv) {
    cout << "I invested in: " << utils::getStonks() << std::endl; //Erreur intentionelle de namespace
    return 0;
}