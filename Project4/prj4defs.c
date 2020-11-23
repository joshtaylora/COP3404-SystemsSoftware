#ifdef ASSEMBLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "prj4defs.h"

#define SYMTAB_SIZE 26
#define OPTAB_SIZE 59


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
    // SICXE Instructions
    OPTAB->Instructions[26] = Instruction_Alloc("ADDF", 0x58);
    OPTAB->Instructions[27] = Instruction_Alloc("ADDR", 0x90);
    OPTAB->Instructions[28] = Instruction_Alloc("CLEAR", 0xB4);
    OPTAB->Instructions[29] = Instruction_Alloc("COMPF", 0x88);
    OPTAB->Instructions[30] = Instruction_Alloc("COMPR", 0xA0);
    OPTAB->Instructions[31] = Instruction_Alloc("DIVF", 0x64);
    OPTAB->Instructions[32] = Instruction_Alloc("DIVR", 0x9C);
    OPTAB->Instructions[33] = Instruction_Alloc("FIX", 0xC4);
    OPTAB->Instructions[34] = Instruction_Alloc("FLOAT", 0xC0);
    OPTAB->Instructions[35] = Instruction_Alloc("HIO", 0xF4);
    OPTAB->Instructions[36] = Instruction_Alloc("LDB", 0x68);
    OPTAB->Instructions[37] = Instruction_Alloc("LDF", 0x70);
    OPTAB->Instructions[38] = Instruction_Alloc("LDS", 0x6C);
    OPTAB->Instructions[39] = Instruction_Alloc("LDT", 0x74);
    OPTAB->Instructions[40] = Instruction_Alloc("LPS", 0xD0);
    OPTAB->Instructions[41] = Instruction_Alloc("MULF", 0x60);
    OPTAB->Instructions[42] = Instruction_Alloc("MULR", 0x98);
    OPTAB->Instructions[43] = Instruction_Alloc("NORM", 0xC8);
    OPTAB->Instructions[44] = Instruction_Alloc("RMO", 0xAC);
    OPTAB->Instructions[45] = Instruction_Alloc("SHIFTL", 0xA4);
    OPTAB->Instructions[46] = Instruction_Alloc("SHIFTR", 0xA8);
    OPTAB->Instructions[47] = Instruction_Alloc("SIO", 0xF0);
    OPTAB->Instructions[48] = Instruction_Alloc("SSK", 0xEC);
    OPTAB->Instructions[49] = Instruction_Alloc("STB", 0x78);
    OPTAB->Instructions[50] = Instruction_Alloc("STF", 0x80);
    OPTAB->Instructions[51] = Instruction_Alloc("STI", 0xD4);
    OPTAB->Instructions[52] = Instruction_Alloc("STS", 0x7C);
    OPTAB->Instructions[53] = Instruction_Alloc("STT", 0x84);
    OPTAB->Instructions[54] = Instruction_Alloc("SUBF", 0x5C);
    OPTAB->Instructions[55] = Instruction_Alloc("SUBR", 0x94);
    OPTAB->Instructions[56] = Instruction_Alloc("SVC", 0xB0);
    OPTAB->Instructions[57] = Instruction_Alloc("TIO", 0xF8);
    OPTAB->Instructions[58] = Instruction_Alloc("TIXR", 0xB8);

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

SymbolTable *ST_create( void ) {
    // allocate memory for a symbol table and set entries to point to the first available address in memory
    SymbolTable *hashtable = malloc( sizeof( SymbolTable ) * 1 );

    // allocate memory for a pointer to each symbol in the TableEntries array of pointers to symbols
    hashtable->TableEntries = malloc( sizeof( Symbol* ) * SYMTAB_SIZE );

    // set each entry in the table to null so that we can search for empty indeces in the table to insert
    int i = 0;
    for (; i < SYMTAB_SIZE; i++) {
        hashtable->TableEntries[i] = NULL;
    }

    return hashtable;
}

Symbol *Symbol_alloc(  char *name, int address, int sourceline ) {
    Symbol *entry = malloc( sizeof( entry ) * 1 ); // allocate memory for the symbol table entry
    entry->Name = malloc( strlen( name ) + 1 ); // allocate memory for the symbol name = size of the char array + 1
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

Symbol *ST_get(SymbolTable *hashtable, char *name) {
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


void ST_print( SymbolTable *hashtable )
{
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
            printf("%s\t%X\n", entry->Name, entry->Address);
            entry = entry->next;
        }
    }
}

void errorPrint(char* line)
{
    printf("ASSEMBLY ERROR:\t%s", line);
    return;
}

int isDirective(char *possibleDirec)
{
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
    return 0; // The directive check failed
}

