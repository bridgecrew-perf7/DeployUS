#include <iostream>
#include <string>

using namespace std;

void clearBuffer(char* buffer, int size)
// Replaces all contents of buffer with the null terminating character
{
	for(int i = 0; i < size; size++) buffer[i] = '\0';
}