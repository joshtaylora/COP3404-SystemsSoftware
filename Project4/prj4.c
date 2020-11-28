#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "prj4defs.h"

#ifdef DEBUG
#define LOG_LABEL(x) fprintf(helperFile, "LABEL: %s\n", x)
#define LOG_OPCODE(x) fprintf(helperFile, "OPCODE: %s\n", x)
#define LOG_OPERAND(x) fprintf(helperFile, "OPERAND: %s\n", x)
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
    int wspLength, labelLength, opcodeLength, operand1Length, operand2Length; // used to parse the line
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
            labelLength = strspn(line, alpha);
            if (labelLength > 0 && labelLength <= 6)
            {
                /*
                * Get the label value
                */ 
                char* label = (char*)calloc(labelLength, sizeof(char));
                strncpy(label, line, labelLength);
                LOG_LABEL(label);
                // advance the line index past the length of the label
                lineIndex += labelLength;
                wspLength = strspn(line + lineIndex, wsp); // count the number of whitespace characters
                // advance the index past the white space characters
                lineIndex += wspLength;
                /*
                * Get the opcode value
                */ 
                opcodeLength = strspn(line + lineIndex, alpha);
                if (opcodeLength > 0 && opcodeLength <= 6)
                {
                    char* opcode = (char*)calloc(opcodeLength, sizeof(char)); // allocate space for the opcode string
                    strncpy(opcode, line + lineIndex, opcodeLength); // copy the characters to the char*
                    LOG_OPCODE(opcode);
                    lineIndex += opcodeLength;
                    // count the whitespace characters and increment index past them in the line
                    wspLength = strspn(line + lineIndex, wsp);
                    lineIndex += wspLength;

                    /*
                    * Get the operand value
                    */
                    operand1Length = strcspn(line + lineIndex, alpha);
                    if (operand1Length > 0)
                    {
                        char* operand1 = (char*)calloc(operand1Length, sizeof(char));
                        strncpy(operand1, line + lineIndex, operand1Length);
                        LOG_OPERAND(operand1);
                        if (line[lineIndex + 1] == ',')
                        {
                            // this means the line contains multiple operands
                            lineIndex += 1;
                            wspLength = strspn(line + lineIndex, wsp);
                            lineIndex += wspLength;
                            operand2Length = strcspn(line + lineIndex, wsp);
                            if (operand2Length > 0)
                            {
                                char* operand2 = (char*)calloc(operand2Length, sizeof(char));
                                strncpy(operand2, line + lineIndex, operand2Length);
                                LOG_OPERAND(operand2);
                            }
                        }
                    }
                }
            }
        }
        // Increment the lineNumber counter
        lineNumber += 1;
    }
    return 0;
}
