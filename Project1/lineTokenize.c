#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    // open the file specified by the user
    FILE *inputFile;
    inputFile = fopen( argv[1], "r" );

    char fileArg[1024];
    int current_line = 0;

    while( fgets(fileArg, 1024, inputFile) ) {
        // splits the file into lines
        char *line = strtok(fileArg, "\n");
        // increment the line counter
        current_line++;
        // print the current line
        printf("CURRENT LINE: %d\n", current_line);

        // splits the line into characters
        char *token = strtok(line, " \t");
        
        // while there are still tokens
        while(token) {
            if (line[0] == 35) {
                printf("--was a comment\n");
            }
            if (line[0] >= 65 && line[0] <= 96) {
                char *symbol = token;
                printf("symbol: %s", symbol);
                char *opcode = strtok(NULL, " \t");
                if (opcode != NULL) {
                    printf("\topcode: %s", opcode);
                    char *operand = strtok(NULL, " \t");
                    if (operand != NULL) {
                        printf("\toperand: %s", operand);
                    }
                }
                printf("\n");

            }
            else {
                if (token[0] >= 65 && token[0] <= 96) {
                    char *opcode = token;
                    printf("\topcode: %s", opcode);
                    char *operand = strtok(NULL, " \t");
                    if (operand != NULL) {
                        printf("\toperand: %s", operand);
                    }
                    printf("\n");
                }
                printf("not a symbol definition\n");
            }

            token = strtok( NULL, " \t" );
        }
        line = strtok(NULL, "\n");
    }
    return 0;
}
