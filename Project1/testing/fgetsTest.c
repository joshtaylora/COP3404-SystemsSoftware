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

        current_line++;
        printf("CURRENT LINE: %d\n", current_line);
        
        for (int i = 0; i < strlen(line); i++) {
            printf("line[%d], char[%d] = %c\n",current_line, i, line[i]);
        }

        // splits the line into characters
        // while there are still tokens
        while(line) {
            char *tab1 = strtok(line, " \t");
            char *tab2 = strtok(NULL, " \t");
            char *tab3 = strtok(NULL, " \t");
            char *tab4 = strtok(NULL, " \t");
            int loc_counter;
            // check for a comment
            if (tab1[0] == 35) {
                printf("----COMMENT \n");
                line = strtok(NULL, "\n");
                continue;
            }
            else if (tab1[0] >= 65 && tab1[0] <= 96) {
                if (strcmp(tab2, "START") == 0) {
                    char hex[15] = "0x";
                    strcat(hex, tab3);
                    const char *hexstring = hex;
                    printf("---hex: %s\n", hexstring);
                    loc_counter = (int)strtol(hexstring, NULL, 0);
                    printf("\tLocation Counter: %d", loc_counter);
                }
                else {
                    loc_counter += 3;
                }
                printf("\tsymbol: %s, opcode: %s, address: %d\n", tab1, tab2, loc_counter);
            }
            else {
                printf("%s\t%s\t%s\t%s\t", tab1, tab2, tab3, tab4);
            }
            printf("\tNew Line\n");
            line = strtok(NULL, "\n");
        }
    }
    return 0;
}
