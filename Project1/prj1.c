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
    //printf("\tST_set: name: %s\taddress: %d\tsource line: %d\n", name, *address, *sourceline);
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
    if (strcmp(directive, "END") == 0)
    {
        return *loc_counter;
    }
    if (strcmp(directive, "WORD") == 0)
    {
        // WORD generates a one word (3 bytes) integer constant 
        return *loc_counter + 3;
    }
    if (strcmp(directive, "RESB") == 0)
    {
        // RESB reserves the indicated (in decimal) number of bytes for a data area
        int byteDec = atoi(operand);
        return *loc_counter + byteDec;
    }
    if (strcmp(directive, "RESW") == 0)
    {
        return 1;
    }
    if (strcmp(directive, "RESR") == 0)
    {
        return 1;
    }
    if (strcmp(directive, "BYTE") == 0)
    {
        // BYTE C'character_sequence'
        if (operand[0] == 67)
        {
            char *opChar = strtok(operand, "\'");
            opChar = strtok(NULL, "\'");
            int charLength = strlen(opChar);

            printf("---opChar:%s, length: %d\n", opChar, charLength);
            return *loc_counter + charLength;
        }
        // BYTE X'hex_sequence'
        else if (operand[0] == 88)
        {
            char *opHex = strtok(operand, "\'");
            opHex = strtok(NULL, "\'");
            int hexLength = strlen(opHex);
            // if the hex characters are not given in multiples of 2 we cannot properly store them
            if (hexLength % 2 != 0)
            {
                printf("ERROR: improper hex constant allocation: %s\n", opHex);
                return 0;
            }
            int byteLength = hexLength / 2;
            return *loc_counter + byteLength;
        }
        else
        {
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

    // length of contiguous whitespace, length of token
    int wspLength, tokenLength;
    // int to check if a comment is on a line, int to see how much of a line is not a comment
    int commCheck, notComm;
    // index of current position in the line
    int lineIndex;
    // current line number for line we are comparing
    int lineNumber = 1;

    
	
    // fgets returns a null pointer if the EOF char is read
	while(fgets(line, 1024, inputFile) != NULL)
    {
        // allocate memory for the token strings
        char* symbol = malloc(6 * sizeof(char));
        char* opcode = malloc(10 * sizeof(char));
        char* operand = malloc(10 * sizeof(char));

        // these are the const char strings used to check for different types of tokens
        const char* wsp = " \t";
        const char* alphaUp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const char* comment = "#";
        // end the line with the null character
        line[strlen(line)-1] = '\0';
        // start at the first character in the line
        lineIndex = 0;

        printf("---------------- LINE[%d] ----------------\n", lineNumber);
        printf("%s\n", line);

        // check for a comment
        commCheck = strspn(line, comment);

        // count the number of contiguous uppercase alpha characters
        tokenLength = strspn(line, alphaUp);
        // copy the symbol name to string called symbol
        strncpy(symbol, line, tokenLength);

        wspLength = strspn(line, wsp);

        // if the directive line is a symbol definition
        if (strlen(symbol) > 0 && strcspn(symbol, alphaUp) == 0 && commCheck == 0)
        {
            printf("\tSYMBOL[0-%d]:\t%s\tlength = %d\n", tokenLength - 1, symbol, (int)strlen((symbol)));
            // incrememnt the line index to the element immediately after the last char in the symbol name
            lineIndex += tokenLength + 1;

            // count the number of contiguous whitespace characters before the next nonwhitespace character
            wspLength = strspn(line + lineIndex, wsp);
            // advance the lineIndex to skip the whitespace characters
            lineIndex += wspLength;

            // find the size of the opcode (must be upper case alpha characters)
            tokenLength = strspn(line + lineIndex, alphaUp);
            strncpy(opcode, line + lineIndex, tokenLength);

            printf("\tOPCODE[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, opcode, (int)strlen(opcode));

            lineIndex += tokenLength + 1;

            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            // get the length of the operand by counting the number of contiguous NON-whitespace characters
            tokenLength = strcspn(line + lineIndex, wsp);

            strncpy(operand, line + lineIndex, tokenLength);
            printf("\tOPERAND[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, operand, (int)strlen(operand));

            printf("\n");

            // if the directive is the start directive
            if (startCheck == 0 && strcmp(opcode, "START") == 0)
            {
                // log that we have found the start directive
                startCheck = 1;
                char hex[15] = "0X";
                strcat(hex, operand);

                loc_counter = (int)strtol(hex, NULL, 0);
                // check to see if the symbol is in the table
                Symbol *duplicate = ST_get(symbol_table, symbol, &loc_counter, &line_number);
                if (duplicate != NULL)
                {
                    
                    printf("ERROR: duplicate symbol %s already defined on line%d (current line = %d)\n", 
                            duplicate->Name, *(duplicate->SourceLineNumber), line_number);
                    return 1;
                }
                // this executes if the symbol is not already in the table
                ST_set(symbol_table, symbol, &loc_counter, &line_number);
                printf("symbol: %s\topcode: %s\toperand: %s\tloc_counter: %X\n", symbol, opcode, operand, loc_counter);
            }
            else if (startCheck == 0)
            {
                printf("ERROR: START directive not encountered before first directive/instruction on line[%d]\n", line_number);
            }
            else if (startCheck == 1 && isDirective(opcode) == 1)
            {
                Symbol *duplicate = ST_get(symbol_table, symbol, &loc_counter, &line_number);
                if (duplicate != NULL)
                {
                    printf("ERROR: duplicate symbol: %s already defined on line %d (current line = %d\n",
                            duplicate->Name, *(duplicate->SourceLineNumber), line_number);
                    return 1;
                }
                // if there is not a duplicate symbol in the symbol table, add the new symbol to the table
                ST_set(symbol_table, symbol, &loc_counter, &line_number);
                loc_counter = calcDirective(opcode, operand, &loc_counter);
                printf("symbol: %s\tdirective: %s\toperand: %s\tloc_counter: %X\n", symbol, opcode, operand, loc_counter);
            }
            else if (strlen(operand) > 0)
            {
                loc_counter += 3;
                printf("symbol: %s\tinstruction: %s\toperand: %s\tloc_counter: %X\n", symbol, opcode, operand, loc_counter);
            }
        }
        
        // if the line is NOT a symbol definition
        else if (commCheck == 0)
        {
            // count # of contiguous whitespace characters
            wspLength = strspn(line, wsp);
            lineIndex += wspLength;
            // find out the size of the substring consisting of chars in line from line[wspLength] onward 
            //      that are all capital letters
            tokenLength = strspn(line + lineIndex, alphaUp);

            strncpy(opcode, line + lineIndex, tokenLength);
            printf("\tOPCODE[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, opcode, (int)strlen(opcode));
            
            // incrememnt lineIndex after the opcode token
            lineIndex += tokenLength + 1;
            wspLength = strspn(line + lineIndex, wsp);
            // incrememnt lineIndex past the whitespace chars in line after the opcode
            lineIndex += wspLength;
            tokenLength = strcspn(line + lineIndex, wsp);

            strncpy(operand, line + lineIndex, tokenLength);
            if (strlen(operand) > 0 && strspn(operand, wsp) == 0 )
            {
                printf("\tOPERAND[%d-%d]:\t%s\tlength = %d\n", lineIndex, lineIndex + tokenLength - 1, operand, (int)strlen(operand));
                if (startCheck == 1 && isDirective(opcode) == 1)
                {
                    printf("\tdirective: %s\toperand: %s\tloc_counter: %X\n", opcode, operand, loc_counter);
                    loc_counter = calcDirective(opcode, operand, &loc_counter);
                }
                // if the directive/opcode token is NOT a directive, increment the location counter by 3 bytes
                else if (startCheck == 1 && isDirective(opcode) == 0)
                {
                    printf("\topcode: %s\t operand: %s\tloc_counter: %X\n", opcode, operand, loc_counter);
                    // increment location counter for the next line
                    loc_counter += 3;

                }
                else
                {
                    printf("ERROR: not a directive or instruction on line: %d", line_number);
                    return 1;
                }
            }
            // this happens when only an opcode is given with no operand
            else
            {
                printf("\tNO OPERAND INSTRUCTION\n");
            }

        }
        // this block executes when a comment has been found in a line
        else 
        { 
            printf("\tcommCheck = %d", commCheck);
            notComm = strcspn(line, comment);
            printf("\t[%d]\t%.*s\n",notComm, notComm, line);
            printf("-----was comment\n");
        }
        lineNumber++;
        printf("\n");
        
        
	}
    // ST_print(symbol_table);
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


