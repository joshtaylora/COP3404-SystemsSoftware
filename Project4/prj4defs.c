#ifdef ASSEMBLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "prj4defs.h"

#define SYMTAB_SIZE 26
#define OPTAB_SIZE 59

Line* initLineLinkedList(int lineNum, int loc, char* programLabel)
{ // used to initialize the first line in the line linked list for the initial START directive
    /*
     * line types:
     * D = Directive
     * I = Instruction
     */
    char startDirective[]= "START";

    Line* startLine = (Line *)calloc(1, sizeof(Line));

    startLine->lineType = 'D';
    startLine->lineNumber = lineNum;
    startLine->location = loc;

    startLine->label = (char *)calloc(strlen(programLabel), sizeof(char));
    strcpy(startLine->label, programLabel);

    startLine->directive = (char *)calloc(strlen("START")  + 1, sizeof(char));
    strcpy(startLine->directive, startDirective);

    startLine->operand = NULL;
    startLine->objCode = NULL;
    startLine->next = NULL;
    return startLine;
}

Line* addDirectiveToLL(int lineNum, int loc, char* label, char* directive, char* operand)
{
    Line* entry = (Line *)calloc(1, sizeof(Line));
    entry->lineType = 'D';
    entry->lineNumber = lineNum;
    entry->location = loc;
    entry->label = (char *)calloc(strlen(label), sizeof(char));
    entry->directive = (char *)calloc(strlen(directive), sizeof(char));
    entry->operand = (char *)calloc(strlen(operand), sizeof(char));
    // set the next entry to NULL
    entry->next = NULL;
    // copy all of the string elements to their respective fields in the line
    strcpy(entry->label, label);
    strcpy(entry->directive, directive);
    strcpy(entry->operand, operand);
    return entry;
}

Line* addInstructionToLL(int lineNum, int loc, char* label, Instruction* instruction, char* operand)
{
    Line* entry = (Line *)calloc(1, sizeof(Line));
    entry->lineType = 'I';
    entry->lineNumber = lineNum;
    entry->location = loc;
    entry->label = (char *)calloc(strlen(label), sizeof(char));
    entry->instruction = instruction;
    entry->operand = (char *)calloc(strlen(operand), sizeof(char));
    // set the next entry to NULL
    entry->next = NULL;
    // copy all of the string elements to their respective fields in the line
    strcpy(entry->label, label);
    strcpy(entry->operand, operand);
    return entry;
}

Instruction* Instruction_Alloc(char* opName, int opHex, int* formats)
{
    // allocate memory for the instruction we are initializing
    Instruction* opcode = malloc(sizeof(Instruction) * 1);
    // compute the length of the string name passed in
    int len = (int)strlen(opName);
    // allocate memory for the opcode name
    opcode->mnemonic = (char *)calloc(len + 1, sizeof(char));
    // copy the opcode mnemonic to the Instruction's mnemonic field
    strcpy(opcode->mnemonic, opName);
    // set the opcode field of the Instruction to the value passed as opHex
    opcode->opcode = opHex;
    opcode->format = (int *)calloc(sizeof(formats)/sizeof(int), sizeof(int));
    opcode->format = formats;

    return opcode;
}

