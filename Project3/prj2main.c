#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "prj2defs.h"

#define OPTAB_SIZE 26

typedef struct Instruction
{
  char* mnemonic;
  int opcode;
} Instruction;

typedef struct OpcodeTable
{
  Instruction** Instructions;
} OpcodeTable;

Instruction* Instruction_Alloc(char* opName, int opHex)
{
    // allocate memory for the instruction we are initializing
    Instruction* opcode = malloc(sizeof(Instruction) * 1);
    // compute the length of the string name passed in
    int len = strlen(opName);
    // allocate memory for the opcode name
    opcode->mnemonic = malloc(len + 1);
    // copy the opcode mnemonic to the Instruction's mnemonic field
    strcpy(opcode->mnemonic, opName);
    // set the opcode field of the Instruction to the value passed as opHex
    opcode->opcode = opHex;

    return opcode;
}

OpcodeTable* OPTAB_create(void)
{
    // allocate memory for the OpcodeTable itself
    OpcodeTable *OPTAB = malloc(sizeof(OpcodeTable) * 1);
    // allocate memory for the size of the array of Instructions
    OPTAB->Instructions = malloc(sizeof(Instruction*) * OPTAB_SIZE);
    
    OPTAB->Instructions[0] = Instruction_Alloc("ADD", 0x18);
    OPTAB->Instructions[1] = Instruction_Alloc("AND", 0x40);
    OPTAB->Instructions[2] = Instruction_Alloc("COMP", 0x28);
    OPTAB->Instructions[3] = Instruction_Alloc("DIV", 0x24);
    OPTAB->Instructions[4] = Instruction_Alloc("J", 0x3C);
    OPTAB->Instructions[5] = Instruction_Alloc("JEQ", 0x30);
    OPTAB->Instructions[6] = Instruction_Alloc("JGT", 0x34);
    OPTAB->Instructions[7] = Instruction_Alloc("JLT", 0x38);
    OPTAB->Instructions[8] = Instruction_Alloc("JSUB", 0x48);
    OPTAB->Instructions[9] = Instruction_Alloc("LDA", 0x00);
    OPTAB->Instructions[10] = Instruction_Alloc("LDCH", 0x50);
    OPTAB->Instructions[11] = Instruction_Alloc("LDL", 0x08);
    OPTAB->Instructions[12] = Instruction_Alloc("LDX", 0x04);
    OPTAB->Instructions[13] = Instruction_Alloc("MUL", 0x20);
    OPTAB->Instructions[14] = Instruction_Alloc("OR", 0x44);
    OPTAB->Instructions[15] = Instruction_Alloc("RD", 0xD8);
    OPTAB->Instructions[16] = Instruction_Alloc("RSUB", 0x4C);
    OPTAB->Instructions[17] = Instruction_Alloc("STA", 0x0C);
    OPTAB->Instructions[18] = Instruction_Alloc("STCH", 0x54);
    OPTAB->Instructions[19] = Instruction_Alloc("STL", 0x14);
    OPTAB->Instructions[20] = Instruction_Alloc("STSW", 0xE8);
    OPTAB->Instructions[21] = Instruction_Alloc("STX", 0x10);
    OPTAB->Instructions[22] = Instruction_Alloc("SUB", 0x1C);
    OPTAB->Instructions[23] = Instruction_Alloc("TD", 0xE0);
    OPTAB->Instructions[24] = Instruction_Alloc("TIX", 0x2C);
    OPTAB->Instructions[25] = Instruction_Alloc("WD", 0xDC);

    return OPTAB;
}

Instruction* OPTAB_Search(OpcodeTable* instructionTable, char* searchName)
{
    int i = 0;
    Instruction* returnOpcode = NULL;
    for (; i < OPTAB_SIZE; i++)
    {
        int comparison = strcmp(instructionTable->Instructions[i]->mnemonic, searchName);
        if (comparison == 0)
        {
            returnOpcode = instructionTable->Instructions[i];
        }
    }
    return returnOpcode;
}

