#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYM_TABLE_SIZE 1024

struct symbol_node{
    int key;
    int symbol;
    struct symbol_node *next;
};

struct symbol_table{
    int size;
    struct symbol_node **list;
};

struct symbol_table *createSymbolTable(int size) {
    struct symbol_table *t = (struct symbol_table*)malloc(sizeof(struct symbol_table));
    t->size = size;
    t->list = (struct symbol_node**)malloc(sizeof(struct symbol_node*)*size);
    // initialize the symbol table with all entries in the table empty (NULL)
    for (int i=0; i < size; i ++) {
        t->list[i] = NULL;
    }
    return t;
}

int hashFunction(struct symbol_table *t, int key) {
    if (key < 0) { return -(key%t->size); }
    return key%t->size;
}

void insert(struct symbol_table *t, int key, int symbol) {
    int position = hashFunction(t, key);
    struct symbol_node *list = t->list[position];
    struct symbol_node *newSymbol = (struct symbol_node*)malloc(sizeof(struct symbol_node));
    struct symbol_node *temp = list;
    while(temp) {
        if (temp->key==key){
            temp->symbol = symbol;
            return;
        }
        temp = temp->next;
    }
    newSymbol->key = key;
    newSymbol->symbol = symbol;
    newSymbol->next = list;
    t->list[position] = newSymbol;
}

int lookup(struct symbol_table *t, int key) {
    int position = hashFunction(t, key);
    struct symbol_node *list = t->list[position];
    struct symbol_node *temp = list;
    while(temp) {
        if (temp->key==key) { return temp->symbol; }
        temp = temp->next;
    }
    return -1;
}

int main(int argc, char *argv[]) {
	// check to see that a file was passed 
	if ( argc != 2) {
		printf("USAGE: %s <filename>\n", argv[0]);
		return 1;
	}

	// open the file
	FILE *inputFile;
	inputFile = fopen( argv[1], "r");
	// check that the file being read exists (if fopen tries to read from a file that does not exist
	// it returns NULL
	if ( !inputFile ){
		printf("ERROR: %s could not be opened for reading.\n", argv[1]);
		return 1;
	}

    // The Location Counter that tells where we are in memory 
	int LocCounter;

    // initialize the symbol table
    struct symbol_table *t = createSymbolTable(SYM_TABLE_SIZE);
    insert(t, 16, 555);
    insert(t, 20, 777);
    printf("Symbol = %d\n", lookup(t,20));

    // Array of SIC Directives
    // START - specifies the name and starting address of program, routine, or library
    // END - Indicates the end of the program and (optionally) specifies the first executable 
    //      instruction on the program
    // BYTE - Generate character or hexadecimal constant, occupying as many bytes as needed to represent
    //      the constant
    // WORD - generate a one-word integer constant (words in SIC are 3 bytes)
    // RESB - reserve the indicated number of bytes for a data area
    // RESW - reserve the indicated number of words for a data area
    // RESR - reserve space for an external reference address or lubrary location. 3 Bytes
    // EXPORTS - export the symbol address in the object file for cross-file linking. 3 Bytes
    char directives[8][15] =
    { "START",
        "END",
        "BYTE",
        "WORD",
        "RESB",
        "RESW",
        "RESR",
        "EXPORTS"
    };

	//read the file, line by line
	char line[1024];
	// fgets returns a null pointer if the EOF char is read
	while( fgets(line, 1024, inputFile) ){
		printf("READ: %s", line );
		int length;
		length = strlen( line );
		if ( length > 0 ) {
			// check for comment ( 35 = character value for ascii value for # symbol )
			if (line[0] == 35 ) {
				printf("--was a comment\n");
			}

			// check for a symbol definition: must be an alpha character (ASCII 65 through 90 )
			if ( ( line[0] >= 65 ) && ( line[0] <= 90 ) ) {
				printf("--had a symbol definition\n");
			}

			// Tokenize the line
			char *token = strtok( line, " \t");
            if ( ( line[0] >= 65 ) && ( line[0] <= 90 ) ) {

            }
			while ( token ){
				printf("\t----> %s\n", token);
				token = strtok( NULL, " \t");
			}
		}
	}

	// close the opened file
	fclose( inputFile );
}