OpcodeTable* OPTAB_create(void)
{
    // allocate memory for the OpcodeTable itself
    OpcodeTable *OPTAB = calloc(1, sizeof(OpcodeTable));
    // allocate memory for the size of the array of Instructions
    OPTAB->Instructions = calloc(OPTAB_SIZE, sizeof(Instruction*));
    
    int* form1 = (int *)calloc(1, sizeof(int));
    form1[0] = 1;
    int* form2 = (int*)calloc(1, sizeof(int));
    form2[0] = 2;
    int* form3and4 = (int *)calloc(2, sizeof(int));
    form3and4[0] = 3;
    form3and4[1] = 4;


    OPTAB->Instructions[0] = Instruction_Alloc( "ADD",  0x18,   form3and4);
    OPTAB->Instructions[1] = Instruction_Alloc( "AND",  0x40,   form3and4);
    OPTAB->Instructions[2] = Instruction_Alloc( "COMP", 0x28,   form3and4);
    OPTAB->Instructions[3] = Instruction_Alloc( "DIV",  0x24,   form3and4);
    OPTAB->Instructions[4] = Instruction_Alloc( "J",    0x3C,   form3and4);
    OPTAB->Instructions[5] = Instruction_Alloc( "JEQ",  0x30,   form3and4);
    OPTAB->Instructions[6] = Instruction_Alloc( "JGT",  0x34,   form3and4);
    OPTAB->Instructions[7] = Instruction_Alloc( "JLT",  0x38,   form3and4);
    OPTAB->Instructions[8] = Instruction_Alloc( "JSUB", 0x48,   form3and4);
    OPTAB->Instructions[9] = Instruction_Alloc( "LDA",  0x00,   form3and4);
    OPTAB->Instructions[10] = Instruction_Alloc("LDCH", 0x50,   form3and4);
    OPTAB->Instructions[11] = Instruction_Alloc("LDL",  0x08,   form3and4);
    OPTAB->Instructions[12] = Instruction_Alloc("LDX",  0x04,   form3and4);
    OPTAB->Instructions[13] = Instruction_Alloc("MUL",  0x20,   form3and4);
    OPTAB->Instructions[14] = Instruction_Alloc("OR",   0x44,   form3and4);
    OPTAB->Instructions[15] = Instruction_Alloc("RD",   0xD8,   form3and4);
    OPTAB->Instructions[16] = Instruction_Alloc("RSUB", 0x4C,   form3and4);
    OPTAB->Instructions[17] = Instruction_Alloc("STA",  0x0C,   form3and4);
    OPTAB->Instructions[18] = Instruction_Alloc("STCH", 0x54,   form3and4);
    OPTAB->Instructions[19] = Instruction_Alloc("STL",  0x14,   form3and4);
    OPTAB->Instructions[20] = Instruction_Alloc("STSW", 0xE8,   form3and4);
    OPTAB->Instructions[21] = Instruction_Alloc("STX",  0x10,   form3and4);
    OPTAB->Instructions[22] = Instruction_Alloc("SUB",  0x1C,   form3and4);
    OPTAB->Instructions[23] = Instruction_Alloc("TD",   0xE0,   form3and4);
    OPTAB->Instructions[24] = Instruction_Alloc("TIX",  0x2C,   form3and4);
    OPTAB->Instructions[25] = Instruction_Alloc("WD",   0xDC,   form3and4);
    // SICXE Instructions
    OPTAB->Instructions[26] = Instruction_Alloc("ADDF",     0x58,   form3and4);
    OPTAB->Instructions[27] = Instruction_Alloc("ADDR",     0x90,   form2);
    OPTAB->Instructions[28] = Instruction_Alloc("CLEAR",    0xB4,  form2);
    OPTAB->Instructions[29] = Instruction_Alloc("COMPF",    0x88,  form3and4);
    OPTAB->Instructions[30] = Instruction_Alloc("COMPR",    0xA0,  form2);
    OPTAB->Instructions[31] = Instruction_Alloc("DIVF",     0x64,   form3and4);
    OPTAB->Instructions[32] = Instruction_Alloc("DIVR",     0x9C, form2);
    OPTAB->Instructions[33] = Instruction_Alloc("FIX",      0xC4, form1);
    OPTAB->Instructions[34] = Instruction_Alloc("FLOAT",    0xC0, form1);
    OPTAB->Instructions[35] = Instruction_Alloc("HIO",      0xF4, form1);
    OPTAB->Instructions[36] = Instruction_Alloc("LDB",      0x68, form3and4);
    OPTAB->Instructions[37] = Instruction_Alloc("LDF",      0x70, form3and4);
    OPTAB->Instructions[38] = Instruction_Alloc("LDS",      0x6C, form3and4);
    OPTAB->Instructions[39] = Instruction_Alloc("LDT",      0x74, form3and4);
    OPTAB->Instructions[40] = Instruction_Alloc("LPS",      0xD0, form3and4);
    OPTAB->Instructions[41] = Instruction_Alloc("MULF", 0x60, form3and4);
    OPTAB->Instructions[42] = Instruction_Alloc("MULR", 0x98, form2);
    OPTAB->Instructions[43] = Instruction_Alloc("NORM", 0xC8, form1);
    OPTAB->Instructions[44] = Instruction_Alloc("RMO", 0xAC, form2);
    OPTAB->Instructions[45] = Instruction_Alloc("SHIFTL", 0xA4, form2);
    OPTAB->Instructions[46] = Instruction_Alloc("SHIFTR", 0xA8, form2);
    OPTAB->Instructions[47] = Instruction_Alloc("SIO", 0xF0, form1);
    OPTAB->Instructions[48] = Instruction_Alloc("SSK", 0xEC, form3and4);
    OPTAB->Instructions[49] = Instruction_Alloc("STB", 0x78, form3and4);
    OPTAB->Instructions[50] = Instruction_Alloc("STF", 0x80, form3and4);
    OPTAB->Instructions[51] = Instruction_Alloc("STI", 0xD4, form3and4);
    OPTAB->Instructions[52] = Instruction_Alloc("STS", 0x7C, form3and4);
    OPTAB->Instructions[53] = Instruction_Alloc("STT", 0x84, form3and4);
    OPTAB->Instructions[54] = Instruction_Alloc("SUBF", 0x5C, form3and4);
    OPTAB->Instructions[55] = Instruction_Alloc("SUBR", 0x94, form2);
    OPTAB->Instructions[56] = Instruction_Alloc("SVC", 0xB0, form2);
    OPTAB->Instructions[57] = Instruction_Alloc("TIO", 0xF8, form1);
    OPTAB->Instructions[58] = Instruction_Alloc("TIXR", 0xB8, form2);

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
            return returnOpcode;
        }
    }
    return returnOpcode;
}

