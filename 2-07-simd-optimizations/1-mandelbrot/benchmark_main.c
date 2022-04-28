#include "builds.h"
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

	// TODO: сделать define для замера времени.
	{
		// Test nosse version.	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		compute_nosse(buffer, &state);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		long nanoseconds = end.tv_nsec - start.tv_nsec;
		static long DIVISOR_TO_MS = 1000 * 1000;
		// Write -Og for dbg version?
		#if BUILD == BUILD_RELEASE
			printf("Plain (-O2)");
		#elif BUILD == BUILD_DEBUG
			printf("Plain (-Og)");
		#else
			printf("Plain (-O?)");
		#endif
		printf(": %04ld.%06ld ms.\n", nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS); 
	}

	if (compute_check_sse_supported()) {
		// Test sse version.	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		compute_sse(buffer, &state);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		long nanoseconds = end.tv_nsec - start.tv_nsec;
		static long DIVISOR_TO_MS = 1000 * 1000;
		printf("        SSE: %04ld.%06ld ms.\n", nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS); 
	} else {
		printf("        SSE: not supported.\n"); 
	}

	if (compute_check_avx_supported()) {
		// Test avx version.	
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		compute_avx(buffer, &state);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		long nanoseconds = end.tv_nsec - start.tv_nsec;
		static long DIVISOR_TO_MS = 1000 * 1000;
		printf("        AVX: %04ld.%06ld ms.\n", nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS); 
	} else {
		printf("        AVX: not supported.\n"); 
	}
	
	return 0;
}