// validate the opcodes
int opcodeCalc(char *opcode) 
{
    // if the opcode is a valid mnemonic then it returns the opcode
    // if not, it returns -1
    if (strcmp(opcode, "ADD")==0)
    {
        return 0x18;
    }
    else if (strcmp(opcode, "AND")==0)
    {
        return 0x40;
    }
    else if (strcmp(opcode, "COMP")==0)
    {
        return 0x28;
    }
    else if (strcmp(opcode, "DIV")==0)
    {
        return 0x24;
    }
    else if (strcmp(opcode, "J")==0)
    {
        return 0x3C;
    }
    else if (strcmp(opcode, "JEQ")==0)
    {
        return 0x30;
    }
    else if (strcmp(opcode, "JGT")==0)
    {
        return 0x34;
    }
    else if (strcmp(opcode, "JLT")==0)
    {
        return 0x38;
    }
    else if (strcmp(opcode, "JSUB")==0)
    {
        return 0x48;
    }
    else if (strcmp(opcode, "LDA")==0)
    {
        return 0x00;
    }
    else if (strcmp(opcode, "LDCH")==0)
    {
        return 0x50;
    }
    else if (strcmp(opcode, "LDL")==0)
    {
        return 0x08;
    }
    else if (strcmp(opcode, "LDX")==0)
    {
        return 0x04;
    }
    else if (strcmp(opcode, "MUL")==0)
    {
        return 0x20;
    }
    else if (strcmp(opcode, "OR")==0)
    {
        return 0x44;
    }
    else if (strcmp(opcode, "RD")==0)
    {
        return 0xD8;
    }
    else if (strcmp(opcode, "RSUB")==0)
    {
        return 0x4C;
    }
    else if (strcmp(opcode, "STA")==0)
    {
        return 0x0C;
    }
    else if (strcmp(opcode, "STCH")==0)
    {
        return 0x54;
    }
    else if (strcmp(opcode, "STL")==0)
    {
        return 0x14;
    }
    else if (strcmp(opcode, "STSW")==0)
    {
        return 0xE8;
    }
    else if (strcmp(opcode, "STX")==0)
    {
        return 0x10;
    }
    else if (strcmp(opcode, "SUB")==0)
    {
        return 0x1C;
    }
    else if (strcmp(opcode, "TD")==0)
    {
        return 0xE0;
    }
    else if (strcmp(opcode, "TIX")==0)
    {
        return 0x2C;
    }
    else if (strcmp(opcode, "WD")==0)
    {
        return 0xDC;
    }
    else
    {
        return -1;
    }
}

int calcByte(char* line, char* operand, int loc_counter, int lineNumber)
{
    // BYTE C'character_sequence'
    if (operand[0] == 'C')
    {
        int byteCharLen = strcspn(operand + 2, "\'");

        char *opChar = malloc(sizeof(char) * byteCharLen);
        sprintf(opChar, "%.*s", byteCharLen, operand + 2);
        /*
        printf("\topChar: %s\n", opChar);
        */
        
        if (operand[byteCharLen + 2] != '\'')
        {
            //printf("operand: %s, byteCharLen + 3: |%c|\n", opChar, operand[byteCharLen + 2]);
            errorPrint(line);
            printf("Line %d: incorrect usage of BYTE directive\n", lineNumber);
            return 0;
        }
        int charLength = strlen(opChar);
        return loc_counter + charLength;
    }
    // BYTE X'hex_sequence'
    else if (operand[0] == 'X')
    {
        char *opHex = strtok(operand, "\'");
        opHex = strtok(NULL, "\'");
        int hexLength = strlen(opHex);
        // Check for valid hex characters
        
        for (int i = 0; i < strlen(opHex); i++)
        {
            char* iHex = malloc(sizeof(char) * 4);
            sprintf(iHex, "0x%.*s", 1, opHex + i);
            int hexVal = (int)strtol(iHex, NULL, 16);
            if (opHex[i] != 48 && hexVal == 0)
            {
                //printf("\tchar = %c\n", opHex[i]);
                errorPrint(line);
                printf("Line %d: improper hex constant allocation: %s\n",lineNumber, opHex);
                return 0;
            }
            //printf("\topHex[%d] c = %s\thex = %X\n", i, iHex, hexVal);
        }
        //printf("\n");
        // if the hex characters are not given in multiples of 2 we cannot properly store them
        if (hexLength % 2 != 0)
        {
            errorPrint(line);
            printf("Line %d: improper hex constant allocation: %s\n", lineNumber, opHex);
            return 0;
        }
        int byteLength = hexLength / 2;
        return loc_counter + byteLength;
    }
    else
    {
        errorPrint(line);
        printf("Line %d: incorrect usage of BYTE directive\n", lineNumber);
        return 0;
    }
}
// function to calculate the new loc_counter address
int calcDirective(char* line, char *directive, char *operand, int loc_counter, int lineNumber)
{
    if (strcmp(directive, "START") == 0)
    {
        return loc_counter;
    }
    if (strcmp(directive, "END") == 0)
    {
        return loc_counter;
    }
    if (strcmp(directive, "WORD") == 0)
    {
        // WORD generates a one word (3 bytes) integer constant 
        int opVal = atoi(operand);
        //printf("\tWORD operand = %d\n", opVal);
        if (opVal >= 16777216)
        {
            errorPrint(line);
            printf("Line %d: attempt to generate a word-length constant (%d) greater than the size of a word\n", lineNumber, opVal);
            return 0;
        }
        return loc_counter + 3;
    }
    if (strcmp(directive, "RESB") == 0)
    {
        // RESB reserves the indicated (in decimal) number of bytes for a data area
        int byteDec = atoi(operand);
        return loc_counter + byteDec;
    }
    if (strcmp(directive, "RESW") == 0)
    {
        loc_counter += atoi(operand) * 3;
        return loc_counter;
    }
    if (strcmp(directive, "RESR") == 0)
    {   
        loc_counter += 3;
        return loc_counter;
    }
    if (strcmp(directive, "BYTE") == 0)
    {
        return calcByte(line, operand, loc_counter, lineNumber);
        
    }
    if (strcmp(directive, "RESR") == 0)
    {
        return loc_counter + 3;
    }
    if (strcmp(directive, "EXPORTS") == 0)
    {
        return loc_counter + 3;
    }
    return 0;
}

const char* convertCharConst(char *optab)
{
    // get the length of the character constant
    int len = strlen(optab);
    char* hex = malloc(sizeof(char) * 100);
    hex[0] = '0';
    hex[1] = 'x';
    int index = 2;
    int x;
    for (x = 0; x < len; x++)
    {
        index += sprintf(&hex[index], "%.*X", 2, optab[x]);
    }
    
    //printf("|%s|\n", hex);
    
    
    
    return hex + 2;

}
#endif