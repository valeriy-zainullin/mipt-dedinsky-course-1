#include "blend.h"

#include <assert.h>
#include <cpuid.h>
#include <emmintrin.h>
#include <limits.h>
#include <stdbool.h>

// Источники:
//   https://stackoverflow.com/questions/14266772/how-do-i-call-cpuid-in-linux
//   https://github.com/gcc-mirror/gcc/blob/master/gcc/config/i386/cpuid.h
//   https://en.wikipedia.org/wiki/CPUID
bool blend_check_sse_supported() {
	static const unsigned int BASIC_CPUID_INFO = 0x0;
	unsigned int cpuid_max_leaf = __get_cpuid_max(BASIC_CPUID_INFO, NULL);
	
	if (cpuid_max_leaf == 0) {
		// CPUID is not supported, then SSE is not supported either.
		return false;
	}
	
	static const unsigned int PROC_INFO_AND_FEATURES_LEAF = 0x1;
	
	if (cpuid_max_leaf < PROC_INFO_AND_FEATURES_LEAF) {
		return false;
	}
	
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
		
	if (!__get_cpuid(PROC_INFO_AND_FEATURES_LEAF, &eax, &ebx, &ecx, &edx)) {
		// Unsupported leaf or cpuid is not supported.
		return false;
	}
	
	if ((ecx & bit_SSE) != 0) {
		return true;
	}
	
	return false;
}

void blend_sse(struct rgba * buffer, struct blend_pictures const * pictures) {
	(void) pictures;
	
	for (size_t row = 0; row < SCREEN_ROWS; ++row) {
		for (size_t col = 0; col < SCREEN_COLS; ++col) {
			size_t pos = row * SCREEN_COLS + col;

			struct rgba color = {255, 0, 0, 255};
			buffer[pos] = color;
		}
	}
}
