#pragma once

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

// Works only with gcc.

#define M128I_ALIGNED __attribute__((aligned(16)))
#define M256I_ALIGNED __attribute__((aligned(32)))

#if !defined(MAY_BE_UNUSED)
#define MAY_BE_UNUSED __attribute__((unused))
#endif

MAY_BE_UNUSED static void print_m128i_pu8(__m128i reg) {
	uint8_t array[sizeof(reg) / sizeof(uint8_t)] M128I_ALIGNED = {0};
	_mm_store_si128((__m128i*) array, reg);
	
	printf("{");
	bool is_first = true;
	for (size_t i = 0; i < sizeof(array) / sizeof(*array); ++i) {
		if (is_first) {
			is_first = false;
		} else {
			printf(",");
		}
		
		printf("%03" PRIu8, array[i]);
	}
	printf("}");
}

MAY_BE_UNUSED static void print_m128i_pi16(__m128i reg) {
	uint16_t array[sizeof(reg) / sizeof(uint16_t)] M128I_ALIGNED = {0};
	_mm_store_si128((__m128i*) array, reg);
	
	printf("{");
	bool is_first = true;
	for (size_t i = 0; i < 128 / (CHAR_BIT * sizeof(uint16_t)); ++i) {
		if (is_first) {
			is_first = false;
		} else {
			printf(",");
		}
		
		printf("%05" PRIu16, array[i]);
	}
	printf("}");
}

MAY_BE_UNUSED static void print_m256i_pu8(__m256i reg) {
	uint8_t array[sizeof(reg) / sizeof(uint8_t)] M256I_ALIGNED = {0};
	_mm256_store_si256((__m256i*) array, reg);
	
	printf("{");
	bool is_first = true;
	for (size_t i = 0; i < 256 / (CHAR_BIT * sizeof(uint8_t)); ++i) {
		if (is_first) {
			is_first = false;
		} else {
			printf(",");
		}
		
		printf("%03" PRIu8, array[i]);
	}
	printf("}");
}

MAY_BE_UNUSED static void print_m256i_pu16(__m256i reg) {
	uint16_t array[sizeof(reg) / sizeof(uint16_t)] M256I_ALIGNED = {0};
	_mm256_store_si256((__m256i*) array, reg);
	
	printf("{");
	bool is_first = true;
	for (size_t i = 0; i < 256 / (CHAR_BIT * sizeof(uint16_t)); ++i) {
		if (is_first) {
			is_first = false;
		} else {
			printf(",");
		}
		
		printf("%05" PRIu16, array[i]);
	}
	printf("}");
}
