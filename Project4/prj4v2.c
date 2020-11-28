#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "prj4defs.h"

#define _CRT_SECURE_NO_WARNINGS

#ifdef DEBUG
#define LOG_LINENUM(x) printf("%d\t", x)
#define LOG_LABEL(x) printf("%s\t", x)
#define LOG_OPCODE(x) printf("%s\t", x)
#define LOG_OPERAND(x) printf("%s\t", x)
#define LOG_ADDRMODE(x) printf("%d\n", x)
#define LOG_ADDRESS(x) printf("%#08X", x)
#define LOG_SYMTAB(x) ST_print(x)
#else
#define LOG_LINENUM(x)
#define LOG_LABEL(x)
#define LOG_OPCODE(x)
#define LOG_OPERAND(x)
#define LOG_ADDRMODE(x)
#define LOG_ADDRESS(x)
#define LOG_SYMTAB(x)
#endif
/*
void directiveSymbol(int lineNumber, char* symbol, char* directive, char* operand)
{
    
}
*/

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

    /**
    * Data area for variable definitions 
    * @symtab = SymbolTable object that is used as the symbol table for all defined symbols
    * @optab = OpcodeTable object that is used as a lookup for the various instructions for SIC/XE
    * @line = buffer to read the lines from the file into
    * @loc_counter = int used to track location of the current instruction/directive
    * 
    **/
    SymbolTable* symtab = ST_create();
    OpcodeTable* optab = OPTAB_create();
    AddrModeTable* addrmodetab = AMT_create();

    char line[1024]; // buffer to read line from file into
    int startCheck = 0; // tracks if the file has used the start directive
    int endCheck = 0; // tracks if the file has used the end directive
    int lineIndex; // tracks the index in the current line being parsed
    int startLine;
    int endLine;
    int lineNumber = 1; // tracks the line in the file
    int startLoc, endLoc; // track the start and end locations when found
    int firstInstruction = 0; // stores where the first instruction in the program is
    int locCounter = 0;
    int instrFormat = 0;
    int newLocCounter;
    int wspLength;
    int labelLength, opcodeLength, operandLength; // used to parse the line
    int commCheck; // used to check if the line contains a comment
    int errorCode; // will store the error code for printing later
    char* programLabel = (char *)calloc(7, sizeof(char)); // will store the program label in this area
    char* endLabel = (char *)calloc(7, sizeof(char));
    const char* wsp = " \t\r\n";
    const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* comment = "#";
    const char* hex = "0123456789ABCDEF";
    /*
     * Begin parsing the lines in the file
     * fgets returns null when there are no more lines in the file
     */
    while (fgets(line, 1024, inputFile) != NULL) {
        // set index in the line to 0
        lineIndex = 0;
        newLocCounter = locCounter;
        if (line[0] == '#') {
            lineNumber += 1;
            continue;
        }
        // =======================================================================================================
        // --------------------------------------- Grab the label ------------------------------------------------
        labelLength = strcspn(line, wsp);
        char* label = (char *)calloc(labelLength, sizeof(char));
        strncpy(label, line + lineIndex, labelLength);
        lineIndex += labelLength;
        // =======================================================================================================
        // ---------------------------------------- Skip whitespace ----------------------------------------------
        wspLength = strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // =======================================================================================================
        // ----------------------------------------- Grab the opcode --------------------------------------------- 
        opcodeLength = strcspn(line + lineIndex, wsp);
        char* opcode = (char *)calloc(opcodeLength, sizeof(char));
        strncpy(opcode, line + lineIndex, opcodeLength);

        lineIndex += opcodeLength;
        // =======================================================================================================
        // ------------------------------------------- Skip whitespace -------------------------------------------
        wspLength = strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // =======================================================================================================
        // -------------------------------------------- Grab the operand -----------------------------------------
        operandLength = strcspn(line + lineIndex+1, wsp)+1;
        char* operand = (char *)calloc(operandLength, sizeof(char));
        strncpy(operand, line + lineIndex, operandLength);
        // =======================================================================================================

        if (strlen(label) > 6) {
            errorCode = 0x00;
            /*
            * TODO create function to print error code
            * @Parameter lineNumber
            * @Parameter line
            * @Parameter label
            */
            exit(1);
        }
        // Check to ensure that the START directive occurs before all other instructions/directives
        if (startCheck == 0) {
            int startDirCheck = strcmp(opcode, "START");
            if (startDirCheck != 0) {
                // throw an error and terminate execution
                errorPrint(line);
                printf("Line %d: error, first item in instruction/directive area (%s) not the START directive\n",
                       lineNumber, opcode);
                return 1;
            }
            // START directive successfully found
            startCheck = 1;
            startLine = lineNumber;
            // copy the label field into the programLabel string that will be used in pass 2
            strcpy(programLabel, label);

            // pointer to store chars not matching ints
            char* runoff;
            // convert the address specified in the operand from char* to long int
            int tempLoc = strtol(operand, &runoff, 10);
            // allocate space for the location counter string
            char* locHex = (char *)calloc(strlen(operand) + 2, sizeof(char));
            sprintf(locHex, "%#06X", tempLoc);

            // initialize the location counter to the starting address
            locCounter = (int)strtol(locHex, NULL, 16);
            if (isDirective(label) == 1) {
                errorPrint("Line %d: ");
                printf("Line %d: symbol cannot have name of directive %s\n", lineNumber, label);
            }
            ST_set(symtab, label, locCounter, lineNumber);
            startLoc = locCounter;
        }
        else {
            if (strcmp(opcode, "START") == 0) {
                // Multiple START directives encountered, need throw an error and terminate
                errorPrint(line);
                printf("Line %d: multiple start directive found\n First occurrence on line (%d)\n",
                       lineNumber, startLine);
                return 1;
            }
            else if (strcmp(opcode, "END") == 0) {
                if (endCheck != 0) {
                    // Multiple END directives, need to throw an error and terminate
                    errorPrint(line);
                    printf("Line %d: multiple END directives found\n First occurrence on line (%d)\n",
                    lineNumber, endLine);
                    return 1;
                }
                endCheck = 1;
                endLine = lineNumber;
                endLoc = locCounter;
            }
            else if (strlen(label) > 0) {
                // ---------------------------- Symbol Definition Line --------------------

                // check if the label has the name of a directive
                if (isDirective(label) == 1) {
                    errorPrint(line);
                    printf("Line %d: label (%s) cannot be the name of a directive\n", lineNumber, label);
                    return 1;
                }
                if (isDirective(opcode) == 1) {
                    // Directive line
                    if (strcmp(opcode, "BYTE") == 0) {
                        // if the operand of BYTE directive is not formatted properly, throw an error and terminate
                        if ( (operand[0] != 'C') && (operand[0] != 'X') ) {
                            errorPrint(line);
                            printf("Line %d: improper use of BYTE directive using operand %s\n", lineNumber, operand);
                            return 1;
                        }
                    }
                    if (locCounter > 0x100000) {
                        // Throw an error because we are out of the memory bounds of the SIC/XE system
                        errorPrint(line);
                        printf("Line %d: location counter exceeded available memory for the system: %#06X\n",
                               lineNumber, locCounter);
                        return 1;
                    }
                    // check for duplicate symbol before inserting into the symbol table
                    Symbol* duplicate = ST_get(symtab, label);
                    if (duplicate != NULL){
                        // duplicate symbol definition error, throw error and terminate
                        errorPrint(line);
                        printf("Line %d: duplicate symbol %s , defined previously on line %d\n",
                               lineNumber, label, duplicate->SourceLineNumber);
                    }
                    // add the symbol to the symbol table
                    ST_set(symtab, label, locCounter, lineNumber);
                    // increment the location counter
                    newLocCounter = calcDirective(line, opcode, operand, locCounter, lineNumber);
                }
                else {
                    // Instruction Line
                    // ===================================================================================================
                    // -------------------------------------- Format 4 Instruction ---------------------------------------
                    // ===================================================================================================
                    if (opcode[0] == '+') {
                        // get the length of the opcode string minus the + symbol
                        int format4opcodeLen = strlen(opcode) -1;
                        // allocate space for the new string and copy the opcode into it without the + symbol
                        char *format4opcode = (char *) calloc(format4opcodeLen, sizeof(char));
                        strncpy(format4opcode, opcode + 1, format4opcodeLen);

                        Instruction *optabInstr = OPTAB_Search(optab, format4opcode);
                        if (optabInstr == NULL) {
                            errorPrint(line);
                            printf("Line %d: instruction mnemonic (%s) not recognized \n", lineNumber, format4opcode);
                            return 1;
                        }
                        Symbol* duplicate = ST_get(symtab, label);
                        if (duplicate != NULL) {
                            errorPrint(line);
                            printf("Line %d: duplicate symbol encountered. First occurrence on line %d\n",
                                   lineNumber, duplicate->SourceLineNumber);
                            return 1;
                        }
                        // log the location counter for the first instruction for use in Pass 2
                        if (firstInstruction == 0) {
                            firstInstruction = locCounter;
                        }
                        ST_set(symtab, label, locCounter, lineNumber);
                        // Increment location counter by 4 bytes
                        newLocCounter += 4;

                    }
                    else {
                        Instruction *tableOp = OPTAB_Search(optab, opcode);
                        // make sure the instruction is valid
                        if (tableOp == NULL) {
                            errorPrint(line);
                            printf("Line %d: instruction mnemonic (%s) not recognized \n", lineNumber, opcode);
                            return 1;
                        }

                        if (tableOp->format[0] == 1) {
                            newLocCounter += 1;
                        }
                        else if (tableOp->format[0] == 2) {
                            newLocCounter += 2;
                        }
                        else if (tableOp->format[0] == 3) {
                            newLocCounter += 3;
                        }
                        else {
                            printf("Assembler error, process terminating.\n");
                            return 1;
                        }
                        Symbol* duplicate = ST_get(symtab, label);
                        if (duplicate != NULL) {
                            errorPrint(line);
                            printf("Line %d: duplicate symbol \'%s\' defined. First defined on line %d\n",
                                   lineNumber, label, duplicate->SourceLineNumber);
                            return 1;
                        }
                        ST_set(symtab, label, locCounter, lineNumber);
                    }
                }
            }
            else {
                if (isDirective(opcode) == 1) {
                    if (strcmp(opcode, "BASE") == 0) {
                        int baseVal = locCounter;
                    }
                }
                else if (opcode[0] == '+') {
                    // get the length of the opcode string minus the + symbol
                    int format4opcodeLen = strlen(opcode) -1;
                    // allocate space for the new string and copy the opcode into it without the + symbol
                    char *format4opcode = (char *) calloc(format4opcodeLen, sizeof(char));
                    strncpy(format4opcode, opcode + 1, format4opcodeLen);

                    Instruction *optabInstr = OPTAB_Search(optab, format4opcode);
                    if (optabInstr == NULL) {
                        errorPrint(line);
                        printf("Line %d: instruction mnemonic (%s) not recognized \n", lineNumber, format4opcode);
                        return 1;
                    }
                    // log the location counter for the first instruction for use in Pass 2
                    if (firstInstruction == 0) {
                        firstInstruction = locCounter;
                    }
                    // Increment location counter by 4 bytes
                    newLocCounter += 4;

                }
                else {
                    Instruction *tableOp = OPTAB_Search(optab, opcode);
                    // make sure the instruction is valid
                    if (tableOp == NULL) {
                        errorPrint(line);
                        printf("Line %d: instruction mnemonic (%s) not recognized \n", lineNumber, opcode);
                        return 1;
                    }

                    if (tableOp->format[0] == 1) {
                        newLocCounter += 1;
                    }
                    else if (tableOp->format[0] == 2) {
                        newLocCounter += 2;
                    }
                    else if (tableOp->format[0] == 3) {
                        newLocCounter += 3;
                    }
                    else {
                        printf("Assembler error, process terminating.\n");
                        return 1;
                    }
                }
            }
        }
        fprintf(helperFile, "[%d] [%#08x] [%s] [%s] [%s] \n", lineNumber, locCounter, label, opcode, operand);

        // ------------------- Increment the lineNumber counter -------------------
        lineNumber += 1;
        locCounter = newLocCounter;
        // ------------------------------ Free memory -----------------------------
        free(label);
        free(opcode);
        free(operand);
    }
    // close input file stream
    //fclose(inputFile);
    // close intermediate file writing stream
    fclose(helperFile);

