// Request POSIX version 2004
// needed for clock_gettime
// and such.
// Worked without this on
// windows (cygwin + mingw).
// Turned out to be needed
// for linux.
#define _XOPEN_SOURCE 600

#include "blend.h"

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
	
	struct blend_pictures * blend_pictures = blend_pictures_new(NULL, NULL);
	
	struct rgba * buffer = calloc(SCREEN_ROWS * SCREEN_COLS, sizeof(struct rgba));
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
		return 1;
	}
	
	// Evaluated 100 times per benchmark, otherwise time stays at 0ms. :)
	// Time difference is not visible. Let it accumulate :)
	printf("Evaluated 100 times per benchmark, otherwise time stays at 0ms. :)\n");
	
	// Test nosse version.
	#if RELEASE_BUILD
		MEASURE(true, "Plain (-O2)", 11,
			for (size_t i = 0; i < 100; ++i) {
				blend_nosse(buffer, blend_pictures);
			}
		);
	#elif DEBUG_BUILD
		MEASURE(true, "Plain (-O0)", 11,
			for (size_t i = 0; i < 100; ++i) {
				blend_nosse(buffer, blend_pictures);
			}
		);
	#else
		MEASURE(true, "Plain (-O?)", 11,
			for (size_t i = 0; i < 100; ++i) {
				blend_nosse(buffer, blend_pictures);
			}
		);
	#endif
	
	// Test sse version.	
	MEASURE(blend_check_sse_impl_supported(), "SSE", 11,
		for (size_t i = 0; i < 100; ++i) {
			blend_sse(buffer, blend_pictures);
		}
	);
	
	// Test avx version.
	MEASURE(blend_check_avx_impl_supported(), "AVX", 11,
		for (size_t i = 0; i < 100; ++i) {
			blend_avx(buffer, blend_pictures);
		}
	);
	
	blend_pictures_delete(blend_pictures);

	return 0;
}
