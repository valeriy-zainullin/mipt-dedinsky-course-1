#include "compute.h"

void compute_sse(struct rgba * buffer, struct screen_state const * screen_center) {
	(void) screen_center;
	for (size_t row = 0; row < SCREEN_ROWS; ++row) {
		for (size_t col = 0; col < SCREEN_COLS; ++col) {
			size_t pos = row * SCREEN_COLS + col;
			buffer[pos].red = (row % 3 == 0) * 255;
			buffer[pos].green = (row % 3 == 0 || row % 3 == 1) * 255;
			buffer[pos].blue = (row % 3 == 1) * 255;
			buffer[pos].alpha = 0;
		}
	}
}