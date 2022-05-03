#include "blend.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Great that we don't export these functions (blend_color_component and blend_colors), as they accept structures.
// These structures are 32-bit, better to pass them, than pointers :)
//
// But if color pallete would change, more colors would be supported, serious copying might occur (more than 4 or 8
// bytes, which are common register sizes).
//
// Linux ABI for x86-64 (https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf) says that if the struct would be
// larger than two 8-bytes or would be a non-POD structure or union (POD term is from C++ standard, but it's
// elaborated in the document) or would contain unaligned fields, it would be passed in memory.
// Got the information from stackoverflow: https://stackoverflow.com/a/42413484

static const uint8_t MAX_ALPHA = 255;

static uint8_t blend_color_component(uint8_t foreground_component, uint8_t foreground_alpha, uint8_t background_component) {
	// Later in SSE we will be dividing by 256 here, as it's a bitwise shift which would be much faster.
	// Value shouldn't change much, it's almost the same.
	uint16_t fg = foreground_component;
	uint16_t bg = background_component;
	uint16_t alpha = foreground_alpha;
	return (uint8_t) ((fg * alpha + bg * (MAX_ALPHA - alpha)) / MAX_ALPHA);
}

static struct rgba blend_colors(struct rgba foreground, struct rgba background) {
	// Let b_r, b_g, b_b, b_a and f_r, f_g, f_b, f_a be channels of background and foreground.
	// Here will be my idea of what we do. It comes down to formulas from the internet:
	// (f_a / MAX_ALPHA) is part of light energy happened to be reflected by the foreground.
	//
	// We treat these colored surfaces as ideal non-consuming, they only reflect and refract.
	// 1 - (f_a / MAX_ALPHA) is part of light energy being refracted by foreground.
	//
	// We think background is fully opaque (fully not transparent), as otherwise there would be
	// another layer with black or white or etc. b_a == MAX_ALPHA, all light being reflected.
	// Part of energy being reflected is 1 - (f_a / MAX_ALPHA).
	//
	// We think that after reflection at background light rays coming from it through the
	// foreground aren't transformed in color. There might be reflection as well and refraction.
	// But we don't count it.
	// Then for the red channel the outcome is f_r * (f_a / MAX_ALPHA) + b_r * (1 - (f_a / MAX_ALPHA))
	// It's the same for every channel, so the formula is in a separate function.
	//
	// By blending our surface, we get fully opaque layer, which doesn't pass any light. So alpha
	// channel is MAX_ALPHA (alpha channel is level of opaqueness).
	struct rgba result = {
		blend_color_component(foreground.red, foreground.alpha, background.red),
		blend_color_component(foreground.green, foreground.alpha, background.green),
		blend_color_component(foreground.blue, foreground.alpha, background.blue),
		MAX_ALPHA
	};
	
	return result;
}

void blend_nosse(struct rgba * buffer, struct blend_pictures const * pictures) {
	// We have straight pixel colors (not premultiplied). If we would want to figure out how to do this, we would
	// consult physics to get physical model to emulate perception of our eyes. But it's already done, I suppose,
	// so just consult wikipedia.
	for (size_t row = 0; row < SCREEN_ROWS; ++row) {
		for (size_t col = 0; col < SCREEN_COLS; ++col) {
			size_t pos = row * SCREEN_COLS + col;

			buffer[pos] = blend_colors(pictures->foreground[pos], pictures->background[pos]);
		}
	}
}