//    ======================================================================================================
//    -------------------------------------------- Pass 2 --------------------------------------------------
    FILE* pass1;
    pass1 = fopen("intermediate.txt", "r");

    int headerCheck = 0;
    int endRecordCheck = 0;

    char* objFileName = strcat(argv[1], ".obj");
    char objStr[60];
    char* objFilePath = objStr;
    objFilePath[0] = '.';
    objFilePath[1] = '/';
    strcat(objFilePath, objFileName);

    // create obj file
    FILE* objFile;
    objFile = fopen(objFilePath, "w");
    if (!objFile) {
        printf("ERROR: file \'%s\' could not be created\n", objFileName);
    }
    // Allocate space for the line
    char* sourceLine = (char *)calloc(1024, sizeof(char));

    char tstart = '[';
    char tend = ']';

	char* ptr;
    // Loop through each line of the output file
    while (fgets(sourceLine, 1024, pass1) != NULL) {
        int lineLength = strlen(sourceLine);
        char* workingLine = (char *)calloc(lineLength, sizeof(char));
        strcpy(workingLine, sourceLine);
    	char lineNum[5], labelStr[10], opcodeStr[10], operandStr[80];

		ptr = strtok(workingLine, "[]"); // removes first [
		ptr = strtok(workingLine, "[]"); // gets string from after [ to ]
		strcpy(lineNum, ptr);
		ptr = strtok(workingLine, "[]");
		ptr = strtok(workingLine, "[]");
		if (ptr != NULL) {
			printf("label: %s\n", ptr);
		}
    }


    // check for BASE directive to establish Base Relative Addressing


    LOG_SYMTAB(symtab);
    return 0;
}