SymbolTable *ST_create( void ) {
    // allocate memory for a symbol table and set entries to point to the first available address in memory
    SymbolTable *hashtable = (SymbolTable *)calloc(1, sizeof(SymbolTable));

    // allocate memory for a pointer to each symbol in the TableEntries array of pointers to symbols
    hashtable->TableEntries = calloc(SYMTAB_SIZE, sizeof(Symbol *));

    // set each entry in the table to null so that we can search for empty indeces in the table to insert
    int i = 0;
    for (; i < SYMTAB_SIZE; i++) {
        hashtable->TableEntries[i] = NULL;
    }

    //hastable->TableEntries[26] = ;
    return hashtable;
}

Symbol *Symbol_alloc(  char *name, int address, int sourceline ) {
    // allocate memory for the symbol table entry
    Symbol *entry = (Symbol *)calloc(1, sizeof(entry));
    // allocate memory for the symbol name = size of the char array + 1
    entry->Name = (char *)calloc(strlen(name) + 1, sizeof(char));
    //entry->Address = malloc( sizeof(int) ); // allocate memory for the address = size of an int
    entry->Address = address;
    //entry->SourceLineNumber = malloc( sizeof(int) ); // allocate memory for the source line number = size of an int
    entry->SourceLineNumber = sourceline;
    
    // copy the name in place
    strcpy(entry->Name, name);
    entry->Address = address;
    entry->SourceLineNumber = sourceline;

    entry->next = NULL;

    return entry;
}

void ST_set( SymbolTable *hashtable, char *name, int address, int sourceline) {
    //printf("ST_set: name: %s\taddress: %X\tsource line: %d\n", name, address, sourceline);
    unsigned int tableIndex = name[0] - 65;
    // summary
    Symbol *entry = hashtable->TableEntries[tableIndex];

    // if there is not already an entry in the symbol table for the first letter of the symbol name,
    // create a new entry
    if ( entry == NULL ) {
        hashtable->TableEntries[tableIndex] = Symbol_alloc(name, address, sourceline);
        return;
    }

    Symbol *prev;
    
    while ( entry != NULL ){
        // check if the symbol name matches the name of a symbol in the first position of the linked list for that letter
        if ( entry->next == NULL ) {
            entry->next = Symbol_alloc(name, address, sourceline);
            return;
        }
        // walk to the next entry and update the next pointer
        prev = entry;
        entry = prev->next;
    }
    
}

