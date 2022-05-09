#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SCREEN_COLS 4
//800
#define SCREEN_ROWS 1
//600

struct rgba {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct blend_pictures {
	struct rgba background[SCREEN_ROWS * SCREEN_COLS];
	struct rgba foreground[SCREEN_ROWS * SCREEN_COLS];
};

struct blend_pictures* blend_pictures_new(char const* background_path, char const* foreground_path);
struct blend_pictures* blend_pictures_delete();

#if defined(__cplusplus)
extern "C" {
#endif

void blend_nosse(struct rgba * buffer, struct blend_pictures const * pictures);

bool blend_check_sse_impl_supported();
void blend_sse(struct rgba * buffer, struct blend_pictures const * pictures);

bool blend_check_avx_impl_supported();
void blend_avx(struct rgba * buffer, struct blend_pictures const * pictures);

#if defined(__cplusplus)
}
#endif