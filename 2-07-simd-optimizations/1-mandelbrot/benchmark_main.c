#include "compute.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	struct timespec start = {0};
	struct timespec end = {0};
	
	const struct screen_state state = INITIAL_SCREEN_STATE;
	
	struct rgba * buffer = calloc(SCREEN_ROWS * SCREEN_COLS, sizeof(struct rgba));
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return 1;
	}

	{
		// Test nosse version.	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		compute_nosse(buffer, &state);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		long nanoseconds = end.tv_nsec - start.tv_nsec;
		static long DIVISOR_TO_MS = 1000 * 1000;
		// Write -Og for dbg version?
		printf("Plain -O2: %04ld.%06ld ms.\n", nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS); 
	}

	{
		// Test sse version.	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		compute_sse(buffer, &state);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		long nanoseconds = end.tv_nsec - start.tv_nsec;
		static long DIVISOR_TO_MS = 1000 * 1000;
		printf("      SSE: %04ld.%06ld ms.\n", nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS); 
	}

	
	return 0;
}