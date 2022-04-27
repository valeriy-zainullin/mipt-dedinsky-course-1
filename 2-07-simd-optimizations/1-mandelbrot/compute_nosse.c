#include "compute.h"

#include <stdio.h>
#include <stdlib.h>

void compute_nosse(struct rgba * buffer, struct screen_state const * screen_state) {
	for (size_t row = 0; row < SCREEN_ROWS; ++row) {
		for (size_t col = 0; col < SCREEN_COLS; ++col) {
			size_t pos = row * SCREEN_COLS + col;

			float pixel_x = screen_state->center.x + screen_state->scale * ((int) col - (int) SCREEN_COLS / 2);
			float pixel_y = screen_state->center.y + screen_state->scale * ((int) SCREEN_ROWS / 2 - (int) row);
			
			// Вычисляем элементы последовательности z_{n + 1} = z_n^2 + c, где c --- началаная
			// точка на комплексной плоскости, c = pixel_x + i * pixel_y.

			float starting_x = pixel_x;
			float starting_y = pixel_y;
			float x = starting_x;
			float y = starting_y;
			size_t iteration = 0;
			for (; iteration < MAX_NUM_ITERATIONS; ++iteration) {
				float x_sq = x * x;
				float xy = x * y;
				float y_sq = y * y;
				
				// (re + i * im) ^ 2 = re^2 + 2 * re * i * im - im^2
				// x = Re((re + i * im)^2) + starting_x
				// y = Im((re + i * im)^2) + starting_y
				x = x_sq - y_sq + starting_x;
				y = 2 * xy + starting_y;

				float r_sq = x * x + y * y;
				
				// Если точка вышла за максимальный радиус, считаем, что она уже улетит на
				// бесконечность. И можем посчитать её цвет.
				// TODO: объяснить, почему здесь +-EPS одинаково неправильно, почему это
				// не делается.
				if (r_sq > MAX_ALLOWED_MODULO_SQ) {
					break;
				}
			}
			
			// Не улетела на бесконечность.
			// Где-то в окружности радиуса 1 с центром в 0 бегало.
			if (iteration == MAX_NUM_ITERATIONS) {
				buffer[pos].red = 0;
				buffer[pos].green = 0;
				buffer[pos].blue = 0;
				buffer[pos].alpha = 255;
			} else {
				buffer[pos].red = iteration % 64;
				buffer[pos].green = 64 + iteration % 64;
				buffer[pos].blue = 128 + iteration % 64;
				buffer[pos].alpha = 255;
			}
		}
	}
}