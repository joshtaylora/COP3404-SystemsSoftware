#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    fp = fopen(argv[1], "r");

    if (fp == 0)
    {
        printf("ERROR: no file passed to assembler\n");
        return 0;
    }

    char line[1024];

    int lineNumber = 1;
    
    while(fgets(line, 1024, fp) != NULL)
    {
        line[strlen(line)-1] = '\0';
        int len1, len2;
        int lineIndex = 0;
        const char wsp[] = " \t";
        char* symbol = malloc(6 * sizeof(char));
        char* opcode = malloc(10 * sizeof(char));

        //printf("BREAK OCCURS BEFORE WHILE\n");
        printf("LINE[%d]: %s\n", lineNumber, line);
        len1 = strcspn(line, wsp);
        strncpy(symbol, line, len1);

        const char *alphaUp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        if (strlen(symbol) > 0 && strcspn(symbol, alphaUp) == 0)
        {
            printf("\tSYMBOL[0 - %d]: %s\n",len1, symbol);
            lineIndex = len1 + 1;
            len1 = strcspn(line + lineIndex, wsp);
            len2 = strspn(line + lineIndex, alphaUp);
            //printf("\tlen1: %d\tlen2: %d\n",len, len2);
            strncpy(opcode, line + lineIndex, len2);
            
            printf("\tOPCODE[%d - %d]: %s\n",lineIndex, lineIndex + len1, opcode);
            //printf("\topcode[%d - %d]: %.*s\n",lineIndex, lineIndex + len, len, line + lineIndex);
            lineIndex += len1 + 1; 
        }
        else
        {
            len1 = strspn(line, wsp);
            len2 = strspn(line + len1, alphaUp);
            lineIndex = len1 + 1;
            strncpy(opcode, line + len1, len2);
            printf("\tOPCODE[%d - %d]: %s\n", lineIndex, lineIndex + len1, opcode);

        }
        lineNumber++;
        //printf("SYMBOL (using .*): %.*s\n", len, line);
    }
    return 1;

}
