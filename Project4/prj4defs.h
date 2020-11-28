typedef struct Symbol {
    char *Name; // character array to store name of symbol
    int Address;
    int SourceLineNumber;
    struct Symbol *next; // pointer to the next symbol in the linked list
} Symbol;
typedef struct SymbolTable {
    Symbol **TableEntries; // pointer to Symbol pointers (acts as an array of pointers)
} SymbolTable;
typedef struct Instruction
{
    char* mnemonic;
    int opcode;
    int* format;
} Instruction;
typedef struct OpcodeTable
{
    Instruction** Instructions;
} OpcodeTable;
typedef struct AddrModeTable {
    int** format3;
    int** format4;
} AddrModeTable;
struct Line {
    int lineNumber;
    int location;
    char* sourceLine;
    char* label;
    char* opcode;
    char* operand;
};
typedef struct Line DirectiveLine;
typedef struct Line InstructionLine;
AddrModeTable* AMT_create(void);
Instruction* Instruction_Alloc(char* opName, int opHex, int* formats);
OpcodeTable* OPTAB_create(void);
Instruction* OPTAB_Search(OpcodeTable* instructionTable, char* searchName);
SymbolTable *ST_create( void );
Symbol *Symbol_alloc(  char *name, int address, int sourceline );
void ST_set( SymbolTable *hashtable, char *name, int address, int sourceline);
Symbol *ST_get(SymbolTable *hashtable, char *name);
void ST_print( SymbolTable *hashtable );
void errorPrint(char* line);
int getInstructionFormat(Instruction* opcode, char* operand);
int isDirective(char *possibleDirec);
int opcodeCalc(char *opcode);
int calcByte(char* line, char* operand, int loc_counter, int lineNumber);
int calcDirective(char* line, char *directive, char *operand, int loc_counter, int lineNumber);
const char* convertCharConst(char *optab);