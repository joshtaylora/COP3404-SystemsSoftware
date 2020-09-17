#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 26

// structure for creating a symbol
typedef struct Symbol 
{
    int Address;
    int SourceLineNumber;
    struct Symbol *Next; // pointer to the next symbol in the linked list
    char *Name; // character array to store name of symbol
} Symbol;

// structure for implementing a symbol table to store symbols
typedef struct SymbolTable 
{
    Symbol **symbolEntries; // points to an array of Symbol pointers
} SymbolTable;

// returns a pointer to the location in memory where the symbol table is stored
SymbolTable *ST_Create(void) 
{
    // alloc memory for the symbol table large enough to hold it
    SymbolTable *symbolTable = malloc(sizeof(SymbolTable) * 1);
    // alloc memory for the symbols that will be in the table
    symbolTable->symbolEntries = malloc(sizeof(Symbol*) * TABLE_SIZE);
    // set each symbol entry in the table to null so we can easily insert new entries without
    // collisions
    int i;
    for (i = 0; i < TABLE_SIZE; i++)
    {
        symbolTable->symbolEntries[i] = NULL;
    }
    return symbolTable;
}

// returns a pointer to the location in memory where a new symbol has been allocated memory
Symbol *Symbol_Alloc(char *name, int address, int sourceLine)
{
    // alloc enough memory for the new ST entry
    Symbol *entry = malloc(sizeof(entry) * 1);
    
    // we add 1 to the length of the char array name for the null character '0'
    // alloc memory for the char array we store in name
    entry->Name = malloc(strlen(name) + 1);
    
    // copy the char array passed as name into the Name field of the symbol in place
    strcpy(entry->Name, name);
    
    // set the symbol's Address field
    entry->Address = address;
    
    // set the symbol's source line number
    entry->SourceLineNumber = sourceLine;
    
    // set the symbol's pointer to the next element = NULL since its the first element in the linked list for the array corresponding to that symbol's first char in Name
    entry->Next = NULL;

    return entry;
}

void ST_Set(SymbolTable *symbolTable, char *name, int address, int sourceLine)
{
    unsigned int tableIndex = name[0] - 65;
    Symbol *entry = symbolTable->symbolEntries[tableIndex];
    
    // if the entry at the index we want to set the item in is NULL, we can insert the symbol
    // immediately
    if (entry == NULL)
    {
        symbolTable->symbolEntries[tableIndex] = Symbol_Alloc(name, address, sourceLine);
        return;
    }
    // create a Symbol object called prev to store the current entry we are checking before moving to the next one
    Symbol *prev;

    while (entry != NULL)
    {
        if (strcmp(entry->Name, name) == 0)
        {
            free(entry->Name);
            entry->Name = malloc(strlen(name) + 1);
            strcpy(entry->Name, name);
            return;
        }
        prev = entry;
        entry = prev->Next;
    }
}

Symbol *ST_Get(SymbolTable *symbolTable, char *name, int address, int sourceline)
{
    unsigned int tableIndex = name[0] - 65;

    Symbol *entry = symbolTable->symbolEntries[tableIndex];
    // if there is no symbol in the index indicated by the first letter of the symbol name
    if (entry == NULL)
    {
        // then the entry is not in the table
        return NULL;
    }
    // while we have not found the corresesponding symbol in the table
    while (entry != NULL)
    {
        if (strcmp(entry->Name, name) == 0)
        {
            return entry; // we have found the matching symbol in the table
        }
        entry = entry->Next;
    }
    // no matching symbol in the table was found
    return NULL;
}

void ST_Print(SymbolTable *symbolTable)
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        Symbol *entry = symbolTable->symbolEntries[i];
        int address = entry->Address;
        int source = entry->SourceLineNumber;
        if (entry == NULL)
        {
            continue;
        }
        for (;;) {
            printf("symbol:%s\tadddress: %d\tsource line: %d\n", entry->Name, address, source);
        }
    }
}


