#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "prj4defs.h"

#ifdef DEBUG
#define LOG_LABEL(x) printf("LABEL: %s", x)
#define LOG_OPCODE(x) printf("OPCODE: %s", x)
#define LOG_OPERAND(x) printf("OPERAND: %s", x)
#else
#define LOG_LABEL(x)
#define LOG_OPCODE(x)
#define LOG_OPERAND(x)
#endif

int main(int argc, char** argv)
{
    // Used to check that the user has provided a SIC/XE file 
    if(argc != 2)
    {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }
    // open the file passed as cmdline arg as a FP
    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    // create an intermediate file for use in assembly process
    FILE *helperFile;
    helperFile = fopen("intermediate.txt", "w+");
    // check that the file passed as input exists
    if (!inputFile)
    {
        printf("ERROR: %s could not be opened for reading\n", argv[1]);
        return 1;
    }
    /*
     * Data area for variable definitions 
     * @symtab = SymbolTable object that is used as the symbol table for all defined symbols
     * @optab = OpcodeTable object that is used as a lookup for the various instructions for SIC/XE
     * @line = buffer to read the lines from the file into
     * @loc_counter = int used to track location of the current instruction/directive
     * 
     */
    SymbolTable* symtab = ST_create();
    OpcodeTable* optab = OPTAB_create();
    char line[1024]; // buffer to read line from file into
    int startCheck = 0; // tracks if the file has used the start directive
    int endCheck = 0; // tracks if the file has used the end directive
    int lineIndex; // tracks the index in the current line being parsed
    int lineNumber = 1; // tracks the line in the file
    int startLoc, endLoc; // track the start and end locations when found
    int firstInstruction = 0; // stores where the first instruction in the program is
    int wspLength, labelLength, opLength, oprndLength; // used to parse the line
    int commCheck; // used to check if the line contains a comment
    char* programLabel = malloc(sizeof(char) * 7); // will store the program label in this area
    const char* wsp = " \t\r";
    const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* comment = "#";
    const char* hex = "0123456789ABCDEF";
    /*
     * Begin parsing the lines in the file
     * fgets returns null when there are no more lines in the file
     */
    while (fgets(line, 1024, inputFile) != NULL)
    {
        line[strlen(line) -1] = '\0';
        lineIndex = 0;
        // count the number of indexes until a comment begins
        commCheck = strspn(line, comment);
        /*
        * if the line does not begin with a comment 
        */
        if (commCheck == 0)
        {
            /*
             * CHECK FOR THE PROGRAM LABEL, START DIRECTIVE, AND START ADDRESS
             */
            labelLength = strspn(line, alpha);
            if (labelLength > 0 && labelLength <= 6)
            {
                char* label = (char*)calloc(labelLength, sizeof(char));
                strncpy(label, line, labelLength);
                LOG_LABEL(label);
                // advance the line index past the length of the label
                lineIndex += labelLength;
                wspLength = strspn(line + lineIndex, wsp); // count the number of whitespace characters
                // advance the index past the white space characters
                lineIndex += wspLength;
                opLength = strspn(line + lineIndex, alpha); // count the number of alpha characters
                if (opLength > 0)
                {
                    // allocate space for the opcode string
                    char* opcode = (char*)calloc(opLength, sizeof(char));
                    // copy the characters from the line to the opcode char pointer
                    strncpy(opcode, line + lineIndex, opLength);
                    // increment index past the opcode characters
                    lineIndex += opLength;
                    LOG_OPCODE(opcode);
                    // check if the opcode is the start directive
                    int cmpStartOpcode = strcmp(opcode, "START");
                    // IF THE FIRST OPCODE IS NOT THE START DIRECTIVE
                    if (cmpStartOpcode != 0)
                    {
                        // TODO make a function that gets passed the line, linenumber, and error code and prints the error
                        printf("ERROR: no start directive\n");
                        return 1;
                    }
                    // count the number of characters used to represent the index
                    oprndLength = strspn(line + lineIndex, hex);
                    // GET THE START ADDRESS & CHECK THAT THE ADDRESS ISNT TOO MANY CHARACTERS
                    if (oprndLength > 0 && oprnLength <= 6)
                    {
                        strncpy();
                    }
                }
            }
            
        }
    }
    return 0;
}
