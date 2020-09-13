#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        int address = *(entry->Address);
        if ( entry == NULL ) { continue; }
        for (;;) {
            printf("%s %d\n", entry->Name, address );
        }
    }
}

int isDirective(char *possibleDirec) {
    // START - specifies the name and starting address of program, routine, or library
    if (strcmp(possibleDirec, "START")) { return 1; }
    // END - Indicates the end of the program and (optionally) specifies the first executable 
    //      instruction on the program
    if (strcmp(possibleDirec, "END")) { return 1; }
    // BYTE - Generate character or hexadecimal constant, occupying as many bytes as needed to represent
    //      the constant
    if (strcmp(possibleDirec, "BYTE")) { return 1; }
    // WORD - generate a one-word integer constant (words in SIC are 3 bytes)
    if (strcmp(possibleDirec, "WORD")) { return 1; }
    // RESB - reserve the indicated number of bytes for a data area
    if (strcmp(possibleDirec, "RESB")) { return 1; }
    // RESW - reserve the indicated number of words for a data area
    if (strcmp(possibleDirec, "RESW")) { return 1; }
    // RESR - reserve space for an external reference address or lubrary location. 3 Bytes
    if (strcmp(possibleDirec, "RESR")) { return 1; }
    // EXPORTS - export the symbol address in the object file for cross-file linking. 3 Bytes
    if (strcmp(possibleDirec, "EXPORTS")) { return 1; }
    return 0; // The directive check failed
}

int main(char argc, char *argv[]) {
    
    Symbol* hashtable[26];
    
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
	int *loc_counter;
    // specifies the line that a symbol is defined on
    int line_number = 0; 

    // initialize the symbol table
    SymbolTable *symbol_table = ST_create();
    printf("SYMBOL TABLE CREATED\n");
    // use st_set(symbol_table, "SYMBOL", loc_counter) to set new entries in the symbol table


	//read the file, line by line
	char fileArg[1024];

	int length;
	
    // fgets returns a null pointer if the EOF char is read
	while( fgets(fileArg, 1024, inputFile) ){
        // initialize the line number counter to 1 to indicate we are on the first line
        line_number++;

        // splits the file into lines tokenized using the new line character
        char *line = strtok(fileArg, "\n");
		
        printf("READ: %s\n", line );

		length = strlen( line );
		 
        if ( length > 0 ) {
			// check for comment ( 35 = character value for ascii value for # symbol )
			if (line[0] == 35 ) {
				printf("--was a comment\n");
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
                        Symbol *duplicate = ST_get(symbol_table, sym, loc_counter, &line_number);
                        if (duplicate != NULL) {
                            
                            printf("ERROR: duplicate symbol %s already defined on line%d (current line = %d)\n", 
                                    duplicate->Name, *(duplicate->SourceLineNumber), line_number);
                            return 1;
                        }

                        // store the symbol in the symbol table 
                        ST_set(symbol_table, sym, loc_counter, &line_number);
                        printf("symbol: %s\n", sym);
                        char *opcode = strtok(NULL, " \t");
                        if (opcode != NULL) {
                            if (isDirective(opcode) == 1) {
                                printf("\tdirective: %s\n", opcode);
                            }
                            else {
                                printf("\topcode: %s\n", opcode);
                            }
                        }
                        char *operand = strtok(NULL, " \t");
                        printf("\t\toperand: %s\n", operand);
                        token = strtok( NULL, " \t");
                        continue;
                    }
                    /*
                    if (isDirective(token) == 1) {
                        printf("--directive line, directive is: %s\n", token);
                    }
                    else {
                        printf("--instruction line, opcode is: %s\n", token);
                    }
                    */
                    token = strtok( NULL, " \t");
                }
            }

		}

        line = strtok(NULL, "\n");
	}

	// close the opened file
	fclose( inputFile );
    return 0;
    
}

