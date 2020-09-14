#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    // open the file specified by the user
    FILE *inputFile;
    inputFile = fopen( argv[1], "r" );
    // buffer to read characters from the line into
    char line[1024];
    // int that acts as a counter to store what line we are on
    int current_line = 1;

    // make a counter to store if the START directive has been encountered
    int startCheck = 0;

    // while fgets can read lines from the file to the char buffer called line (up to 1024 characters)
    while( fgets(line, 1024, inputFile) ) {

        printf("CURRENT LINE: %d\n", current_line);
        
        for (int i = 0; i < strlen(line); i++) {
            printf("line[%d], char[%d] = %c\n",current_line, i, line[i]);
        }

        
        // check for a comment
        if (line[0] == 35) {
            printf("----COMMENT \n");
            // move to the next line
            continue;
        }


        // splits the line into tokens
        char *token = strtok(line, " \t");
        // while there are still tokens
        while(token) {
            int loc_counter;
            // check for a symbol definition -> first char in 
            if (line[0] >= 65 && line[0] <= 96) {
                char *symbol = token;
                token = strtok(NULL, " \t");
                // if the directive is START, set the loc_counter = the start address 
                if (strcmp(token, "START") == 0) {
                    startCheck = 1;
                    // save the current token as opcode
                    char *opcode = token;
                    // advance to the next token
                    token = strtok(NULL, " \t");
                    char *address = token;
                    char hex[15] = "0X";
                    strcat(hex, address);
                    printf("\tHEX: %s\n", hex);
                    loc_counter = (int)strtol(hex, NULL, 0);
                    printf("\n loc_counter: %d\n", loc_counter);
                    printf("symbol: %s, opcode: %s, address: %X\n", symbol, opcode, loc_counter);

                }
                else {
                    loc_counter += 3;
                }
            }
            // if there is not a symbol definition but there is a directive
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
                    printf("\topcode: %s, address: %d\n", opcode, loc_counter);
                }
            }
            token = strtok(NULL, " \n");
        }
        
        printf("\tNew Line\n");
        // increment the line counter
        current_line++;
    }
    return 0;
}
