#include <iostream>
#include <string>

using namespace std;

void clearBuffer(char* buffer, int size)
// Replaces all contents of buffer with the null terminating character
{
	for(int i = 0; i < size; size++) buffer[i] = '\0';
}


string execSystemCommand(string command) {
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return string("popen failed!");
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}