int isDirective(char *possibleDirec) {
    /* Checking for START is reduntant because we must first check for it in the program before any other directive
    // START - specifies the name and starting address of program, routine, or library
    if (strcmp(possibleDirec, "START")) { return 1; }
    */
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


int isOpcode(char *possibleOpcode) {
    // validate the opcodes
}
// function to calculate the new loc_counter address
int calcDirective(char *directive, char *operand, int *loc_counter) {
    if (strcmp(directive, "WORD") == 0) {
        // WORD generates a one word (3 bytes) integer constant 
        return *loc_counter + 3;
    }
    if (strcmp(directive, "RESB") == 0) {
        // RESB reserves the indicated (in decimal) number of bytes for a data area
        int byteDec = atoi(operand);
        int byteHex = (int)strtol(operand, NULL, 16);
        printf("\t---RESB hex: %X", byteHex);
        return *loc_counter + byteHex;
    }
    if (strcmp(directive, "RESW") == 0) {
        return 1;
    }
    if (strcmp(directive, "RESR") == 0) {
        return 1;
    }
    if (strcmp(directive, "BYTE") == 0) {
        // BYTE C'character_sequence'
        if (operand[0] == 67) {
            char *opChar = strtok(operand, "\'");
            opChar = strtok(NULL, "\'");
            int charLength = strlen(opChar);

            printf("---opChar:%s, length: %d\n", opChar, charLength);
            return *loc_counter + charLength;
        }
        // BYTE X'hex_sequence'
        else if (operand[0] == 88) {
            char *opHex = strtok(operand, "\'");
            opHex = strtok(NULL, "\'");
            int hexLength = strlen(opHex);
            // if the hex characters are not given in multiples of 2 we cannot properly store them
            if (hexLength % 2 != 0) {
                printf("ERROR: improper hex constant allocation: %s\n", opHex);
                return 0;
            }
            int byteLength = hexLength / 2;
            return *loc_counter + byteLength;
        }
        else {
            printf("ERROR: incorrect usage of BYTE directive\n");
            return 0;
        }
        
    }
    return 0;
}

int main(char argc, char *argv[]) {
    
    // check if a file was passed
    if( argc != 2 ) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }
    
    FILE *inputFile;
	inputFile = fopen( argv[1], "r");
	// check that the file being read exists (if fopen tries to read from a file that does not exist
	// it returns NULL
	if ( !inputFile ){
		printf("ERROR: %s could not be opened for reading.\n", argv[1]);
		return 1;
	}

    // The Location Counter that tells where we are in memory 
	int loc_counter;
    // specifies the line that a symbol is defined on
    int line_number = 1; 

    // initialize the symbol table
    SymbolTable *symbol_table = ST_Create();
    printf("SYMBOL TABLE CREATED\n");
    // use st_set(symbol_table, "SYMBOL", loc_counter) to set new entries in the symbol table


	//read the file, line by line
	char line[1024];

	int length;
    // counter that checks if we have encountered the START directive yet
    int startCheck = 0;
	
    // fgets returns a null pointer if the EOF char is read
	while(fgets(line, 1024, inputFile)){
        printf("LINE[%d]: %s\n",line_number, line);
		 
		// check for comment ( 35 = character value for ascii value for # symbol )
		if (line[0] == 35 ) {
			printf("--was a comment\n");
            line_number++;
            continue;
		}

        else {
            // splits the line into tokens separated by spaces or tabs
            char *token = strtok(line, " \t");

            // while there are still tokens on the line
            while ( token ){
                // check for a symbol definition: must be an alpha character (ASCII 65 through 90 )
                if ( ( line[0] >= 65 ) && ( line[0] <= 90 ) ) {
                    char *sym = token;
                    // if the symbol already exists in the symbol table, we need to throw an error
                    Symbol *duplicate = ST_Get(symbol_table, sym, loc_counter, line_number);
                    if (duplicate != NULL) {
                        
                        printf("ERROR: duplicate symbol %s already defined on line%d (current line = %d)\n", 
                                duplicate->Name, duplicate->SourceLineNumber, line_number);
                        return 1;
                    }

                    // store the symbol in the symbol table 
                    ST_Set(symbol_table, sym, loc_counter, line_number);
                    // increment the token to a possible directive/opcode
                    token =  strtok(NULL, " \t");
                    char *opcode = token;
                    // check for the start directive
                    if (strcmp(opcode, "START") == 0) {
                        // increment counter to indicate the start directive has been found
                        startCheck = 1;
                        // increment to the next token to check the starting address
                        token = strtok(NULL, " \t");
                        char *addr = token;
                        // the next 2 lines add the 0X characters to the address token to make the conversion to a hex int easier
                        char hex[15] = "0X";
                        strcat(hex, addr);
                        // makes loc_counter = hex int value of the address specified by the start directive
                        loc_counter = (int)strtol(hex, NULL, 0);
                        printf("symbol: %s\topcode: %s\taddress: %X\n",
                                sym, opcode, loc_counter);
                    }
                    // if the start directive has not yet been encountered throw an error
                    else if (startCheck == 0) {
                        printf("ERROR: START directive not encountered before first directive/instruction on line[%d]\n", line_number);
                    }
                    else if (startCheck == 1 && isDirective(opcode) == 1) {
                        printf("directive: %s\t loc_counter: %X\n", opcode, loc_counter);
                        // advance token to the operand
                        token = strtok(NULL, " \t");
                        char *operand = token;
                        int newLoc = calcDirective(opcode, operand, &loc_counter);
                        token = strtok(NULL, " \t");
                    }
                    else {
                        printf("\topcode: %s\n", opcode);
                    }

                }
                // if we have already encountered the START directive and the directive/opcode token IS a directive
                else if (startCheck == 1 && isDirective(token) == 1) {
                    char *directive = token;
                    printf("\tdirective: %s, loc_counter: %X\n", directive, loc_counter);
                    token = strtok(NULL, " \t");
                    char *operand = token;
                    int newLoc = calcDirective(directive, operand, &loc_counter);
                    token = strtok(NULL, " \t");
                    
                }
                // if the directive/opcode token is NOT a directive, increment the location counter by 3 bytes
                else if (startCheck == 1 && isDirective(token) == 0) {
                    printf("---not a directive\n");
                    char *opcode = token;
                    token = strtok(NULL, " \t");
                    char *operand = token;
                    printf("opcode: %s\t operand: %s\tloc_counter: %X\n", opcode, operand, loc_counter);
                    // increment location counter for the next line
                    loc_counter += 3;

                }
                else {
                    printf("ERROR: not a directive or instruction on line: %d", line_number);
                    return 1;
                }
                token = strtok(NULL, " \t");

            }
        }

        // increment the line counter
        line_number++;
	}

	// close the opened file
	fclose( inputFile );
    return 0;
    
}

