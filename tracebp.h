/*
 *  An API for getting the next instuction in a program's
 *  executed instruction stream
 *  EECE 476 2012
 */

#define MAX_STR 255
#define GZ_CHUNK_SIZE (8 * 1024)

// An instruction from the trace file
typedef struct instruction_
{
    char op[MAX_STR]; // String based op code (i.e. add, mul, beq, ...)
    void *pc;
    void *addr;
} instruction;


/*
 *  Initialize the instruction stream.
 *  Pass the tracefilename.
 */

void initInstructionStream(char* traceFilename);

/*
 *  Pass in a pointer to the instruction struct
 *  that will be populated.
 *  Returns 1 if there is another instruction
 *  Returns 0 if this is the last instruction
 */

int getNextInstruction( instruction* next_inst );

/*
 * Determine whether the instruction is a load
 * Returns 1 if it is a load
 * Returns 0 if it is NOT a load
 */

int isLoad( char* op );


/*
 * Determine whether the instruction is a store
 * Returns 1 if it is a store
 * Returns 0 if it is NOT a store
 */

int isStore( char* op );


/*
 * Determine whether the instruction is a branch
 * Returns 1 if it is a branch
 * Returns 0 if it is NOT a branch
 */

int isBranch( char* op );


/*
 * Close the last opened instruction stream
 */

void closeInstructionStream();


/*
 * Print the memory access statistics.
 */

void printAccesses( int num_memory_accesses, int num_instrs, int num_data_load_accesses, int num_data_store_accesses );

/*
 * Print the cache access statistics.
 *    NB: type = "i" or "d"
 */

void printCacheRates( char *type, int num_instrs, int num_misses, int num_accesses );

/*
 * Print the branch and predictor outcomes.
 */

void printBP( int num_taken, int num_not_taken, int num_mispredictions );


