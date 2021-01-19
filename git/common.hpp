#pragma once
#include <string>

#define INDEX_FILE_DEMLIMETER '\0'

using namespace std;

string generateSHA1(string text);
string readFile(const char* path);