Symbol *ST_get(SymbolTable *hashtable, const char *name) {
    //printf("\tST_get: name: %s\taddress: %d\tsource line: %d\n", name, *address, *sourceline);
    unsigned int tableIndex = name[0] - 65;

    Symbol *entry = hashtable->TableEntries[tableIndex];

    if ( entry == NULL ) {
        return NULL;
    }

    while ( entry != NULL ) {
        if ( strcmp( entry->Name, name ) == 0 ) {
            return entry;
        }
        // move to the next entry to check
        entry = entry->next;
    }
    // no matching entry found
    return NULL;
}


void ST_print( SymbolTable *hashtable ) {
        // grab the entry in the hashtable at index 1 
        //      - Each index corresponds to an entry with a symbol name starting with a letter in the alphabet
        //      - If there are more than one symbols starting with that letter, the entry links to the next entry
        //          in the linked list using entry->next
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        Symbol *entry = hashtable->TableEntries[i];
        if (entry == NULL) {
            continue;
        }
        while (entry!= NULL) {
            printf("%-10s %#08x\n", entry->Name, entry->Address);
            entry = entry->next;
        }
    }
}

AddrModeTable* AMT_create(void) {
    AddrModeTable* table = (AddrModeTable *)calloc(1, sizeof(AddrModeTable));
    table->format3 = (int **)calloc(sizeof(int *), 1);
    table->format4 = (int **)calloc(sizeof(int *), 1);

    table->format3[0] = (int *)calloc(sizeof(int), 8);
    table->format3[1] = (int *)calloc(sizeof(int), 3);
    table->format3[2] = (int *)calloc(sizeof(int), 3);
    
    table->format3[0][0] = 0x30; 
    table->format3[0][1] = 0x32; 
    table->format3[0][2] = 0x34;
    table->format3[0][3] = 0x38;
    table->format3[0][4] = 0x3a;
    table->format3[0][5] = 0x3c;
    table->format3[0][6] = 0x00;
    table->format3[0][7] = 0x08;
    table->format3[1][0] = 0x20;
    table->format3[1][1] = 0x22;
    table->format3[1][2] = 0x24;
    table->format3[2][0] = 0x10;
    table->format3[2][1] = 0x12; 
    table->format3[2][2] = 0x14;

    table->format4[0] = (int *)calloc(sizeof(int), 2);
    table->format4[1] = (int *)calloc(sizeof(int), 1);
    table->format4[2] = (int *)calloc(sizeof(int), 1);

    table->format4[0][0] = 0x31;
    table->format4[0][0] = 0x39;
    table->format4[1][0] = 0x21;
    table->format4[2][0] = 0x11;
    
    return table;
}

void errorPrint(char* line) {
    printf("ASSEMBLY ERROR:\t%s", line);
    return;
}

void DocumentError(int errorCode, char* line, int lineNumber, char* label, char* opcode, char* operand, int location) {
    /**
     * 
     */

}

//const char* parseOpcodeStr(char* opcodeStr)
//{ // used to parse the opcode char array to remove format 4
//
//}

//const char* indirectFormatObjCode(OpcodeTable* optab, SymbolTable* symtab, char* opcodeStr, char* operandStr)
//{ // indirect format instruction handling
//    char format4Char = opcodeStr[0];
//    if (format4Char == '+')
//    { // format 4 instructions are indicated by a plus sign prepended to an instruction mnemonic
//
//    }
//}

//const char* immediateFormatObjCode(OpcodeTable* optab, SymbolTable* symtab, char* opcodeStr, char* operandStr)
//{
//
//}