int main(int argc, char *argv[]) {
         
    // check if a file was passed
    if( argc != 2 ) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }
    
    FILE *inputFile;
	inputFile = fopen( argv[1], "r");
    // open the output file for reading and writing
    FILE *outputFile;
    outputFile = fopen("output.txt", "w+");

	// check that the file being read exists (if fopen tries to read from a file that does not exist
	// it returns NULL
	if ( !inputFile ){
		printf("ERROR: %s could not be opened for reading.\n", argv[1]);
		return 1;
	}
    // The Location Counter that tells where we are in memory 
	int loc_counter;
    // initialize the symbol table
    SymbolTable *symbol_table = ST_create();
    /* Commented out because in its current state it is unused
    OpcodeTable* optab = OPTAB_create();
	*/
    //read the file, line by line
	char line[1024];
    // counter that checks if we have encountered the START directive yet
    int startCheck = 0;
    // make sure that there is an END directive (and not multiple ones)
    int endCheck = 0;
    // length of contiguous whitespace, length of token
    int wspLength, tokenLength;
    // int to check if a comment is on a line, int to see how much of a line is not a comment
    int commCheck;
    // index of current position in the line
    int lineIndex;
    // current line number for line we are comparing
    int lineNumber = 1;
    // location counter values for the start and end directives
    int startLoc, endLoc;
    int firstInstruction = 0;
    // allocate memory for a char array that will store the program label
    char* programLabel = malloc(sizeof(char)*6);
    // fgets returns a null pointer if the EOF char is read
	while(fgets(line, 1024, inputFile) != NULL)
    {
        // allocate memory for the token strings
        char* symbol = malloc(6 * sizeof(char));
        char* opcode = malloc(10 * sizeof(char));
        char* operand = malloc(200 * sizeof(char));
        // these are the const char strings used to check for different types of tokens
        const char* wsp = " \t\r";
        const char* alphaUp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const char* comment = "#";
        // end the line with the null character
        line[strlen(line)-1] = '\0';
        // start at the first character in the line
        lineIndex = 0;
        // check for a comment
        commCheck = strspn(line, comment);
        // count the number of contiguous uppercase alpha characters
        tokenLength = strspn(line, alphaUp);
        // copy the symbol name to string called symbol
        strncpy(symbol, line, tokenLength);
        wspLength = strspn(line, wsp);
        // if the line is a symbol definition
        if (strlen(symbol) > 0 && strcspn(symbol, alphaUp) == 0 && commCheck == 0)
        {
            if (isDirective(symbol) == 1)
            {
                errorPrint(line);
                printf("Line %d: symbol (%s) name cannot be a directive name \n", lineNumber, symbol);
                return 1;
            }
            // incrememnt the line index to the element immediately after the last char in the symbol name
            lineIndex += tokenLength + 1;
            // count the number of contiguous whitespace characters before the next nonwhitespace character
            wspLength = strspn(line + lineIndex, wsp);
            // advance the lineIndex to skip the whitespace characters
            lineIndex += wspLength;
            // find the size of the opcode (must be upper case alpha characters)
            tokenLength = strspn(line + lineIndex, alphaUp);
            // copy the corresponding indeces in the line to the string opcode
            strncpy(opcode, line + lineIndex, tokenLength);
            // increment the line
            lineIndex += tokenLength;
            // count whitespace characters after the opcode
            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            // check for end directive in this line
            if (strcmp(opcode, "END") == 0 && endCheck == 0)
            {
                endCheck++;
            }
            else if (strcmp(opcode, "END") == 0 && endCheck > 0)
            {
                errorPrint(line);
                printf("Line %d: multiple END directives encounterd\n", lineNumber);
                return 1;
            }
            // if the directive is the start directive and we haven't already encountered the start directive
            if (startCheck == 0 && strcmp(opcode, "START") == 0)
            {
                tokenLength = strcspn(line + lineIndex, wsp);
                strncpy(operand, line + lineIndex, tokenLength);
                // log that we have found the start directive
                startCheck = 1;
                char hex[15] = "0X";
                strcat(hex, operand);
                loc_counter = (int)strtol(hex, NULL, 0);
                if (loc_counter >= 0x8000)
                {
                    errorPrint(line);
                    printf("Line %d: Starting address (%X) cannot be >= 0x8000\n", lineNumber, loc_counter);
                    return 1;
                }

                // check to see if the symbol is in the table
                Symbol *duplicate = ST_get(symbol_table, symbol);
                if (duplicate != NULL)
                {
                    errorPrint(line);
                    printf("Line %d: duplicate symbol %s already defined on line %d\n", 
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    fprintf(outputFile, "Line %d: duplicate symbol %s already defined on line %d \n", 
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    return 1;
                }
                // copy the program label into the programLabel string variable
                strcpy(programLabel, symbol);
                startLoc = loc_counter;
                // this executes if the symbol is not already in the table
                ST_set(symbol_table, symbol, loc_counter, lineNumber);
                // write the location counter address to the file
                fprintf(outputFile, "%X\t", loc_counter);
                fprintf(outputFile, "%s\t", symbol);
                // write the opcode to the file
                fprintf(outputFile, "%s\t", opcode);
                // write the operand (location counter starting address) to the file
                fprintf(outputFile, "%s\n",operand);
            }
            // check if a second START directive is found
            else if (startCheck >= 1 && strcmp(opcode, "START") == 0)
            {
                errorPrint(line);
                printf("Line %d: more than one START directive in file\n", lineNumber);
                return 1;
            }
            // symbol definition directive line after the START directive line 
            else if (startCheck == 1 && isDirective(opcode) == 1)
            {
                // check specifically for BYTE directive
                if (strcmp(opcode, "BYTE") == 0)
                {
                    if (line[lineIndex] == 'C' || line[lineIndex] == 'X')
                    {
                        if (line[lineIndex + 1] != '\'')
                        {
                            errorPrint(line);
                            printf("Line %d: BYTE directive operand not formatted correctly\n", lineNumber);
                            return 1;
                        }
                        int charLen = strcspn(line + lineIndex + 2, "\'");
                        sprintf(operand, "%.*s", charLen + 3, line + lineIndex);
                        
                        lineIndex += 3;
                        lineIndex += strlen(operand);
                    }
                }
                else 
                {
                    //printf("\toperand line: %s\n", line + lineIndex);
                    tokenLength = strcspn(line + lineIndex, wsp);
                    strncpy(operand, line + lineIndex, tokenLength);
                }
                if (loc_counter > 0x8000)
                {
                    errorPrint(line);
                    printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                    return 1;
                }
                Symbol *duplicate = ST_get(symbol_table, symbol);
                if (duplicate != NULL)
                {
                    errorPrint(line);
                    printf("Line %d:  duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    fprintf(outputFile, "Line %d: duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    return 1;
                }
                // if there is not a duplicate symbol in the symbol table, add the new symbol to the table
                ST_set(symbol_table, symbol, loc_counter, lineNumber);
                fprintf(outputFile, "%X\t", loc_counter);
                // write the symbol to the file
                fprintf(outputFile, "%s\t", symbol);
                // write the opcode to the file
                fprintf(outputFile, "%s\t", opcode);
                // write the operand (location counter starting address) to the file
                fprintf(outputFile, "%s\n", operand);
                //printf("%s\t%X\n", symbol, loc_counter); // comment out for pass 2
                loc_counter = calcDirective(line, opcode, operand, loc_counter, lineNumber);
                if (loc_counter == 0)
                {
                    return 1;
                }
            }
            // if the opcode is an instruction and not a directive
            else if (startCheck == 1 && strlen(opcode) > 0 )
            {
                /*
                printf("opcode name: %s\n", opcode);
                Instruction* currentOpcode = OPTAB_Search(optab, opcode);
                if (currentOpcode != NULL)
                {
                    fprintf(outputFile, "|mnemonic: %s|\t|opcode: %.*X|\n", currentOpcode->mnemonic, 2, currentOpcode->opcode);
                }
                */
                tokenLength = strcspn(line + lineIndex, wsp);
                strncpy(operand, line + lineIndex, tokenLength);
                if (loc_counter > 0x8000)
                {
                    errorPrint(line);
                    printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                    return 1;
                }
                int opcodeHex = opcodeCalc(opcode);
                if (opcodeHex < 0 )
                {
                    errorPrint(line);
                    printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                    return 1;
                }
                 // check if the symbol is already in the table
                Symbol *duplicate = ST_get(symbol_table, symbol);
                // if we found a symbol in the table already with that name in the table, throw an error
                if (duplicate != NULL)
                {
                    errorPrint(line);
                    printf("Line %d: duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    fprintf(outputFile, "Line %d: duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    return 1;
                }
                if (firstInstruction == 0)
                {
                    firstInstruction = loc_counter;
                }
                // if there is not a duplicate symbol in the symbol table, add the new symbol to the table
                ST_set(symbol_table, symbol, loc_counter, lineNumber);
                // write the symbol to the file
                fprintf(outputFile, "%X\t", loc_counter);
                fprintf(outputFile, "%s\t", symbol);
                fprintf(outputFile, "0x%.*X\t", 2, opcodeHex);
                fprintf(outputFile, "%s\n", operand);
                // increment the location counter after adding the the symbol to the table
                loc_counter += 3;
            }
            else if (startCheck == 0)
            {
                errorPrint(line);
                printf("Line %d: START directive not encountered before first directive/instruction\n", 
                        lineNumber);
                fprintf(outputFile, "Line %d: START directive not encountered before first directive/instruction\n", 
                        lineNumber);
                return 1;
            }
            else
            {
                errorPrint(line);
                printf("Line %d: assembly line syntax not valid\n", lineNumber);
                //printf("\tsymbol: %s\n\topcode: %s\n\toperand: %s\n", symbol, opcode, operand);
                fprintf(outputFile, "Line %d: ERROR: assembly line syntax not valid\n", lineNumber);
                return 1;
            }
        }
        // if the line is NOT a symbol definition and NOT a comment line
        else if (commCheck == 0 && startCheck == 1)
        {
            if (loc_counter > 0x8000)
            {
                errorPrint(line);
                printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                return 1;
            }
            // count # of contiguous whitespace characters from starting index of line
            wspLength = strspn(line, wsp);
            lineIndex += wspLength;
            tokenLength = strspn(line + lineIndex, alphaUp);
            strncpy(opcode, line + lineIndex, tokenLength);
            // check for end directive in this line
            if (strcmp(opcode, "END") == 0 && endCheck == 0)
            {
                endCheck++;
            }
            else if (strcmp(opcode, "END") == 0 && endCheck > 0)
            {
                errorPrint(line);
                printf("Line %d: multiple END directives encounterd\n", lineNumber);
                return 1;
            }
            // increment index past opcode
            lineIndex += tokenLength;
            // count the whitespace char's and increment the index past them
            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            // count the number of uppercase alpha characters and copy those characters to a string called operand
            tokenLength = strcspn(line + lineIndex, wsp);
            strncpy(operand, line + lineIndex, tokenLength);

            // if the length of the operand is greater than 0 and there is no whitespace in the operand string
            // -- this happens when we have a non symbol definition 
            if (strlen(opcode) > 0)
            {
                // ensure that the start directive has already been found and that the opcode is a directive
                //  -- happens when we have directive opcode that is not on a symbol definition line
                if (isDirective(opcode) == 1)
                {
                    if (strcmp(opcode, "BYTE") == 0)
                    {
                        if (line[lineIndex] == 'C' || line[lineIndex] == 'X')
                        {
                            if (line[lineIndex + 1] != '\'')
                            {
                                errorPrint(line);
                                printf("Line %d: BYTE directive operand not formatted correctly\n", lineNumber);
                                return 1;
                            }
                            // count the number of char's between the ' and ' char's 
                            int charLen = strcspn(line + lineIndex + 2, "\'");
                            // save the character constant into a the operand string
                            sprintf(operand, "%.*s", charLen + 3, line + lineIndex);
                            //printf("\tpass1 BYTE C operand: %s\n", operand);
                            lineIndex += 3;
                            lineIndex += strlen(operand);
                            
                        }
                    }
                    else 
                    {
                        //printf("\toperand line: %s\n", line + lineIndex);
                        tokenLength = strcspn(line + lineIndex, wsp);
                        strncpy(operand, line + lineIndex, tokenLength);
                    }
                    fprintf(outputFile, "%X\t\t", loc_counter);
                    fprintf(outputFile, "%s\t", opcode);
                    printf("line863: opcode = %s\n", opcode);
                    fprintf(outputFile, "%s\n", operand);
                    // must increase the loc_counter for the line after this line
                    loc_counter = calcDirective(line, opcode, operand, loc_counter, lineNumber);
                }
                // if the opcode token is NOT a directive, increment the location counter by 3 bytes
                else if (isDirective(opcode) == 0)
                {
                    /*
                    Instruction* currentOpcode = OPTAB_Search(optab, opcode);
                    if (currentOpcode != NULL)
                    {
                        fprintf(outputFile, "|mnemonic: %s|\t|opcode: %.*X|\n", currentOpcode->mnemonic, 2, currentOpcode->opcode);
                    }
                    */
                    tokenLength = strcspn(line + lineIndex, wsp);
                    strncpy(operand, line + lineIndex, tokenLength);
                    if (firstInstruction == 0)
                    {
                        firstInstruction = loc_counter;
                    }
                    int opcodeHex = opcodeCalc(opcode);
                    if (opcodeHex < 0 )
                    {
                        errorPrint(line);
                        printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                        return 1;
                    }
                    if (strlen(operand) == 0) // no operand instruction (RSUB) 
                    {
                        fprintf(outputFile, "%X\t\t",loc_counter);
                        fprintf(outputFile, "0x%.*X\t", 2, opcodeHex);
                        fprintf(outputFile, "(null)\n");
                
                        // increment location counter by 3 bytes for the no-operand instruction
                        loc_counter += 3;   
                    }
                    else
                    {
                        fprintf(outputFile, "%X\t\t", loc_counter);
                        fprintf(outputFile, "0x%.*X\t", 2, opcodeHex);
                        fprintf(outputFile, "%s\n", operand);
                        // increment location counter for the next line
                        loc_counter += 3;
                    }
                }
                // if the line 
                else
                {
                    errorPrint(line);
                    printf("Line %d: not a directive or instruction", lineNumber);
                    fprintf(outputFile, "Line %d: not a directive or instruction", lineNumber);

                    return 1;
                }
            }
            // no-operand instruction
            else if (strlen(opcode) > 0)
            {
                if (isDirective(opcode) == 1)
                {
                    errorPrint(line);
                    printf("Line %d: directive does not contain an operand\n", lineNumber);
                    fprintf(outputFile, "Line %d: directiv does not contain an operand\n", lineNumber);
                    return 1;
                }
                int opcodeHex = opcodeCalc(opcode);
                if (opcodeHex < 0 )
                {
                    errorPrint(line);
                    printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                    return 1;
                }
                if (firstInstruction == 0)
                {
                    firstInstruction = loc_counter;
                }
                // print the opcode for the instruction to the file
                fprintf(outputFile, "%X\t\t",loc_counter);
                fprintf(outputFile, "0x%.*X\t", 2, opcodeHex);
                fprintf(outputFile, "(null)\n");
                // increment location counter by 3 bytes for the no-operand instruction
                loc_counter += 3;
            }
            // this happens when only an opcode is given with no operand
            else
            {
                errorPrint(line);
                printf("Line %d: does not contain a symbol, opcode, and operand\n", lineNumber);
                fprintf(outputFile, "Line %d: does not contain a symbol, opcode, and operand\n", lineNumber);

                return 1;
            }
        }
        // this block executes when a comment has been found in a line
        else 
        { 
            fprintf(outputFile, "#\n");
            lineNumber++;
            continue; 
        }
        lineNumber++;
        //printf("\n");
        fprintf(outputFile, "\n");
	}

    ST_print(symbol_table);
    char* checkAddr = malloc(sizeof(char) * 10);
    sprintf(checkAddr, "%X", loc_counter);
    if (loc_counter > 0x8000)
    {
        errorPrint(line);
        printf("ERROR: location counter[%s] goes over maximum hex address\n", checkAddr);
        return 1;
    }
    else
    {
        endLoc = loc_counter;
    }
	// close the opened file
	fclose( inputFile );
    // compute program size
    int programSize = endLoc - startLoc;

    // --------------------------------------------------------------------------------------
    //                                  PASS 2 START
    // --------------------------------------------------------------------------------------
    
    
    
    
    int fileIndex = 1; // create index to track where we are in the file
    int bufferIndex; // create index to track where we are in the line
    char lineBuffer[1024]; // create a buffer to store the characters we read into the line

    char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *wsp = " \t\r\n";

    int headerCheck = 0;
    int endrecordcheck = 0;

    fclose(outputFile);
    outputFile = fopen("output.txt", "r");
    // make the correct output file name
    char* objFilename = strcat(argv[1], ".obj");
    char objStr[60];
    char* objFilePath = objStr;
    

    //printf("FileName: %s\n", objFilename);
    objFilePath[0] = '.';
    objFilePath[1] = '/';
    strcat(objFilePath, objFilename);
    //printf("Filepath: %s\n", objFilePath);

    // open the output file for writing
    FILE* objFile;
    objFile = fopen(objFilename, "w");

    // Loop through the output file line by line loading it into a buffer
    while (fgets(lineBuffer, 1024, outputFile) != NULL)
    {
        // if lineBuffer[0] == '\n' skip the line
        // if lineBuffer[0] == '\t' then the line is not a symbol definition
        // otherwise, lineBuffer[0] will be an alpha character


        bufferIndex = 0;
        char *locChar = malloc(sizeof(char) * 6);
        
        char *optab = malloc(sizeof(char) * 6);
        char *operandtab = malloc(sizeof(char) * 300);
        

        if (lineBuffer[0] == '\n') // skip purely whitespace lines
        {
            continue;
        }
        if (lineBuffer[0] == '#')
        {
            fileIndex++;
            continue;
        }
        
        int hexLocCount = strspn(lineBuffer, "ABCDEF1234567890");
        sprintf(locChar, "0x%.*s", hexLocCount, lineBuffer);
        int locCount = strtol(locChar, NULL, 16);
        
        bufferIndex += strlen(locChar) - 1;
    
        // make the header record
        if (headerCheck == 0)
        {
            int alphaLen = strspn(lineBuffer + bufferIndex, alpha);
            char *labeltab = malloc(sizeof(char) * alphaLen);
            sprintf(labeltab, "%.*s", alphaLen, lineBuffer + bufferIndex);
            
            bufferIndex += strlen(labeltab) + 1;
            alphaLen = strspn(lineBuffer + bufferIndex, alpha);
            sprintf(optab, "%.*s", alphaLen, lineBuffer + bufferIndex);
            
            bufferIndex += alphaLen;
            if (strcmp(optab, "START") == 0)
            {
                char* headerR = malloc(sizeof(char) * 20);
                sprintf(headerR, "H%-6s%06X00%X", labeltab, locCount, programSize);
                //printf("%s\n", headerR);
                fprintf(objFile, "%s\n", headerR);
                free(headerR);
                headerCheck = 1;
            }
            else
            {
                printf("ERROR line %d\n", fileIndex);
                remove(objFilePath);
                return 1;
            }
            
            
        } // symbol definition line
        else if ( headerCheck == 1 && lineBuffer[bufferIndex] != '\t')
        {
            int alphaLen = (int)strspn(lineBuffer + bufferIndex, alpha);
            char *labeltab = malloc(sizeof(char) * alphaLen);
            sprintf(labeltab, "%.*s", alphaLen, lineBuffer + bufferIndex);
            
            alphaLen = 0;
            bufferIndex += strlen(labeltab) + 1;
            alphaLen = (int)strspn(lineBuffer + bufferIndex, alpha);
            if (alphaLen > 1)
            {
                sprintf(optab, "%.*s", alphaLen, lineBuffer + bufferIndex);
                /*
                printf("optab: %s\n", optab);
                */
                if (isDirective(optab) == 1)
                {
                    if (strcmp(optab, "BYTE") == 0)
                    {
                        bufferIndex += 5;
                        if (lineBuffer[bufferIndex] == 'X' && lineBuffer[bufferIndex + 1] == '\'')
                        {
                            bufferIndex += 2;
                            int bytehex = strcspn(lineBuffer + bufferIndex, "\'");
                            sprintf(operandtab, "%.*s", bytehex, lineBuffer + bufferIndex);
                            /*
                            printf("bytehex: %s\n", operandtab);
                            */
                            int bytehexLen = (int)strlen(operandtab) / 2;
                            char *textR = malloc(sizeof(char) * 69);
                            sprintf(textR, "T%06X%.*X%s", locCount, 2, bytehexLen, operandtab);
                            fprintf(objFile, "%s\n", textR);
                            //printf("%s\n", textR);
                            free(textR);
                        }
                        else if (lineBuffer[bufferIndex] == 'C' && lineBuffer[bufferIndex + 1] == '\'')
                        {
                            // skip past the 'C' and '\'' characters
                            bufferIndex += 2;
                            int bytehex = strcspn(lineBuffer + bufferIndex, "\'");
                            sprintf(operandtab, "%.*s", bytehex, lineBuffer + bufferIndex);
                            int optabLen = strlen(operandtab);
                            operandtab[optabLen] = '\0';
                            
                            const char *byteCharHex = convertCharConst(operandtab);

                            bufferIndex += 2 * strlen(operandtab);


                            int bcLen = strlen(byteCharHex);
                            //printf("bcLen: %d\n", bcLen);
                            if (bcLen > 60)
                            {
                                char *textR = malloc(sizeof(char) * 200);
                                char *textR2 = malloc(sizeof(char) * 200);
                                sprintf(textR, "T%06X%.*X%.*s", locCount, 2, (bcLen/2)/2, bcLen/2, byteCharHex);
                                fprintf(objFile, "%s\n", textR);
                                
                                sprintf(textR2, "T%06X%.*X%.*s", locCount + (bcLen/2)/2, 2, (bcLen/2)/2, bcLen/2, byteCharHex + bcLen/2);
                                fprintf(objFile, "%s\n", textR2);

                                //printf("%s\n", textR);
                                //printf("%s\n", textR2);
                                free(textR);
                                free(textR2);
                                //free(labeltab);
                                //free(optab)
                                //free(operandtab)
                                fileIndex++;
                                continue;
                            }
                            else
                            {
                                char *textR = malloc(sizeof(char) * 200);
                                sprintf(textR, "T%06X%.*X%s", locCount, 2, optabLen, byteCharHex);
                                fprintf(objFile, "%s\n", textR);
                                //printf("%s\n", textR);
                                free(textR);
                                fileIndex++;
                                continue;
                            }
                
                        }
                    }
                    else if (strcmp(optab, "WORD") == 0)
                    {
                        bufferIndex += 5; // skip past WORD and the subsequent tab character

                        //printf("WORD: %s\n", lineBuffer + bufferIndex);
                        int wordDLen = strcspn(lineBuffer + bufferIndex, wsp);

                        char *wordChar = malloc(sizeof(char) * 100);
                        sprintf(wordChar, "%.*s", wordDLen, lineBuffer + bufferIndex);
                        //int operandLen = strlen(wordChar) / 2;

                        int wordDec = atoi(wordChar);
                        //printf("wordChar: %s\n", wordChar);
                        char *textR = malloc(sizeof(char) * 200);
                        sprintf(textR, "T%06X03%06X", locCount,  wordDec);
                        fprintf(objFile, "%s\n", textR);
                        //printf("%s\n", textR);
                        free(textR);
                        free(wordChar);
                        //free(labeltab);
                        //free(optab)
                        //free(operandtab)
                        fileIndex++;
                        continue;
                    }
                    // this is for when we define a symbol for the END directive
                    else if (strcmp(optab, "END") == 0 && endrecordcheck == 0)
                    {
                        bufferIndex += 4;
                        int symbolLen = strspn(lineBuffer + bufferIndex, alpha);
                        if (symbolLen == 0)
                        {
                            char *endR = malloc(sizeof(char) * 8);
                            sprintf(endR, "E%06X", firstInstruction);
                            fprintf(objFile, "%s\n", endR);
                            //printf("%s\n", endR);
                            free(endR);
                            fileIndex++;
                            endrecordcheck = 1;
                            continue;
                        }
                        sprintf(operandtab, "%.*s", symbolLen, lineBuffer + bufferIndex);
                        //printf("%s\n",operandtab);
                        //printf("END record symbol: %s\n", operandtab);
                        Symbol *endSym = ST_get(symbol_table, operandtab);
                        // if the symbol is not found in the symbol table
                        if (endSym == NULL)
                        {
                            errorPrint(lineBuffer);
                            printf("Line %d: Symbol (%s) is used but is never defined\n", fileIndex, operandtab);
                            return 1;
                        }
                        char *endR = malloc(sizeof(char) * 8);
                        sprintf(endR, "E%06X", firstInstruction);
                        fprintf(objFile, "%s\n", endR);
                        //printf("%s\n", endR);
                        free(endR);
                        //free(labeltab);
                        //free(optab)
                        //free(operandtab)
                        fileIndex++;
                        endrecordcheck = 1;
                        continue;
                        
                    }
                    else if (strcmp(optab, "END") == 0 && endrecordcheck != 0)
                    {
                        errorPrint(lineBuffer);
                        printf("Line %d: multiple END directives used in program\n", fileIndex);
                        //printf("symbol: %s\topcode: %s\toperand: %s\n", labeltab, optab, operandtab);
                        remove(objFilePath);
                        return 1;
                    }
                    else // the directive is one that does not store a constant
                    {
                        //printf("Directive line -> skip\n");
                        fileIndex++;
                        continue;
                    }
                }
            }
            else // symbol definiton with an instruction for the optab
            {
                //printf("%s", lineBuffer + bufferIndex);
                if (lineBuffer[bufferIndex] == '0')
                {
                    sprintf(optab, "%.*s", 4, lineBuffer+bufferIndex);
                    int opHex = strtol(optab, NULL, 16);
                    //printf("%X\n", opHex);
                    bufferIndex += 5;
                    alphaLen = 0;
                    alphaLen = strspn(lineBuffer + bufferIndex, alpha);
                    if (alphaLen > 1)
                    {
                        sprintf(operandtab, "%.*s", alphaLen, lineBuffer + bufferIndex);
                        bufferIndex += strlen(operandtab);
                        //printf("operand: %s\n", operandtab);
                        Symbol *symbolR = ST_get(symbol_table, operandtab);
                        if (symbolR != NULL)
                        {
                            if (lineBuffer[bufferIndex] == ',')
                            {
                                bufferIndex++;
                                if (lineBuffer[bufferIndex] == 'X')
                                {
                                    //printf("Line %d: INDEXED ADDRESSING\n", fileIndex);
                                    char *textR = malloc(sizeof(char) * 69);
                                    sprintf(textR, "T%06X03%.*X%04X", locCount, 2, opHex, symbolR->Address + 0x8000);
                                    fprintf(objFile, "%s\n", textR);
                                    //printf("%s\n", textR);
                                    free(textR);
                                    //free(labeltab);
                                    //free(optab)
                                    //free(operandtab)
                                    fileIndex++;
                                    continue;
                                }
                            }
                            else
                            {
                                char *textR = malloc(sizeof(char) * 69);
                                sprintf(textR, "T%06X03%.*X%04X", locCount, 2, opHex, symbolR->Address);
                                fprintf(objFile, "%s\n", textR);
                                //printf("%s\n", textR);
                                free(textR);
                                //free(operandtab)
                                //free(labeltab);
                                //free(optab)
                                //free(operandtab)
                                fileIndex++;
                                continue;
                            }
                            
                        }
                        else
                        {
                            errorPrint(lineBuffer);
                            printf("Line %d: Symbol (%s) does not exist in symbol table.\n", fileIndex, operandtab);
                            remove(objFilePath);
                            return 1;
                        }
                        
                    }
                    else
                    {
                        errorPrint(lineBuffer);
                        printf("Line %d: operand is not valid for the %s instruction\n", fileIndex, optab);
                        remove(objFilePath);
                        return 1;
                    }
                    

                }
            }
            
        }
        // this executes for non-symbol definiton lines
        else if (headerCheck == 1 && lineBuffer[bufferIndex] == '\t')
        {
            bufferIndex++;
            int alphaLen = strspn(lineBuffer + bufferIndex, alpha);
            // check for end directive
            if (alphaLen > 1)
            {
                sprintf(optab, "%.*s", alphaLen, lineBuffer + bufferIndex);
                if (isDirective(optab) == 1)
                {
                    if (strcmp(optab, "BYTE") == 0)
                    {
                        bufferIndex += 5;
                        if (lineBuffer[bufferIndex] == 'X' && lineBuffer[bufferIndex + 1] == '\'')
                        {
                            bufferIndex += 2;
                            int bytehex = strcspn(lineBuffer + bufferIndex, "\'");
                            sprintf(operandtab, "%.*s", bytehex, lineBuffer + bufferIndex);
                            /*
                            printf("bytehex: %s\n", operandtab);
                            */
                            int bytehexLen = (int)strlen(operandtab) / 2;
                            char *textR = malloc(sizeof(char) * 69);
                            sprintf(textR, "T%06X%.*X%s", locCount, 2, bytehexLen, operandtab);
                            fprintf(objFile, "%s\n", textR);
                            //printf("%s\n", textR);
                            free(textR);
                        }
                        else if (lineBuffer[bufferIndex] == 'C' && lineBuffer[bufferIndex + 1] == '\'')
                        {
                            // skip past the 'C' and '\'' characters
                            bufferIndex += 2;
                            int bytehex = strcspn(lineBuffer + bufferIndex, "\'");
                            sprintf(operandtab, "%.*s", bytehex, lineBuffer + bufferIndex);
                            int optabLen = strlen(operandtab);
                            operandtab[optabLen] = '\0';
                            
                            const char *byteCharHex = convertCharConst(operandtab);

                            bufferIndex += 2 * strlen(operandtab);


                            int bcLen = strlen(byteCharHex);
                            //printf("bcLen: %d\n", bcLen);
                            if (bcLen > 60)
                            {
                                char *textR = malloc(sizeof(char) * 200);
                                char *textR2 = malloc(sizeof(char) * 200);
                                sprintf(textR, "T%06X%.*X%.*s", locCount, 2, (bcLen/2)/2, bcLen/2, byteCharHex);
                                fprintf(objFile, "%s\n", textR);
                                
                                sprintf(textR2, "T%06X%.*X%.*s", locCount + (bcLen/2)/2, 2, (bcLen/2)/2, bcLen/2, byteCharHex + bcLen/2);
                                fprintf(objFile, "%s\n", textR2);

                                //printf("%s\n", textR);
                                //printf("%s\n", textR2);
                                free(textR);
                                free(textR2);
                                //free(labeltab);
                                //free(optab)
                                //free(operandtab)
                                fileIndex++;
                                continue;
                            }
                            else
                            {
                                char *textR = malloc(sizeof(char) * 200);
                                sprintf(textR, "T%06X%.*X%s", locCount, 2, optabLen, byteCharHex);
                                fprintf(objFile, "%s\n", textR);
                                //printf("%s\n", textR);
                                free(textR);
                                ////free(labeltab);
                                ////free(optab)
                                ////free(operandtab)
                                fileIndex++;
                                continue;
                            }
                
                        }
                    }
                    else if (strcmp(optab, "WORD") == 0)
                    {
                        bufferIndex += 5; // skip past WORD and the subsequent tab character

                        //printf("WORD: %s\n", lineBuffer + bufferIndex);
                        int wordDLen = strcspn(lineBuffer + bufferIndex, wsp);

                        char *wordChar = malloc(sizeof(char) * 100);
                        sprintf(wordChar, "%.*s", wordDLen, lineBuffer + bufferIndex);
                        //int operandLen = strlen(wordChar) / 2;

                        int wordDec = atoi(wordChar);
                        //printf("wordChar: %s\n", wordChar);
                        char *textR = malloc(sizeof(char) * 200);
                        sprintf(textR, "T%06X03%06X", locCount,  wordDec);
                        fprintf(objFile, "%s\n", textR);
                        //printf("%s\n", textR);
                        free(textR);
                        //free(wordChar);
                        //free(labeltab);
                        //free(optab)
                        //free(operandtab)
                        fileIndex++;
                        continue;
                    }
                    // this is for when we define a symbol for the END directive
                    else if (strcmp(optab, "END") == 0 && endrecordcheck == 0)
                    {
                        bufferIndex += 4;
                        int symbolLen = strspn(lineBuffer + bufferIndex, alpha);
                        sprintf(operandtab, "%.*s", symbolLen, lineBuffer + bufferIndex);
                        //printf("END record symbol: %s\n", operandtab);
                        Symbol *endSym = ST_get(symbol_table, operandtab);
                        // if the symbol is not found in the symbol table
                        if (endSym == NULL)
                        {
                            errorPrint(lineBuffer);
                            printf("Line %d: Symbol (%s) is used but is never defined\n", fileIndex, operandtab);
                            return 1;
                        }
                        char *endR = malloc(sizeof(char) * 8);
                        sprintf(endR, "E%06X", firstInstruction);
                        fprintf(objFile, "%s\n", endR);
                        //printf("%s\n", endR);
                        free(endR);
                        //free(labeltab);
                        //free(optab)
                        //free(operandtab)
                        fileIndex++;
                        endrecordcheck = 1;
                        continue;
                        
                    }

                    else if (strcmp(optab, "END") == 0 && endrecordcheck != 0)
                    {
                        errorPrint(lineBuffer);
                        printf("Line %d: multiple END directives used in program\n", fileIndex);
                        //printf("symbol: %s\topcode: %s\toperand: %s\n", labeltab, optab, operandtab);
                        remove(objFilePath);
                        return 1;
                    }
                    else // the directive is one that does not store a constant
                    {
                        //printf("Directive line -> skip\n");
                        fileIndex++;
                        continue;
                    }
                }
            }
            else if (lineBuffer[bufferIndex] == '0')
            {// else if check for the opcode for an instruction
                sprintf(optab, "%.*s", 4, lineBuffer + bufferIndex);
                /*
                printf("optab: %s\n", optab);
                */
                int opHex = (int)strtol(optab, NULL, 16);
                bufferIndex += 5;
                alphaLen = 0;
                alphaLen = strspn(lineBuffer + bufferIndex, alpha);
                if (alphaLen > 1)
                {// beginning of if check for instruction with operand
                    sprintf(operandtab, "%.*s", alphaLen, lineBuffer + bufferIndex);
                    bufferIndex += strlen(operandtab);

                    //printf("operand: %s\n", operandtab);
                    Symbol *symbolR = ST_get(symbol_table, operandtab);
                    if (symbolR != NULL)
                    {
                        if (lineBuffer[bufferIndex] == ',')
                        {// begining of if check for indexed addressing
                            bufferIndex++;
                            if (lineBuffer[bufferIndex] == 'X')
                            {// checks for the X bit saying that the addressing mode is indexed
                                //printf("Line %d: INDEXED ADDRESSING\n", fileIndex);
                                char *textR = malloc(sizeof(char) * 69);
                                sprintf(textR, "T%06X03%.*X%04X", locCount, 2, opHex, symbolR->Address + 0x8000);
                                fprintf(objFile, "%s\n", textR);
                                //printf("%s\n", textR);
                                free(textR);
                                //free(optab)
                                //free(operandtab)
                                fileIndex++;
                                continue;
                            }// end of if checking for the X bit saying that the addressing mode is indexed
                        }// end of if check for indexed addressing
                        else
                        {// executes when the operand is not being addressed using indexed addressing
                            char *textR = malloc(sizeof(char) * 69);
                            sprintf(textR, "T%06X03%.*X%04X", locCount, 2, opHex, symbolR->Address);
                            fprintf(objFile, "%s\n", textR);
                            //printf("%s\n", textR);
                            free(textR);
                            //free(optab)
                            ////free(operandtab)
                            fileIndex++;
                            continue;
                        }// end of else when operand is not addressed using indexed addressing
                    }
                    else
                    {// executes when the symbol used as an operand is not found in the symbol_table
                        errorPrint(lineBuffer);
                        printf("Line %d: symbol (%s) is used but never defined.\n", fileIndex, operandtab);
                        remove(objFilePath);
                        return 1;
                    }
                    
                }// end of if check for instruction with operand
                else
                {// executes when line is a no operand instruction line 
                    sprintf(operandtab, "%.*s", 6, lineBuffer + bufferIndex);
                    /*
                    printf("%s\n", operandtab);
                    */
                    if (strcmp(operandtab, "(null)") == 0)
                    {
                        bufferIndex+=   6;
                        char *textR = malloc(sizeof(char) * 69);
                        sprintf(textR, "T00%X03%X0000", locCount, opHex);
                        fprintf(objFile, "%s\n", textR);
                        //printf("%s\n", textR);
                        free(textR);
                        //free(operandtab)
                        fileIndex++;
                        continue;
                    }
                    else
                    {
                        errorPrint(lineBuffer);
                        printf("Line %d: instruction (%06X) does not have the correct operand type (%s)\n", fileIndex, opHex, operandtab);
                        remove(objFilePath);
                        return 1;
                    }
                    free(operandtab);
                    free(optab);
                }// end of else block for no operand instruction

            }
            
        }// end of if/else control block for each line
    }// end of while loop through intermediate file
    fclose(objFile);
    fclose(outputFile);
    return 0; 
    
}
