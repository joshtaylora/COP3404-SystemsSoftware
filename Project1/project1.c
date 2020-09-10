#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1000

// possible idea is to make the hash table only length = 26
// each entry represents a  
struct symbol {
    int address; // 
    int SourceLineDefinition; // line of the input file the symbol is defined on
    char name[8]; // name of the symbol max 6 chars and the 7th is the '0' null char
} symbol;

// The hash function
unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; i++) {
        value = value *37 + key[i];
    }

    value = value % TABLE_SIZE;

    return value;
}

// creates a linked list of entries, each one with a pointer to the next one
typedef struct entry_list {
    char *key; // the key is the symbol name ( possible error = duplicate symbol definitions )
    char *value; // the value is the location in memory
    struct entry_list *next; // pointer to the next entry in the symbol table
} entry_list;

// the symbol table acts as an array of linked lists
typedef struct {
    entry_list **entries; // array of pointers to the linked lists of entries
} symbol_table;

// function that initializes the symbol table and allocates the required memory
symbol_table *st_create(void) {
    // allocate memory for the table
    symbol_table *hashtable = malloc(sizeof(symbol_table) * 1);

    // allocate memory for the table entries
    hashtable->entries = malloc(sizeof(entry_list*) *TABLE_SIZE);
    
    // when we create the symbol table, we want all of the entries to be set to null so we can 
    // perform table lookup very easily
    int i = 0;
    for (; i < TABLE_SIZE; i++) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

entry_list *st_pair(const char *key, const char *value) {
    // allocate the entry
    entry_list *entry = malloc(sizeof(entry) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next entry starts out null but can be set later on
    entry->next = NULL;
    
    return entry;
}

void st_set(symbol_table *hashtable, const char *key, const char *value) {
    // generate a hash using the key and create a bucket in case we have matching hashes
    unsigned int bucket = hash(key);

    entry_list *entry = hashtable->entries[bucket];

    // if there is no entry at the hash, create a new bucket with 1 entry and no next entry yet
    if (entry == NULL) {
        hashtable->entries[bucket] = st_pair(key, value);
        return;
    }

    entry_list *prev;

    while (entry != NULL) {
        // check key (if the keys match)
        if (strcmp(entry->key, key) == 0) {
            // match found, replace the value
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }
        
        // walk to next entry and update next values
        prev = entry;
        entry = prev->next;
    }
    
    // end of the linked list reached without a match, add a new entry
    prev->next = st_pair(key, value);
}

// function to get values ( memory locations ) given a key ( symbol name )
char *st_get(symbol_table *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot in the table using the hash for the given key
    entry_list *entry = hashtable->entries[slot];

    // no slot with that hash means the entry is not in the symbol table
    if (entry == NULL) {
        return NULL;
    }

    while (entry != NULL) {
        // match found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        
        // proceed to the next key (if possible) and try again
        entry = entry->next;
    }
    // no matching entry found
    return NULL;
}

void st_print(symbol_table *hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        entry_list *entry = hashtable->entries[i];

        if (entry == NULL) { continue; }
        printf("slot[%4d]: ", i);

        for (;;) {
            printf("%s=%s ", entry->key, entry->value);
            if (entry->next == NULL) { break; }
            entry = entry->next;
        }
        printf("\n");
    }
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
	char *locCounter;

    // initialize the symbol table
    symbol_table *symtab = st_create();
    // use st_set(symtab, "SYMBOL", locCounter) to set new entries in the symbol table

    int isDirective(char *possibleDirec) {
        if (strcmp(possibleDirec, "START")) { return 1; }
        if (strcmp(possibleDirec, "END")) { return 1; }
        if (strcmp(possibleDirec, "BYTE")) { return 1; }
        if (strcmp(possibleDirec, "WORD")) { return 1; }
        if (strcmp(possibleDirec, "RESB")) { return 1; }
        if (strcmp(possibleDirec, "RESW")) { return 1; }
        if (strcmp(possibleDirec, "RESR")) { return 1; }
        if (strcmp(possibleDirec, "EXPORTS")) { return 1; }
    }
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
