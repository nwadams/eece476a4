/*
 * EECE 476
 *  Your name(s) here
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tracebp.h"

int main(int argc, char* argv[])
{
	int num_taken = 0;
	int num_not_taken = 0;   
	int num_mispredictions = 0;

	int bp_pht[32768];
	memset( bp_pht, 0, sizeof(bp_pht) ); // initialize to 0

	// open the trace file
	char trace_filename[MAX_STR];
	if (argc > 1) {
		sscanf(argv[1], "%s", trace_filename);
	} else {
		printf("Usage: %s <trace_file>\n", argv[0]);
		return 1;
	}

	initInstructionStream( trace_filename );
	instruction prev_instruction;
	prev_instruction.op[0] = 0;
	instruction current_instruction;

	while( getNextInstruction( &current_instruction ) ) {

		// Was the prev instruction a conditional branch?
		// Each instruction in this architecture is 8 bytes long
		if( isBranch(prev_instruction.op) ) {

			unsigned index = ((unsigned)(long)current_instruction.pc >> 3) & ((1<<15)-1);
			assert(index < 32768);
			int prediction = bp_pht[ index ];

			// compute actual branch outcome
			int br_taken = (current_instruction.pc != prev_instruction.pc + 8);
			if( br_taken )
				++num_taken;
			else
				++num_not_taken;

			if( prediction != br_taken ) num_mispredictions++;
			bp_pht[ index ] = br_taken;


		}
		prev_instruction = current_instruction;

	}

	closeInstructionStream();

	// OFFICIAL PRINT STATEMENTS FOR AUTO-MARKING
	// DO NOT PRINT ANYTHING ELSE BEFORE THESE LINES!!
	// DO NOT REMOVE THESE LINES!!!
	printBP( num_taken, num_not_taken, num_mispredictions );

	// Print your own information and statistics after this line

	return 0;
}
