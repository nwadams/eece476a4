/*
 *  An API for getting the next instuction in a program's
 *  executed instruction stream
 *  EECE 476 2012
 */

///////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tracebp.h"


static int chunk_size = GZ_CHUNK_SIZE;
static gzFile* trace_file = NULL;
static char* gzBuff = NULL;
static int offset = 0;


/////////////////

// All possible load/store/branch instructions
// For a complete description see:
// http://www.simplescalar.com/docs/users_guide_v2.pdf

#define NUM_DIFFERENT_LOADS 8
static char* load_instrs[NUM_DIFFERENT_LOADS] = {
	 "lb", "lbu", "lh", "lhu", "lw", "dlw", "l.s", "l.d"
 };

// Functions to determine whether an op is a load or store

int isLoad( char* op )
{
	int i;
	for ( i = 0; i < NUM_DIFFERENT_LOADS; ++i ) {
		if (strstr(op, load_instrs[i]) != NULL) {
			return 1;
		}
	}
	return 0;
}


#define NUM_DIFFERENT_STORES 8
static char* store_instrs[NUM_DIFFERENT_STORES] = {
	"sb", "sbu", "sh", "shu", "sw", "dsw", "s.s", "s.d"
};

int isStore( char* op )
{
	int i;
	for ( i = 0; i < NUM_DIFFERENT_STORES; ++i ) {
		if (strstr(op, store_instrs[i]) != NULL) {
			return 1;
		}
	}
	return 0;
}


// All possible conditional branch instructions
#define NUM_DIFFERENT_BRANCHES 8
static char* cond_branch_instrs[8] = {
	"beq", "bne", "blez", "bgtz", "bltz", "bgez", "bc1f", "bc1t"
};

int isBranch( char* op )
{
    int i;
    for ( i = 0; i < NUM_DIFFERENT_BRANCHES; ++i ) {
        if (strstr(op, cond_branch_instrs[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

////////////////////////////


void initInstructionStream(char* traceFilename)
{
    offset = 0;
    gzBuff = malloc(GZ_CHUNK_SIZE);
    trace_file = gzopen(traceFilename, "r");
    if( !trace_file ) {
        printf("Error - Cannot open \"%s\"\n", traceFilename);
    }
}

int getNextInstruction( instruction* next_instruction )
{
    if ( offset == 0 || offset >= chunk_size ) {
        chunk_size = gzread(trace_file, gzBuff, GZ_CHUNK_SIZE);
        if ( chunk_size == 0 ) {
            return 0;
        }
        offset = 0;
    }

    char* next_line = strchr( gzBuff + offset, '\n' );
    if ( next_line ) {
        sscanf( gzBuff + offset,
                "%s\t0x%08p\t0x%08p\n",
                next_instruction->op,
                &next_instruction->pc,
                &next_instruction->addr );
        offset = next_line + 1 - gzBuff;
    } else {
        char temp[MAX_STR];
        strncpy(temp, gzBuff + offset, chunk_size - offset);
        temp[chunk_size - offset] = '\0';
        chunk_size = gzread(trace_file, gzBuff, GZ_CHUNK_SIZE);
        if ( chunk_size > 0) {
            offset = 0;
            next_line = strchr( gzBuff + offset, '\n' );
            strncat( temp, gzBuff, next_line - gzBuff );
            sscanf( temp,
                "%s\t0x%08p\t0x%08p\n",
                next_instruction->op,
                &next_instruction->pc,
                &next_instruction->addr );
            offset = next_line + 1 - gzBuff;
        } else {
            return 0;
        }
    }
    return 1;
}

void closeInstructionStream()
{
    gzclose(trace_file);
    trace_file = NULL;
    free(gzBuff);
    gzBuff = 0;
}


void printAccesses( int refs, int instrs, int loads, int stores )
{
	printf( "Number of memory accesses     = %d\n", refs   );
	printf( "Number of instructions        = %d\n", instrs );
	printf( "Number of data load accesses  = %d\n", loads  );
	printf( "Number of data store accesses = %d\n", stores );
}


void printCacheRates( char *type, int num_instrs, int misses, int refs )
{
	float percent = 100.0 * misses / refs ;
	float MKPI    = 1000.0 * misses / num_instrs ;
	printf( "%s-cache miss rate = %.4f%%\n", type, percent  );
	printf( "%s-cache MKPI = %.4f\n", type, MKPI );
}

void printBP( int num_taken, int num_not_taken, int num_mispredictions )
{
	int num_branches = num_taken + num_not_taken;
	printf( "Branches %d, Taken %d (%.4f), Not Taken %d (%.4f), Prediction Accuracy %.4f\n",
		num_branches,
		num_taken,
		1.*num_taken/num_branches,
		num_not_taken,
		1.*num_not_taken/num_branches,
		1.*(num_branches-num_mispredictions)/num_branches
	 );
}
