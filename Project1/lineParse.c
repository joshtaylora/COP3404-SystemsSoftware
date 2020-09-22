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
        int wspLength, tokenLength;
        int commCheck, notComm;
        int lineIndex = 0;
        char* symbol = malloc(6 * sizeof(char));
        char* opcode = malloc(10 * sizeof(char));
        char* operand = malloc(10 * sizeof(char));

        const char *wsp = " \t";
        const char *alphaUp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const char *comment = "#";

        /*
        printf("------------------------------------------------------------------------\n");
        for (int i = 0; i < strlen(line); i++)
        {
            printf("LINE[%d], char[%i]: %c\n", lineNumber, i, line[i]);
        }
        printf("\n");
        printf("------------------------------------------------------------------------\n");
        */
        
        //printf("BREAK OCCURS BEFORE WHILE\n");
        printf("---------------- LINE[%d] ----------------\n", lineNumber);
        printf("%s\n", line);
        wspLength = strspn(line, alphaUp);
        strncpy(symbol, line, wspLength);

        commCheck = strspn(line, comment);


        if (strlen(symbol) > 0 && strcspn(symbol, alphaUp) == 0 && commCheck == 0)
        {
            printf("\tSYMBOL[0-%d]:\t%s\tlength = %d\n",wspLength -1, symbol, (int)strlen(symbol));
            /*
            lineIndex += wspLength + 1;
            wspLength = strcspn(line + lineIndex, wsp);
            */
            // advance the index in the line to the index right after the symbol we just found
            lineIndex += wspLength + 1;
            
            // find the size of the substring in the line that is made up of entirely whitespace characters
            // ------ we will advance the lineIndex counter this many indeces to get to the next char we want to check
            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            
            // find the size of the opcode (must be upper case alpha characters)
            tokenLength = strspn(line + lineIndex, alphaUp);
            // copy the opcode characters to a string called opcode
            strncpy(opcode, line + lineIndex, tokenLength);
            
            printf("\tOPCODE[%d-%d]:\t%s\tlength = %d\n",lineIndex, lineIndex + tokenLength - 1, opcode, (int)strlen(opcode));
            //printf("\topcode[%d - %d]: %.*s\n",lineIndex, lineIndex + len, len, line + lineIndex);
            // advance the lineIndex to the position just after the last char of opcode in line
            lineIndex += tokenLength + 1;
            
            // count the whitespace chars between the opcode and the operand
            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            tokenLength = strcspn(line + lineIndex, wsp);
            
            strncpy(operand,line + lineIndex, tokenLength);
            printf("\tOPERAND[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, operand, (int)strlen(operand));


            printf("\n");
        }
        else if (commCheck == 0)
        {
            // find out the number of chars in line before we get to a non whitespace char
            wspLength = strspn(line, wsp);
            // increment the lineIndex to just after the white space characters
            lineIndex += wspLength;
            // find out the size of the substring consisting of chars in line from line[wspLength] onward 
            //      that are all capital letters
            tokenLength = strspn(line + lineIndex, alphaUp);

            strncpy(opcode, line + wspLength, tokenLength);
            printf("\tOPCODE[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, opcode, (int)strlen(opcode));
            
            // incrememnt lineIndex after the opcode token
            lineIndex += tokenLength + 1;
            wspLength = strspn(line + lineIndex, wsp);
            // incrememnt lineIndex past the whitespace chars in line after the opcode
            lineIndex += wspLength;
            tokenLength = strcspn(line + lineIndex, wsp);

            strncpy(operand, line + lineIndex, tokenLength);
            if (strlen(operand) > 0 && strspn(operand, wsp) <= 0 )
            {
                printf("\tOPERAND[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, operand, (int)strlen(operand));
            }

        }
        // this block executes when a comment has been found in a line
        else 
        { 
            printf("\tcommCheck = %d", commCheck);
            notComm = strcspn(line, comment);
            printf("\t[%d]\t%.*s\n",notComm, notComm, line);
            printf("-----was comment\n");
        }
        lineNumber++;
        printf("\n");
        //printf("SYMBOL (using .*): %.*s\n", len, line);
    }
    return 1;

}
