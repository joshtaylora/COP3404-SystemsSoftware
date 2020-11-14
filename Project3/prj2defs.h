typedef struct Symbol {
    char *Name; // character array to store name of symbol
    int Address;
    int SourceLineNumber;
    struct Symbol *next; // pointer to the next symbol in the linked list
} Symbol;

typedef struct SymbolTable {
    Symbol **TableEntries; // pointer to Symbol pointers (acts as an array of pointers)
} SymbolTable;

SymbolTable *ST_create( void );
Symbol *Symbol_alloc(  char *name, int address, int sourceline );
void ST_set( SymbolTable *hashtable, char *name, int address, int sourceline);
Symbol *ST_get(SymbolTable *hashtable, char *name);
void ST_print( SymbolTable *hashtable );
void errorPrint(char* line);
int isDirective(char *possibleDirec);
int opcodeCalc(char *opcode);
int calcByte(char* line, char* operand, int loc_counter, int lineNumber);
int calcDirective(char* line, char *directive, char *operand, int loc_counter, int lineNumber);
const char* convertCharConst(char *optab);