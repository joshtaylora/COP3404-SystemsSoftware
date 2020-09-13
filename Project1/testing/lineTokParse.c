#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    // open the file specified by the user
    FILE *inputFile;
    inputFile = fopen( argv[1], "r" );

    char fileArg[1024];
    int current_line = 0;

    int loc_counter;

    while( fgets(fileArg, 1024, inputFile) ) {
        current_line++;
        printf("CURRENT LINE: %d\n", current_line);
        // splits the line into characters
        
        // splits the file into lines
        char *line = strtok(fileArg, "\n");

        
        while(line) {
            // check for comment
            if (line[0] == 35) {
                printf("COMMENT\n");
                line = strtok(NULL, "\n");
                continue;
            }
            char *token = strtok(line, " \t");
            while (token) {
                if (token[0] >= 65 && token[0] <= 96) {
                    char *symbol = token;
                    char *opcode = strtok(NULL, " \t");
                    char *operand = strtok(NULL, " \t");
                    // check for the start command
                    if (strcmp(opcode, "START") == 0) {
                        char *hex = "0x";
                        strcat(hex, operand);
                        printf("----hexstring: %s", hex);
                    }
                }
            }
            // advance to next line
            printf("\tNew Line\n");
            line = strtok(NULL, "\n");
        }
    }
    return 0;
}
