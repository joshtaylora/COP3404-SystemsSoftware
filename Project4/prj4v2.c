#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "prj4defs.h"

#ifdef DEBUG
#define LOG_LABEL(x) printf("LABEL: %s\t", x)
#define LOG_OPCODE(x) printf("OPCODE: %s\t", x)
#define LOG_OPERAND(x) printf("OPERAND: %s\t", x)
#define LOG_SYMTAB(x) ST_print(x)
#else
#define LOG_LABEL(x)
#define LOG_OPCODE(x)
#define LOG_OPERAND(x)
#endif

void directiveSymbol(int lineNumber, char* symbol, char* directive, char* operand)
{
    
}

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
    int wspLength;
    int labelLength, opcodeLength, operandLength; // used to parse the line
    int commCheck; // used to check if the line contains a comment
    int errorCode; // will store the error code for printing later
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
        line[strlen(line)] = '\0';
        lineIndex = 0;
        
        if (line[0] == '#') {
            lineNumber += 1;
            continue;
        }
        // --------------------------- Grab the label ---------------------------
        labelLength = strcspn(line, wsp);
        char* label = (char *)calloc(labelLength, sizeof(char));
        strncpy(label, line + lineIndex, labelLength);
        lineIndex += labelLength;
        LOG_LABEL(label);
        // --------------------------- Skip whitespace ---------------------------
        wspLength = strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // --------------------------- Grab the opcode --------------------------- 
        opcodeLength = strcspn(line + lineIndex, wsp);
        char* opcode = (char *)calloc(opcodeLength, sizeof(char));
        strncpy(opcode, line + lineIndex, opcodeLength);
        lineIndex += opcodeLength;
        LOG_OPCODE(opcode);
        // --------------------------- Skip whitespace ---------------------------
        wspLength = strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // --------------------------- Grab the operand ---------------------------
        operandLength = strcspn(line + lineIndex, wsp);
        char* operand = (char *)calloc(operandLength, sizeof(char));
        strncpy(operand, line + lineIndex, operandLength);
        LOG_OPERAND(operand);
        // ------------------------------------------------------------------------
        if (strlen(label) > 6)
        {
            errorCode = 0x00;
            /*
            * TODO create function to print error code
            * @Parameter lineNumber
            * @Parameter line
            * @Parameter label
            */
            exit(1);
        }
        // ---------------------------- Symbol Definition Line --------------------
        if (strlen(label) > 0)
        {
            // 
            // * LOG THE LENGTH OF THE OPCODE
            // * printf("opcode length = %d\n", strlen(opcode));
            // 
            if (isDirective(opcode) > 0)
            {   
                /*
                * LOG THE LINE TYPE
                * printf("DIRECTIVE LINE\n");
                */
                // ---------------------- Start Directive Handler -----------------
                if (strcmp(opcode, "START") == 0)
                {
                    strcpy(programLabel, label);
                    /* 
                    * LOG the programLabel
                    * printf("programLabel: %s\n", programLabel); 
                    */

                    // pointer to store chars not matching ints
                    char* runoff;

                    // convert the address specified in the operand from char* to long int
                    startLoc = strtol(operand, &runoff, 16);
                    // allocate space for the 
                    char* locHex = (char *)calloc(strlen(operand), sizeof(char));
                    sprintf(locHex, "0x%.*X", 6, operand);
                    // printf("%.*X\n", 6, startLoc);

                    // add the start symbol to the symtab
                    ST_set(symtab, programLabel, startLoc, lineNumber);
                }
            }
            else if (OPTAB_Search(optab, opcode) != NULL)
            {
                /*
                * LOG THE LINE TYPE
                printf("OPCODE LINE\n");
                */
            }
        }
        
        // ------------------- Increment the lineNumber counter -------------------
        lineNumber += 1;
        // ------------------------------ Free memory -----------------------------
        free(label);
        free(opcode);
        free(operand);
        printf("\n");
    }
    LOG_SYMTAB(symtab);
    return 0;
}
