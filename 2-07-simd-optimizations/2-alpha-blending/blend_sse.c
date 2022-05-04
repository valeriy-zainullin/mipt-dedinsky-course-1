#include "blend.h"

#include "builds.h"

#include <assert.h>
#include <cpuid.h>
#include <emmintrin.h>
#include <limits.h>
#include <smmintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <tmmintrin.h>

static const uint8_t MAX_ALPHA = 255;

// Источники:
//   https://stackoverflow.com/questions/14266772/how-do-i-call-cpuid-in-linux
//   https://github.com/gcc-mirror/gcc/blob/master/gcc/config/i386/cpuid.h
//   https://en.wikipedia.org/wiki/CPUID
bool blend_check_sse_impl_supported() {
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
	static const size_t SIMD_REGISTER_BIT_SIZE = 128;

	// Мы здесь умножаем на 2, потому что придётся расширять 8-ми битные числа
	// (в этом случае, палитра может расшириться) до 16-ти битных
	static const size_t NUM_PTS_IN_PACKED_REG = SIMD_REGISTER_BIT_SIZE / (CHAR_BIT * sizeof(struct rgba) * 2);
	assert(SCREEN_COLS % NUM_PTS_IN_PACKED_REG == 0);
	
	// Код дальше написан для упаковки 2-ух struct rgba (32-ух битных структур цвета пиксела), нет static_assert,
	// приходится так.
	// Проверяется всё равно только в отладочной версии.
	assert(NUM_PTS_IN_PACKED_REG == 2);
	
	static const uint8_t ZERO_BITS = 0x80;
	//  15 14 13 12   11 10  9  8    7  6  5  4    3  2  1  0
	// [-- a1 -- b0 | -- g1 -- r1 | -- a0 -- b0 | -- g0 -- r0]
	//      |___________________        |___________________
	//      |     \      \      \       |     \      \      \.
	// [-- a1 -- a1 | -- a1 -- a1 | -- a0 -- a0 | -- a0 -- a0]
	__m128i extract_alpha16_mask = _mm_set_epi8(
		ZERO_BITS, 14, ZERO_BITS, 14,
		ZERO_BITS, 14, ZERO_BITS, 14,
		ZERO_BITS,  6, ZERO_BITS,  6,
		ZERO_BITS,  6, ZERO_BITS,  6
	);
	
	// We will need to divide by 256 (almost the same as dividing by 255).
	// Do so with a mask.
	__m128i div_pi16_by_256_mask = _mm_set_epi8(
		ZERO_BITS, 15, ZERO_BITS, 13, ZERO_BITS, 11, ZERO_BITS, 9,
		ZERO_BITS,  7, ZERO_BITS,  5, ZERO_BITS,  3, ZERO_BITS, 1
	);
	
	// _mm_cvtepi can only promote to larger types (in bits).
	// We will need to convert pi16 back to pu8. Do that
	// with a mask.
	__m128i cast_pi16_to_pu8 = _mm_set_epi8(
		ZERO_BITS, ZERO_BITS, ZERO_BITS, ZERO_BITS,
		ZERO_BITS, ZERO_BITS, ZERO_BITS, ZERO_BITS,
		14,12,10,8, 6,4,2,0
	);
	
	__m128i m128i_epi16_255s = _mm_set_epi16(255,255,255,255, 255,255,255,255);

	// Composition of permutations will be both operations combined.
	// Apply both of them at the same time. It will reduce run time.
	__m128i div_256_cast_to_pu8 = _mm_shuffle_epi8(div_pi16_by_256_mask, cast_pi16_to_pu8);
	
	for (size_t row = 0; row < SCREEN_ROWS; ++row) {
		for (size_t col = 0; col < SCREEN_COLS; col += NUM_PTS_IN_PACKED_REG) {
			size_t pos = row * SCREEN_COLS + col;
			
			// Формат комментариев, в которых написано содержимое SSE-регистров, был взят у
			// Ильи Рудольфовича Дединского, нашего преподавателя. Он скидывал нам решения
			// этих задач, только в них нам самим нужно было найти SSE интринсики SSE по
			// тому, что они делают. Части названий функции отсутствовали, вместо них
			// вставлялось слово "meow" :) И нужно было найти их названия, как раз были
			// такие комментарии. Найти, например, можно с помощью intel intrinsics guide
			// (оригинал сейчас недоступен в России; зеркало на https://www.laruence.com/sse/,
			// им я и пользовался).
			// 
			
			//--------------------------------------------------------------------------------
			//           15  14  13  12    11  10   9   8     7   6   5   4     3   2   1   0
			// front = [--- --- --- --- | --- --- --- --- | a1f b1f g1f r1f | a0f b0f g0f r0f]
			// back  = [--- --- --- --- | --- --- --- --- | a1b b1b g1b r1b | a0b b0b g0b r0b]
			//--------------------------------------------------------------------------------
			__m128i front = _mm_set_epi64x(0, *(__int64*) &pictures->foreground[pos]);
			__m128i back  = _mm_set_epi64x(0, *(__int64*) &pictures->background[pos]);
			
			// uint16_t fg_16 = foreground_component;
			// uint16_t bg_16 = background_component;
			// uint16_t alpha_16 = foreground_alpha;
			// Расширяем 8-ми битные беззнаковые до 16-ти битных, чтобы в дальнейшем при
			// операциях результаты помещались. Делаем всё как в no-sse версии, только
			// для нескольких пикселей сразу.
			// Нам нужно вычислить по формуле, которая представлена в no-sse версии:
			// (uint8_t) ((fg_16 * alpha_16 + bg_16 * (MAX_ALPHA - alpha_16)) / MAX_ALPHA)
			// Для этого нужно расширить значения до 16-ти бит, затем дальше проводить операции.
			// Теперь каждое значение будет занимать 2 байта.
			// _mm_cvtepu8_epi16 расширает младшие 8 байт с 8-ми байтных беззнаковых до 16-ти
			// байтовых знаковых (по факту заполнение нулями получается).
			//--------------------------------------------------------------------------------
			//           15  14  13  12    11  10   9   8     7   6   5   4     3   2   1   0
			// front = [--- --- --- --- | --- --- --- --- | a1f b1f g1f r1f | a0f b0f g0f r0f]
			//                                                         ________/   /   /   |
			//                ...     ...       ...     ...       ____/   ________/ __/    |
			//               /       /         /       /         /       /         /       |
			// fg_16 = [--- a1f --- b1f | --- g1f --- r1f | --- a0f --- b0f | --- g0f --- r0f]
			//--------------------------------------------------------------------------------
			//           15  14  13  12    11  10   9   8     7   6   5   4     3   2   1   0
			// back  = [--- --- --- --- | --- --- --- --- | a1b b1b g1b r1b | a0b b0b g0b r0b]
			//                                                         ________/   /   /   |
			//                ...     ...       ...     ...       ____/   ________/ __/    |
			//               /       /         /       /         /       /         /       |
			// bg_16 = [--- a1b --- b1b | --- g1b --- r1b | --- a0b --- b0b | --- g0b --- r0b]
			//--------------------------------------------------------------------------------
			__m128i fg_16 = _mm_cvtepu8_epi16(front);
			__m128i bg_16 = _mm_cvtepu8_epi16(back);
			
			// Достаём альфа-каналы, не убираем из front_16 и back_16, потому что это ничего
			// не даст, только потратим время на удаление, проще их оставить, мы их
			// забесплатно перемножаем.
			// uint16_t alpha_16 = foreground_alpha;
			//--------------------------------------------------------------------------------
			//              15  14  13  12    11  10   9   8     7   6   5   4     3   2   1   0
			// fg_16    = [--- a1f --- b1f | --- g1f --- r1f | --- a0f --- b0f | --- g0f --- r0f]
			//                  |                                   |
			//                  |_________________________          |_________________________
			//                  |       \         \       \         |       \         \       \.
			// alpha_16 = [--- a1f --- a1f | --- a1f --- a1f | --- a0f --- a0f | --- a0f --- a0f]
			//--------------------------------------------------------------------------------
			// "." добавляется в конце за тем, чтобы не получать -Wcomment в следствие того, что
			// обратным слэшом экранируем перевод строки, одним однострочным комментарием
			// комментируем две строки. Пробел не помогает.
			__m128i alpha_16 = _mm_shuffle_epi8(fg_16, extract_alpha16_mask);
			
			// (uint8_t) ((fg_16 * alpha_16 + bg_16 * (MAX_ALPHA - alpha_16)) / MAX_ALPHA)

			// Judging by _mm_mullo_epi16 docs (generalizing), _mm_mullo multiplies, producing
			// value with 2 times more bits of the same signedness, and returns lower bits.
			// Judging by docs, mullo only exists for signed types. mulhi for unsigned.
			// It's possible, these operation exist only for 16-bit and 8-bit types.
			// _mm_mullo_epi16...
			// TODO: explain _mm_mullo_epi16.

			// fg_16 * alpha_16
			__m128i fg_16_multiplied = _mm_mullo_epi16(fg_16, alpha_16);
			
			// bg_16 * (MAX_ALPHA - alpha_16)
			__m128i bg_16_multiplied = _mm_mullo_epi16(bg_16, _mm_sub_epi16(m128i_epi16_255s, alpha_16));
			
			// (fg_16 * alpha_16 + bg_16 * (MAX_ALPHA - alpha_16)
			__m128i sum = _mm_add_epi16(fg_16_multiplied, bg_16_multiplied);

			// result =
			//   ((fg_16 * alpha_16 + bg_16 * (MAX_ALPHA - alpha_16)) / 256       ~=
			//   ((fg_16 * alpha_16 + bg_16 * (MAX_ALPHA - alpha_16)) / MAX_ALPHA
			// result = (uint8_t) result
			// Композиция перестановок.
			__m128i result = _mm_shuffle_epi8(sum, div_256_cast_to_pu8);

			// Записать младшие 64 бита --- 8 байт --- 2 struct rgba в буфер.
			_mm_storeu_si64(&buffer[pos], result);
			
			// In _mm_movelh and _mm_movehl always upper bits of result are from a,
			// lower bits are from b.
			// In order of arguments.
			// l h means move lowers (discard h :))
			// h l means move highers (discard l :))
			// I think the reason is that in case one would want to express lh with hl, for example,
			// one would just exchange halves of numbers. lh moves lowers and hl moves uppers just
			// because it's those numbers with halves exchanged. One could always think it's move of
			// lowers, just hl exchanges halves, it results in moving highers :) I will remember
			// this approach, but think of it in previous way.

			// Fixing alpha channels.
			for (size_t i = 0; i < NUM_PTS_IN_PACKED_REG; ++i) {
				buffer[pos + i].alpha = MAX_ALPHA;
			}
		}
	}
}
