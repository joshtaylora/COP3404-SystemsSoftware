#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define TABLE_SIZE 26

FILE *outputFile;

typedef struct Symbol {
    char *Name; // character array to store name of symbol
    int Address;
    int SourceLineNumber;
    struct Symbol *next; // pointer to the next symbol in the linked list
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

Symbol *ST_get(SymbolTable *hashtable, char *name, int address, int sourceline) {
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
        // grab the entry in the hashtable at index 1 
        //      - Each index corresponds to an entry with a symbol name starting with a letter in the alphabet
        //      - If there are more than one symbols starting with that letter, the entry links to the next entry
        //          in the linked list using entry->next
    for (int i = 0; i < TABLE_SIZE; i++) {
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
    printf("ASSEMBLY ERROR:\n");
    printf("%s\n", line);
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

// validate the opcodes
int opcodeCalc(char* line, char *opcode, int lineNumber) 
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

int calcByte(char* line, char*operand, int loc_counter, int lineNumber)
{
    // BYTE C'character_sequence'
    if (operand[0] == 67)
    {
        char *opChar = strtok(operand, "\'");
        opChar = strtok(NULL, "\'");
        int charLength = strlen(opChar);
        return loc_counter + charLength;
    }
    // BYTE X'hex_sequence'
    else if (operand[0] == 88)
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
                printf("\tchar = %c\n", opHex[i]);
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
int calcDirective(char* line, char *directive, char *operand, int loc_counter, int lineNumber) {
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
        if (opVal >= 8388607)
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

int main(int argc, char *argv[]) {
    
    // check if a file was passed
    if( argc != 2 ) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }
    
    FILE *inputFile;
	inputFile = fopen( argv[1], "r");
    // open the output file for reading and writing
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
    //printf("SYMBOL TABLE CREATED\n");
    // use st_set(symbol_table, "SYMBOL", loc_counter) to set new entries in the symbol table


	//read the file, line by line
	char line[1024];

	int length;
    // counter that checks if we have encountered the START directive yet
    int startCheck = 0;
    // make sure that there is an END directive (and not multiple ones)
    int endCheck = 0;

    // length of contiguous whitespace, length of token
    int wspLength, tokenLength;
    // int to check if a comment is on a line, int to see how much of a line is not a comment
    int commCheck, notComm;
    // index of current position in the line
    int lineIndex;
    // current line number for line we are comparing
    int lineNumber = 1;
    // location counter values for the start and end directives
    int startLoc, endLoc;

    // allocate memory for a char array that will store the program label
    char* programLabel = malloc(sizeof(char)*6);
	
    // fgets returns a null pointer if the EOF char is read
	while(fgets(line, 1024, inputFile) != NULL)
    {
        // allocate memory for the token strings
        char* symbol = malloc(6 * sizeof(char));
        char* opcode = malloc(10 * sizeof(char));
        char* operand = malloc(10 * sizeof(char));

        

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
            strncpy(opcode, line + lineIndex, tokenLength);

            lineIndex += tokenLength + 1;

            wspLength = strspn(line + lineIndex, wsp);
            lineIndex += wspLength;
            // get the length of the operand by counting the number of contiguous NON-whitespace characters
            tokenLength = strcspn(line + lineIndex, wsp);

            strncpy(operand, line + lineIndex, tokenLength);

            // check for end directive in this line
            if (strcmp(opcode, "END") == 0 && endCheck == 0)
            {
                endCheck++;
            }
            else if (strcmp(opcode, "END") == 0 && endCheck > 0)
            {
                printf("ASSEMBLY ERROR\n");
                printf("%s\n", line);
                printf("Line %d: multiple END directives encounterd\n", lineNumber);
                return 1;
            }

            

            // if the directive is the start directive and we haven't already encountered the start directive
            if (startCheck == 0 && strcmp(opcode, "START") == 0)
            {
                
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
                Symbol *duplicate = ST_get(symbol_table, symbol, loc_counter, lineNumber);
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
                //printf("%s\t%X\n", symbol, loc_counter); // comment out for pass 2
                // writes the symbol name to the file
                fprintf(outputFile, "%s\t", symbol);
                // write the opcode to the file
                fprintf(outputFile, "%s\t", opcode);
                // write the operand (location counter starting address) to the file
                fprintf(outputFile, "%s\t",operand);
                // write the location counter address to the file
                fprintf(outputFile, "%X\n", loc_counter);
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
                

                if (loc_counter >= 0x8000)
                {
                    errorPrint(line);
                    printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                    return 1;
                }
                Symbol *duplicate = ST_get(symbol_table, symbol, loc_counter, lineNumber);
                if (duplicate != NULL)
                {
                    printf("ASSEMBLY ERROR\n");
                    printf("%s\n", line);
                    printf("Line %d:  duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    fprintf(outputFile, "Line %d: duplicate symbol: %s already defined on line %d\n",
                            lineNumber, duplicate->Name, duplicate->SourceLineNumber);
                    return 1;
                }
                // if there is not a duplicate symbol in the symbol table, add the new symbol to the table
                ST_set(symbol_table, symbol, loc_counter, lineNumber);
                // write the symbol to the file
                fprintf(outputFile, "%s\t", symbol);
                // write the opcode to the file
                fprintf(outputFile, "%s\t", opcode);
                // write the operand (location counter starting address) to the file
                fprintf(outputFile, "%s\t", operand);
                //printf("%s\t%X\n", symbol, loc_counter); // comment out for pass 2
                fprintf(outputFile, "%X\n", loc_counter);
                loc_counter = calcDirective(line, opcode, operand, loc_counter, lineNumber);
                if (loc_counter == 0)
                {
                    return 1;
                }
            }
            // if the opcode is an instruction and not a directive
            else if (startCheck == 1 && strlen(opcode) > 0 && strlen(operand) > 0)
            {
                if (loc_counter >= 0x8000)
                {
                    errorPrint(line);
                    printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                    return 1;
                }
                int opcodeHex = opcodeCalc(line, opcode, lineNumber);
                if (opcodeHex < 0 )
                {
                    errorPrint(line);
                    printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                    return 1;
                }
                

                // check if the symbol is already in the table
                Symbol *duplicate = ST_get(symbol_table, symbol, loc_counter, lineNumber);
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
                // if there is not a duplicate symbol in the symbol table, add the new symbol to the table
                ST_set(symbol_table, symbol, loc_counter, lineNumber);
                //printf("%s\t%X\n", symbol, loc_counter); // comment out for Pass 2
                // write the symbol to the file
                fprintf(outputFile, "%s\t", symbol);
                fprintf(outputFile, "%s\t", opcode);
                fprintf(outputFile, "%s\t", operand);
                fprintf(outputFile, "%X\n", loc_counter);
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
                fprintf(outputFile, "Line %d: ERROR: assembly line syntax not valid\n", lineNumber);
                return 1;
            }
            
        }
        
        // if the line is NOT a symbol definition and NOT a comment line
        else if (commCheck == 0 && startCheck == 1)
        {
            if (loc_counter >= 0x8000)
            {
                errorPrint(line);
                printf("Line %d: location counter has gone over the maximum address available for a SIC program\n", lineNumber);
                return 1;
            }
            // count # of contiguous whitespace characters from starting index of line
            wspLength = strspn(line, wsp);
            // increment index in the line past the whitespace
            lineIndex += wspLength;
            // find out the size of the substring consisting of chars in line from line[wspLength] onward 
            //      that are all capital letters
            tokenLength = strspn(line + lineIndex, alphaUp);
            // copy the chars in the line that make up the opcode
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

            // print a tab character to the file followed by the instruction
            //fprintf(outputFile, "\t%s\t", opcode);

            // incrememnt lineIndex after the opcode token
            lineIndex += tokenLength + 1;
            wspLength = strspn(line + lineIndex, wsp);
            // incrememnt lineIndex past the whitespace chars in line after the opcode
            lineIndex += wspLength;
            // find how long the operand characters are in the line
            tokenLength = strcspn(line + lineIndex, wsp);
            // copy characters in the line from lineIndex to the end of the operand
            strncpy(operand, line + lineIndex, tokenLength);

            // if the length of the operand is greater than 0 and there is no whitespace in the operand string
            // -- this happens when we have a non symbol definition 
            if (strlen(opcode) > 0 && strlen(operand) > 0 && strspn(operand, wsp) == 0 )
            {
                //fprintf(outputFile, "%s\t", operand);
                
                // ensure that the start directive has already been found and that the opcode is a directive
                //  -- happens when we have directive opcode that is not on a symbol definition line
                if (isDirective(opcode) == 1)
                {
                    fprintf(outputFile, "%s\t", opcode);
                    fprintf(outputFile, "%s\t", operand);
                    fprintf(outputFile, "%X\n", loc_counter);
                    // must increase the loc_counter for the line after this line
                    loc_counter = calcDirective(line, opcode, operand, loc_counter, lineNumber);
                }
                // if the opcode token is NOT a directive, increment the location counter by 3 bytes
                else if (isDirective(opcode) == 0)
                {
                    int opcodeHex = opcodeCalc(line, opcode, lineNumber);
                    if (opcodeHex < 0 )
                    {
                        errorPrint(line);
                        printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                        return 1;
                    }

                    // print the opcode for the instruction to the file
                    fprintf(outputFile, "\t%s\t", opcode);
                    //fprintf(outputFile, "%.*X\t", 2, opcodeHex);
                    fprintf(outputFile, "%s\t", operand);
                    //printf("\topcode: %s\t operand: %s\tloc_counter: %X\n", opcode, operand, loc_counter);
                    fprintf(outputFile, "%X\n", loc_counter);
                    // increment location counter for the next line
                    loc_counter += 3;

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
                int opcodeHex = opcodeCalc(line, opcode, lineNumber);
                if (opcodeHex < 0 )
                {
                    errorPrint(line);
                    printf("Line %d: Line contains an invalid opcode mnemonic: %s\n", lineNumber, opcode);
                    return 1;
                }

                // print the opcode for the instruction to the file
                fprintf(outputFile, "\t%s\t", opcode);
                //fprintf(ooutputFile, "%.*X\t", 2, opcodeHex);
                fprintf(outputFile, "(null)\t");
                fprintf(outputFile, "%X\n",loc_counter);
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
           continue; 
        }
        lineNumber++;
        //printf("\n");
        fprintf(outputFile, "\n");
        
        
	}

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

    //fprintf(outputFile, "START: %X\tEND: %X\n", startLoc, endLoc);

	// close the opened file
	fclose( inputFile );
    

    // --------------------------------------------------------------------------------------
    //                                  PASS 2 START
    // --------------------------------------------------------------------------------------
    // make the correct output file name
    char* objFilename = strcat(strtok(argv[1],"."), ".obj");
    // open the output file for writing
    FILE* objFile;
    objFile = fopen(objFilename, "w");
    
    int programSize = endLoc - startLoc;
    //printf("Program Size: %X\n", programSize); // prints the size of the program for debug
    int hIndex = 0;

    // if the number of characters in the label is not 6 then we need to add spaces after
    // the label name to get the proper header record format
    if (strlen(programLabel) < 6)
    {   
        for (int i = 0; i <= 6 - strlen(programLabel); i++)
        {
            strcat(programLabel, " ");
        }
        //printf("ProgramLabel: |%s|\n", programLabel);
    }
    char* hRecord = malloc(sizeof(char) * 20);
    hRecord[0] = 'H';
    // increment the index of the hrecord we are at
    hIndex++;
    for (int j = 0; j < strlen(programLabel); j++)
    {
        hRecord[hIndex] = programLabel[j];
        hIndex++;
    }
    //printf("hRecord: %s\thIndex: %d\n", hRecord, hIndex); // debug the h record so far
    
    // index 7 - 12 will be the starting address of the program (in Bytes)
    char* hStartChar = malloc(sizeof(char) * 6);
    // this only works since we are garunteed to have a starting location in SIC less than
    // 6 digits long -> will need to fix for the XE version
    sprintf(hStartChar, "00%X", startLoc); // save the startLoc value as a string with the right format
    hIndex += strlen(hStartChar); // increment the hRecord index based on the number of char's added
    strcat(hRecord, hStartChar); // concatenate to the header record

    //printf("hRecord: %s\n", hRecord); // print the h record for debug
    //fprintf(objFile, "H%s", );

    // hRecord index 13 - 18 are the Length of the object program in bytes (hex)
    char* progLengthChar = malloc(sizeof(char) * 6);
    // will store the updated size
    char* progSize = malloc(sizeof(char) * 6);
    // copy the programSize variable as a hex int to the string progLengthChar
    sprintf(progLengthChar, "%X", programSize);
    if (strlen(progLengthChar) < 6)
    {
        if (6 - strlen(progLengthChar) == 1)
        {
            sprintf(progSize, "0");
            strcat(progSize, progLengthChar);
            strcat(hRecord, progSize);
        }
        else if (6 - strlen(progLengthChar) == 2)
        {
            sprintf(progSize, "00");
            strcat(progSize, progLengthChar);
            strcat(hRecord, progSize);
        }
        else
        {
            printf("Program is full 6 digits long");
        }
    }
    else
    {
        strcat(hRecord, progLengthChar);
    }
    //printf("H Record: %s\n", hRecord);
    /* //uncomment out to print each character in the hrecord so far
    for (int x = 0; x < strlen(hRecord); x++)
    {
        printf("H[%d]: %c\n", x + 1, hRecord[x]);
    }
    */
    fprintf(objFile, "%s\n", hRecord); // print the h record to the
    // object file
    // ------------------------------------------------------------
    // -------------------- Begin T Records -----------------------
    // ------------------------------------------------------------
    // allocate space for the t record
    char *tRecord = malloc(sizeof(char) * 69); // will store the values for the hRecord

    // rewind the output file pointer to point to the first line in the file
    rewind(outputFile);
    int fileIndex = 0;
    // Loop through the output file line by line
    
    // close open objFile
    fclose(objFile);
    fclose(outputFile);
    return 0;
    
}


