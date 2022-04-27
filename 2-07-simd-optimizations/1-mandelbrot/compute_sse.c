#include "compute.h"

#include <assert.h>
#include <emmintrin.h>
#include <limits.h>
#include <stdio.h>

void compute_sse(struct rgba * buffer, struct screen_state const * screen_state) {
	static const size_t SIMD_REGISTER_BIT_SIZE = 128;
	static const size_t NUM_PTS_IN_PACKED_REG = SIMD_REGISTER_BIT_SIZE / (CHAR_BIT * sizeof(float));
	assert(SCREEN_COLS % NUM_PTS_IN_PACKED_REG == 0);
	
	// Код дальше написан для упаковки 4-ех float-ов (32-ух битных вещественных), нет static_assert, приходится так.
	// Проверяется всё равно только в отладочной версии.
	assert(NUM_PTS_IN_PACKED_REG == 4);
	// Изменение x (возрастание) при движении по строке.
	// В таком порядке, поскольку в С и в отладчиках big-endian (а в числах для нас это привычно, всё понятно), а в
	// реальности платформа-то little-endian. Здесь указываем в стиле C.
	__m128 diff_x = _mm_set_ps(screen_state->scale * 3, screen_state->scale * 2, screen_state->scale * 1, 0);	
	
	__m128 max_allowed_rs_sq = _mm_set_ps1(MAX_ALLOWED_MODULO_SQ);

	for (size_t row = 0; row < SCREEN_ROWS; ++row) {		
		for (size_t col = 0; col < SCREEN_COLS; col += NUM_PTS_IN_PACKED_REG) {
			size_t pos = row * SCREEN_COLS + col;
			
			float pixel_x = screen_state->center.x + screen_state->scale * ((int) col - (int) SCREEN_COLS / 2);
			float pixel_y = screen_state->center.y + screen_state->scale * ((int) SCREEN_ROWS / 2 - (int) row);

			// Кординаты точек для следующих 4-ех в строке.
			__m128 starting_xs = _mm_add_ps(_mm_set_ps1(pixel_x), diff_x);			
			__m128 starting_ys = _mm_set_ps1(pixel_y);
			
			// Вычисляем элементы последовательности z_{n + 1} = z_n^2 + c, где c --- началаная
			// точка на комплексной плоскости, c = pixel_x + i * pixel_y.
			
			__m128 xs = starting_xs;
			__m128 ys = starting_ys;
			// Итерация, на которой ушла на бесконечность. Считаем, что она больше не вернётся в допустимый радиус.
			// Номер итерации нужен для вычисления цвета.
			size_t iter_left_surf[4] = {
				MAX_NUM_ITERATIONS, MAX_NUM_ITERATIONS,
				MAX_NUM_ITERATIONS, MAX_NUM_ITERATIONS
			};
			// Маска уже посчитанных позиций.
			unsigned int calculated_mask = 0;
			for (size_t iteration = 0; iteration < MAX_NUM_ITERATIONS; ++iteration) {
				__m128 xs_sq = _mm_mul_ps(xs, xs);
				__m128 ys_sq = _mm_mul_ps(ys, ys);
				__m128 xys = _mm_mul_ps(xs, ys);
				
				// (re + i * im) ^ 2 = re^2 + 2 * re * i * im - im^2
				// x = Re((re + i * im)^2) + starting_x
				// y = Im((re + i * im)^2) + starting_y
				xs = _mm_add_ps(_mm_sub_ps(xs_sq, ys_sq), starting_xs);
				ys = _mm_add_ps(_mm_add_ps(xys, xys), starting_ys);
				
				__m128 rs_sq = _mm_add_ps(_mm_mul_ps(xs, xs), _mm_mul_ps(ys, ys));
				
				// Либо 0, либо 0xffffffff в каждом из запакованных вещественных чисел
				// одинарной точности.
				__m128 cmp_results = _mm_cmpgt_ps(rs_sq, max_allowed_rs_sq);
				
				// Достать самые старшие биты каждого запакованного вещественного числа.
				unsigned int cmp_mask = (unsigned int) _mm_movemask_ps(cmp_results);

				// Если точка вышла за максимальный радиус, считаем, что она уже улетит на
				// бесконечность. И можем посчитать её цвет.
				
				// Вычисленные позиции, которые раньше не были вычислены.
				unsigned int newly_calculated_mask = cmp_mask & ~calculated_mask;
				if (newly_calculated_mask != 0) {
					for (size_t i = 0; i < NUM_PTS_IN_PACKED_REG; ++i) {
						if (((newly_calculated_mask >> i) & 1) == 1) {
							iter_left_surf[i] = iteration;
						}
					}
				}
				calculated_mask |= cmp_mask;
				
				// Если все вышли за пределы, можно больше не считать.
				// Иначе считать нужно.
				if (calculated_mask == (1 << NUM_PTS_IN_PACKED_REG) - 1) {
					break;
				}
			}
			
			for (size_t i = 0; i < NUM_PTS_IN_PACKED_REG; ++i) {
				size_t iteration = iter_left_surf[i];
				// printf("(%zu, %zu), iteration = %zu.\n", row, col + i, iteration);
				// Не улетела на бесконечность.
				// Где-то в окружности радиуса 1 с центром в 0 бегало.
				if (iteration == MAX_NUM_ITERATIONS) {
					struct rgba color = {0, 0, 0, 255};
					buffer[pos + i] = color;
				} else {
					struct rgba color = {iteration % 64, 64 + iteration % 64, 128 + iteration % 64, 255};
					buffer[pos + i] = color;
				}
			}
		}
	}
}