//const char* simpleFormatObjCode(OpcodeTable* optab, SymbolTable* symtab, char* opcodeStr, char* operandStr)
//{
//    // create buffer to store the opcode without the format 4 indicator, '+'
//    char* opcode = (char *)calloc(strlen(opcodeStr), sizeof(char));
//    strcpy(opcode, opcodeStr+1);
//    printf("Inside of format4ObjCode, opcode: %s\n", opcode);
//    return opcode;
//}

//const char* getObjCode(OpcodeTable* optab, SymbolTable* symtab, char* opcodeStr, char* operandStr)
//{
//    char operandFormat = operandStr[0];
//    switch(operandFormat)
//    {
//        case('@'):
//            return indirectFormatObjCode(optab, symtab, opcodeStr, operandStr);
//        case('#'):
//            return immediateFormatObjCode(optab, symtab, opcodeStr, operandStr);
//        default:
//            return simpleFormatObjCode(optab, symtab, opcodeStr, operandStr);
//    }
//}

int isDirective(char *possibleDirec) {
    // START - specifies the name and starting address of program, routine, or library
    if (strcmp(possibleDirec, "START") == 0) { return 1; }
    // END - Indicates the end of the program and (optionally) specifies the first executable 
    //      instruction on the program
    if (strcmp(possibleDirec, "END") == 0) { return 1; }
    // BYTE - Generate character or hexadecimal constant, occupying as many bytes as needed to represent
    //      the constant
    if (strcmp(possibleDirec, "BYTE") == 0 ) { return 1; }
    // WORD - generate a one-word integer constant (words in SIC are 3 bytes)
    if (strcmp(possibleDirec, "WORD") == 0) { return 1; }
    // RESB - reserve the indicated number of bytes for a data area
    if (strcmp(possibleDirec, "RESB") == 0) { return 1; }
    // RESW - reserve the indicated number of words for a data area
    if (strcmp(possibleDirec, "RESW") == 0) { return 1; }
    // RESR - reserve space for an external reference address or lubrary location. 3 Bytes
    if (strcmp(possibleDirec, "RESR") == 0) { return 1; }
    // EXPORTS - export the symbol address in the object file for cross-file linking. 3 Bytes
    if (strcmp(possibleDirec, "EXPORTS") == 0) { return 1; }
    // BASE - used to indicate what value will be in the base register that displacement will be calculated with
    if (strcmp(possibleDirec, "BASE") == 0) { return 1; }
    return 0; // The directive check failed
}

int calcByte(char* line, char* operand, int loc_counter, int lineNumber) {
    // BYTE C'character_sequence'
    if (operand[0] == 'C') {
        int byteCharLen = strcspn(operand + 2, "\'");
        char *opChar = (char *)calloc(byteCharLen, sizeof(char));
        sprintf(opChar, "%.*s", byteCharLen, operand + 2);
        /*
        printf("\topChar: %s\n", opChar);
        */
        
        if (operand[byteCharLen + 2] != '\'') {
            //printf("operand: %s, byteCharLen + 3: |%c|\n", opChar, operand[byteCharLen + 2]);
            errorPrint(line);
            printf("Line %d: incorrect usage of BYTE directive\n", lineNumber);
            return 0;
        }
        int charLength = strlen(opChar);
        return loc_counter + charLength;
    } else if (operand[0] == 'X') {
        // BYTE X'hex_sequence'
        int byteCharHex = strcspn(operand + 2, "\'");
        char* opHex = (char *)calloc(strlen(operand) - 2, sizeof(char));
        strncpy(opHex, operand + 2, byteCharHex);
        int hexLength = strlen(opHex);
        // Check for valid hex characters
        
        for (int i = 0; i < strlen(opHex); i++) {
            char* iHex = (char *)calloc(4, sizeof(char));
            sprintf(iHex, "0x%.*s", 1, opHex + i);
            int hexVal = (int)strtol(iHex, NULL, 16);
            if (opHex[i] != 48 && hexVal == 0) {
                //printf("\tchar = %c\n", opHex[i]);
                errorPrint(line);
                printf("Line %d: improper hex constant allocation: %s\n",lineNumber, opHex);
                return 0;
            }
            //printf("\topHex[%d] c = %s\thex = %X\n", i, iHex, hexVal);
        }
        //printf("\n");
        // if the hex characters are not given in multiples of 2 we cannot properly store them
        if (hexLength % 2 != 0) {
            errorPrint(line);
            printf("Line %d: improper hex constant allocation: %s\n", lineNumber, opHex);
            return 0;
        }
        int byteLength = hexLength / 2;
        return loc_counter + byteLength;
    } else {
        errorPrint(line);
        printf("Line %d: incorrect usage of BYTE directive\n", lineNumber);
        return 0;
    }
}
/** function to calculate the new loc_counter address
 * TODO Add BASE directive handling
 *
 */
