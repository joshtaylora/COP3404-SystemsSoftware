#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    // open the file specified by the user
    FILE *inputFile;
    inputFile = fopen( argv[1], "r" );

    char line[1024];
    int current_line = 0;

    while( fgets(line, 1024, inputFile) ) {

        current_line++;
        printf("CURRENT LINE: %d\n", current_line);
        
        for (int i = 0; i < strlen(line); i++) {
            printf("line[%d], char[%d] = %c\n",current_line, i, line[i]);
        }

        
        // check for a comment
        if (line[0] == 35) {
            printf("----COMMENT \n");
            continue;
        }


        // splits the line into characters
        // while there are still tokens
        char *token = strtok(line, " \t");
        while(token) {
            int loc_counter;
            // check for a symbol definition
            if (token[0] >= 65 && token[0] <= 96) {
                char *symbol = token;
                token = strtok(NULL, " \t");
                if (strcmp(token, "START") == 0) {
                    char *opcode = token;
                    token = strtok(NULL, " \t");
                    char *address = token;
                    char hex[15] = "0x";
                    strcat(hex, address);
                    int hexint = atoi(hex);
                    loc_counter = (int)strtol(hex, NULL, 16);
                    printf("symbol: %s, opcode: %s, address: %d\n", symbol, opcode, loc_counter);

                }
                else {
                    loc_counter += 3;
                }
            }
            else {
                char *opcode = token;
                if (strcmp(token, "WORD") == 0) {
                    token = strtok(NULL, " \t");
                    char *operand = token;
                    loc_counter += 3;
                    printf("\t opcode: %s, address: %d\n", opcode, loc_counter);
                }
                else {
                    token = strtok(NULL, " \t");
                    char *operand = token;
                    loc_counter += 3;
                    printf("\t opcode: %s, address: %d\n", opcode, loc_counter);
                }
            }
            token = strtok(NULL, " \n");
        }
        
        printf("\tNew Line\n");

    }
    return 0;
}
