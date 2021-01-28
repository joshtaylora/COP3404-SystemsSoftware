#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "prj4defs.h"

#define _CRT_SECURE_NO_WARNINGS

#ifdef DEBUG
#define LOG_LINENUM(x) printf("%2d\t", x)
#define LOG_LABEL(x) printf("%-10s\t", x)
#define LOG_OPCODE(x) printf("%-10s\t", x)
#define LOG_OPERAND(x) printf("%-10s\n", x)
#define LOG_ADDRMODE(x) printf("%d\n", x)
#define LOG_ADDRESS(x) printf("%#08x\t", x)
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

typedef union Line {
    struct InstructionLine* instructionLine;
    struct DirectiveLine* directiveLine;
} Line;

*/

static char tRecord[100];
static int tRecordIndex = 0;
static int tRecordMaxLen = 70;

Line* lineHead = NULL;
Line* prevLine = NULL;
Line* currentLine = NULL;

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
        labelLength = (int)strcspn(line, wsp);
        char* label = (char *)calloc(labelLength, sizeof(char));
        strncpy(label, line + lineIndex, labelLength);
        lineIndex += labelLength;
        // =======================================================================================================
        // ---------------------------------------- Skip whitespace ----------------------------------------------
        wspLength = (int)strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // =======================================================================================================
        // ----------------------------------------- Grab the opcode --------------------------------------------- 
        opcodeLength = (int)strcspn(line + lineIndex, wsp);
        if (opcodeLength == 0) {
            printf("error getting opcode from line\n");
            return 1;
        }
        char* opcode = (char *)calloc(opcodeLength, sizeof(char));
        strncpy(opcode, line + lineIndex, opcodeLength);

        lineIndex += opcodeLength;
        // =======================================================================================================
        // ------------------------------------------- Skip whitespace -------------------------------------------
        wspLength = (int)strspn(line + lineIndex, wsp);
        lineIndex += wspLength;
        // =======================================================================================================
        // -------------------------------------------- Grab the operand -----------------------------------------
        operandLength = (int)strcspn(line + lineIndex+1, "\n");
        char* operand = (char *)calloc(operandLength + 1, sizeof(char));
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
            // if the start directive is not the first directive / instruction in the program this will be true
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
            lineHead = initLineLinkedList(startLine, startLoc, programLabel);
            currentLine = lineHead;
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
                    currentLine->next = addDirectiveToLL(lineNumber, locCounter, label, opcode, operand);

                    printf("\nLine\nlabel: %s\ndirective: %s\noperand: %s\n",
                           currentLine->label,
                           currentLine->directive,
                           currentLine->operand);
                    // Set the previous line to the current line for the next line to use
                    prevLine = currentLine;
                    currentLine = currentLine->next;
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
                        int format4opcodeLen = (int)strlen(opcode) -1;
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
                        currentLine->next = addInstructionToLL(lineNumber, locCounter, label, optabInstr, operand);
                        currentLine = currentLine->next;
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
                        currentLine->next = addInstructionToLL(lineNumber, locCounter, label, tableOp, operand);
                        prevLine = currentLine;
                        currentLine = currentLine->next;

                        ST_set(symtab, label, locCounter, lineNumber);
                    }
                }
            }
            else
            { // case occurs when the line is not a symbol definition
                if (isDirective(opcode) == 1) {
                    // NON-SYMBOL DEF DIRECTIVE LINE
                    if (strcmp(opcode, "BASE") == 0) {
                        int baseVal = locCounter;
                    }
                }
                else if (opcode[0] == '+') {
                    // NON-SYMBOL DEF FORMAT 4 INSTRUCTION LINE

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
                    free(format4opcode);
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
        if (strlen(label) > 0) {
            fprintf(helperFile, "%2d %3s %#08x %3s %-10s %-10s %-s \n", lineNumber, "", locCounter, "", label, opcode, operand);
        }
        else if (strlen(operand) > 0) {
            fprintf(helperFile, "%2d %3s %#08x %3s %-10s %-10s %-s \n", lineNumber, "", locCounter, "", "null", opcode, operand);
        }
        else {

            fprintf(helperFile, "%2d %3s %#08x %3s %-10s %-10s %-s \n", lineNumber, "", locCounter, "", "null", opcode, "null");

        }
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
//    ======================================================================================================
    FILE* pass1;
    pass1 = fopen("intermediate.txt", "r");

    int headerRecordCheck = 0;
    int endRecordCheck = 0;

    char* objFileName = strcat(argv[1], ".obj");
    char* objFilePath = (char *)calloc(80, sizeof(char));
    //char* objFilePath = objStr;
    objFilePath[0] = '.';
    objFilePath[1] = '/';
    strcat(objFilePath, objFileName);

    // create obj file
    FILE* objFile;
    objFile = fopen(objFilePath, "w+");
    if (!objFile) {
        printf("ERROR: file \'%s\' could not be created\n", objFileName);
        return 1;
    }
    // Allocate space for the line
    char* sourceLine = (char *)calloc(1024, sizeof(char));


	char* ptr;
	int p2LocationCounter;
	int programCounter;
	int baseCounter;
    int textRecordLength = 0;
	char* tRecord = (char *)calloc(70, sizeof(char));
    tRecord[69] = '\0';
	int tIndex = 0;

    // Loop through each line of the output file
    while (fgets(sourceLine, 1024, pass1) != NULL) {
        sourceLine[strlen(sourceLine) - 1] = '\0';
        int lineLength = strlen(sourceLine);
        char* workingLine = (char *)calloc(lineLength, sizeof(char));
        strcpy(workingLine, sourceLine);

        int p2LineNum;

    	char* lineNum = (char *)calloc(5, sizeof(char));
    	char* labelStr = (char *)calloc(10, sizeof(char));
    	char* locStr = (char *)calloc(10, sizeof(char));
    	char* opcodeStr = (char *)calloc(10, sizeof(char));
    	char* operandStr = (char *)calloc(80, sizeof(char));
        // ------------------------ Grab the line number -------------------------
		ptr = strtok(workingLine, " ");
		strcpy(lineNum, ptr);
        p2LineNum = strtol(lineNum, NULL, 10);
		LOG_LINENUM(p2LineNum);
        // ------------------------- Grab the location ----------------------------
		ptr = strtok(NULL, " ");
		strcpy(locStr, ptr);
		p2LocationCounter = (int)strtol(locStr, NULL, 16);
		LOG_ADDRESS(p2LocationCounter);
        // --------------------------- Grab the label -----------------------------
		ptr = strtok(NULL, " ");
		strcpy(labelStr, ptr);
		LOG_LABEL(labelStr);
        // -------------------- Grab the instruction/directive --------------------
		ptr = strtok(NULL, " ");
		strcpy(opcodeStr, ptr);
        LOG_OPCODE(opcodeStr);
        // --------------------------- Grab the operand ---------------------------
		ptr = strtok(NULL, "\0");
		wspLength = strspn(ptr, wsp);
		strncpy(operandStr, ptr + wspLength, strlen(ptr + wspLength)-1);
		//printf("\n[%s]\n", operandStr);
		//printf("OPERANDSTR LENGTH: %d\n", strlen(operandStr));
        LOG_OPERAND(operandStr);




        if (strcmp(opcodeStr, "START") == 0)
        {// if the instr/directive in the opcode field is the START directive...
            if (headerRecordCheck != 0)
            {
                errorPrint(sourceLine);
                printf("Line %d: multiple START directives, attempt to create multiple header records canceled.\n", p2LineNum);
                return 1;
            }
            char* headerRecord = (char *)calloc(71, sizeof(char));
            // write header record
            sprintf(headerRecord, "H%-7s%06X%06X", programLabel, startLoc, endLoc-startLoc);
            printf("H-Record: %s\n", headerRecord);
            // update the check notifying the assembler that we have made the header record for the program and that we
            // cannot make another
            headerRecordCheck = 1;
        }
        else if (strcmp(opcodeStr, "END") == 0) {
            char *endRecord = (char *) calloc(71, sizeof(char));
            // write end record
            sprintf(endRecord, "E%06X", firstInstruction);
            printf("E-Record: %s\n", endRecord);
        }
        else
        { // else, we are dealing with a text record
            // create a tes to see if we can fit object code into the current text record
            int testObjCodeLen = 0; // fix later
            // getObjCode(optab, symtab, opcodeStr, operandStr);
//
//            if (tRecordIndex >= tRecordMaxLen)
//            { // occurs when we are at the end of a tRecord
//
//            }
        }
        free(lineNum);
        free(labelStr);
        free(opcodeStr);
        free(operandStr);
        free(workingLine);
    }
    // Terminate program successfully
    return 0;
}