int calcDirective(char* line, char *directive, char *operand, int loc_counter, int lineNumber) {
    int returnLoc;
    if (strcmp(directive, "START") == 0) {
        returnLoc = loc_counter;
    } else if (strcmp(directive, "END") == 0) {
        returnLoc = loc_counter;
    } else if (strcmp(directive, "WORD") == 0) {
        // WORD generates a one word (3 bytes) integer constant 
        int opVal = atoi(operand);
        //printf("\tWORD operand = %d\n", opVal);
        if (opVal >= 16777216) {
            errorPrint(line);
            printf("Line %d: attempt to generate a word-length constant (%d) greater than the size of a word\n", lineNumber, opVal);
            return 0;
        }
        returnLoc = loc_counter + 3;
    } else if (strcmp(directive, "RESB") == 0) {
        // RESB reserves the indicated (in decimal) number of bytes for a data area
        int byteDec = atoi(operand);
        returnLoc = loc_counter + byteDec;
    } else if (strcmp(directive, "RESW") == 0) {
        returnLoc = loc_counter + atoi(operand) * 3;
    } else if (strcmp(directive, "RESR") == 0) {
        returnLoc = loc_counter + 3;
    } else if (strcmp(directive, "BYTE") == 0) {
        returnLoc = calcByte(line, operand, loc_counter, lineNumber);
    } else if (strcmp(directive, "RESR") == 0) {
        returnLoc = loc_counter + 3;
    } else if (strcmp(directive, "EXPORTS") == 0) {
        returnLoc = loc_counter + 3;
    } else {
        returnLoc = 0;
    }
    return returnLoc;
}



const char* convertCharConst(char *characterConst) {
    // get the length of the character constant
    int len = strlen(characterConst);
    char* hex = (char *)calloc(1 + len * 2, sizeof(char));
    hex[0] = '0';
    hex[1] = 'x';
    int index = 2;
    int x;
    for (x = 0; x < len; x++) {
        index += sprintf(&hex[index], "%.*X", 2, characterConst[x]);
    }
    //printf("|%s|\n", hex);
    return hex + 2;
}

const char* byteConstantObjCode(char* byteDirOperand) {

    char* workingStr = (char *)calloc(strlen(byteDirOperand), sizeof(char));
    strcpy(workingStr, byteDirOperand);

    char* ptr;

    if (byteDirOperand[0] == 'X') {
        // process string to get constant
        ptr = strtok(byteDirOperand, "\'");
        ptr = strtok(NULL, "\'");
        printf("result of byteCharConstantObjCode() call: %s\n", ptr);
        if ((strlen(ptr) % 2) != 0) {
            return "null";
        }
        int x = 0;
        int index = 2;
        char hex[80];
        hex[0] = '0';
        hex[1] = 'X';

        int len = strlen(ptr);
        for (; x < len; x++) {
            index += sprintf(&hex[index], "%.*X", 2, ptr[x]);
        }
        const char* returnVal = hex;
        return returnVal;
    }
    else if (byteDirOperand[0] == 'C') {
        // process string to get constant
        ptr = strtok(byteDirOperand, "\'");
        ptr = strtok(NULL, "\'");
        printf("result of byteCharConstantObjCode() call: %s\n", ptr);
    }
    return "null";
}



#endif
