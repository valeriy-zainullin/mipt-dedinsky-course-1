#include "builds.h"
#include "compute.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TO_STR2(MACRO) #MACRO
#define TO_STR(MACRO) TO_STR2(MACRO)

#define MEASURE(FEATURE_CHECK_EXPR, NAME, PADDING, EXPR) \
	if (FEATURE_CHECK_EXPR) {                                                                                                \
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);                                                                     \
		EXPR;                                                                                                                \
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);                                                                       \
		long nanoseconds = end.tv_nsec - start.tv_nsec;                                                                      \
		static long DIVISOR_TO_MS = 1000 * 1000;                                                                             \
		printf("%" TO_STR(PADDING) "s: %04ld.%06ld ms.\n", NAME, nanoseconds / DIVISOR_TO_MS, nanoseconds % DIVISOR_TO_MS);  \
	} else {                                                                                                                 \
		printf("%" TO_STR(PADDING) "s: not supported\n", NAME);                                                              \
	}

int main() {
	struct timespec start = {0};
	struct timespec end = {0};
	
	const struct screen_state state = INITIAL_SCREEN_STATE;
	
	struct rgba * buffer = calloc(SCREEN_ROWS * SCREEN_COLS, sizeof(struct rgba));
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return 1;
	}
	
	// Test nosse version.
	#if BUILD == BUILD_RELEASE
		MEASURE(true, "Plain (-O2)", 11, compute_nosse(buffer, &state));
	#elif BUILD == BUILD_DEBUG
		MEASURE(true, "Plain (-Og)", 11, compute_nosse(buffer, &state));
	#else
		MEASURE(true, "Plain (-O?)", 11, compute_nosse(buffer, &state));
	#endif
	
	// Test sse version.	
	MEASURE(compute_check_sse_supported(), "SSE", 11, compute_sse(buffer, &state));
	
	// Test avx version.
	MEASURE(compute_check_avx_supported(), "AVX", 11, compute_avx(buffer, &state));

	return 0;
}