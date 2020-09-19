#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define TABLE_SIZE 26

typedef struct Symbol {
    int *Address;
    int *SourceLineNumber;
    struct Symbol *next; // pointer to the next symbol in the linked list
    char *Name; // character array to store name of symbol
} Symbol;

typedef struct SymbolTable {
    Symbol **TableEntries; // pointer to Symbol pointers (acts as an array of pointers)
} SymbolTable;

SymbolTable *ST_create( void ) {
    // allocate memory for a symbol table and set entries to point to the first available address in memory
    SymbolTable *hashtable = malloc( sizeof( SymbolTable ) * 1 );

    // allocate memory for a pointer to each symbol in the TableEntries array of pointers to symbols
    hashtable->TableEntries = malloc( sizeof( Symbol* ) * TABLE_SIZE );

    // set each entry in the table to null so that we can search for empty indeces in the table to insert
    int i = 0;
    for (; i < TABLE_SIZE; i++) {
        hashtable->TableEntries[i] = NULL;
    }

    return hashtable;
}

Symbol *Symbol_alloc(  char *name, int *address, int *sourceline ) {
    Symbol *entry = malloc( sizeof( entry ) * 1 ); // allocate memory for the symbol table entry
    entry->Name = malloc( strlen( name ) + 1 ); // allocate memory for the symbol name = size of the char array + 1
    entry->Address = malloc( sizeof(int) ); // allocate memory for the address = size of an int
    entry->SourceLineNumber = malloc( sizeof(int) ); // allocate memory for the source line number = size of an int
    
    // copy the name in place
    strcpy(entry->Name, name);
    entry->Address = address;
    entry->SourceLineNumber = sourceline;

    entry->next = NULL;

    return entry;
}

void ST_set( SymbolTable *hashtable, char *name, int *address, int *sourceline) {
    printf("\tST_set: name: %s\taddress: %d\tsource line: %d\n", name, *address, *sourceline);
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
        if ( strcmp( entry->Name, name ) == 0 ) {
            free( entry->Name );
            entry->Name = malloc( strlen( name ) + 1 );
            strcpy( entry->Name, name );
            return;
        }
        // walk to the next entry and update the next pointer
        prev = entry;
        entry = prev->next;
    }
}

Symbol *ST_get(SymbolTable *hashtable, char *name, int *address, int *sourceline) {
    printf("\tST_get: name: %s\taddress: %d\tsource line: %d\n", name, *address, *sourceline);
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

void ST_print( SymbolTable *hashtable ){
    for( int i = 0; i < TABLE_SIZE; i++ ) {
        Symbol *entry = hashtable->TableEntries[i];
        if ( entry == NULL ) 
        { 
            continue; 
        }
        for (;;) {
            printf("%s\t%d\n", entry->Name, *(entry->Address));
            /*
            while (entry->next != NULL) 
            {
                Symbol *next = entry->next;
                printf("%s\t%d\n", next->Name, *(next->Address));
                Symbol *prev = entry->next;
                entry = prev->next;
            }
            */
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
/*
int isOpcode(char *possibleOpcode) {
    // validate the opcodes
}
*/
// function to calculate the new loc_counter address
int calcDirective(char *directive, char *operand, int *loc_counter) {
    if (strcmp(directive, "WORD") == 0) {
        // WORD generates a one word (3 bytes) integer constant 
        return *loc_counter + 3;
    }
    if (strcmp(directive, "RESB") == 0) {
        // RESB reserves the indicated (in decimal) number of bytes for a data area
        int byteDec = atoi(operand);
        return *loc_counter + byteDec;
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

int main(int argc, char *argv[]) {
    
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
    int line_number; 
    line_number = 1; 

    // initialize the symbol table
    SymbolTable *symbol_table = ST_create();
    printf("SYMBOL TABLE CREATED\n");
    // use st_set(symbol_table, "SYMBOL", loc_counter) to set new entries in the symbol table


	//read the file, line by line
	char line[1024];

	int length;
    // counter that checks if we have encountered the START directive yet
    int startCheck = 0;
	
    // fgets returns a null pointer if the EOF char is read
	while(fgets(line, 1024, inputFile)){
        printf("LINE[%d]: %s\n", line_number, line);

        for (int i = 0; i < strlen(line); i++) {
            printf("line[%d], char[%d] = %c\n",line_number, i, line[i]);
        }
		 
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
                    Symbol *duplicate = ST_get(symbol_table, sym, &loc_counter, &line_number);
                    if (duplicate != NULL) {
                        
                        printf("ERROR: duplicate symbol %s already defined on line%d (current line = %d)\n", 
                                duplicate->Name, *(duplicate->SourceLineNumber), line_number);
                        return 1;
                    }

                    // store the symbol in the symbol table 
                    ST_set(symbol_table, sym, &loc_counter, &line_number);
                    // increment the token to a possible directive/opcode
                    token =  strtok(NULL, " \t");
                    char *opcode = token;
                    // check for the start directive
                    if (startCheck == 0 && strcmp(opcode, "START") == 0) {
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
                        printf("symbol: %s\topcode: %s\taddress: %X\n", sym, opcode, loc_counter);

                    }
                    // if the start directive has not yet been encountered throw an error
                    else if (startCheck == 0) {
                        printf("ERROR: START directive not encountered before first directive/instruction on line[%d]\n", line_number);
                    }
                    else if (startCheck == 1 && isDirective(opcode) == 1) {
                        printf("\tdirective: %s, loc_counter: %X\n", opcode, loc_counter);
                        // advance token to get operand
                        token = strtok(NULL, " \t");
                        char *operand = token;
                        int newLoc = calcDirective(opcode, operand, &loc_counter);
                        // reach the new line character
                        token = strtok(NULL, " \t");
                        loc_counter = newLoc;
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
                    loc_counter = newLoc;
                    
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
                token = strtok(NULL, " \t\r\n");

            }
        }

        // increment the line counter
        line_number++;
	}
    ST_print(symbol_table);
    /*
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol *entry = symbol_table->TableEntries[i];
        if (entry == NULL) {
            continue;
        }
        printf("%s\t%d", entry->Name, *(entry->Address));
        Symbol *next = entry->next;
        while (entry->next != NULL) {
            Symbol *next = entry->next;
            printf("%s\t%d", next->Name, *(next->Address));
        }
    }
    */
	// close the opened file
	fclose( inputFile );
    return 0;
    
}


