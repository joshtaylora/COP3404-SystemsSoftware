#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
   FILE *inputFile;
   inputFile = fopen(argv[1], "r");

   char line[1024];
   
   int current_line;

   while(fgets(line, 1024, inputFile)) {
       printf("LINE[%d] = %s", current_line, line);
       current_line++;
   }
}
