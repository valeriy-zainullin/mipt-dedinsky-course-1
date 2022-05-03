#include "builds.h"
#include "compute.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Workaround strange SDL2 behaviour when it redefines main function, but then with -lSDL2Main -lSDL2 it doesn't compile.
// Based on https://stackoverflow.com/a/29848468
#if defined(main)
#undef main
#endif

enum computation_mode {
	COMPUTATION_MODE_PLAIN = 0,
	COMPUTATION_MODE_SSE   = 1,
	COMPUTATION_MODE_AVX   = 2
};
static const int NUM_COMPUTATION_MODES = 3;

bool display_fps(SDL_Renderer* renderer, TTF_Font* fps_font, float prev_frame_time) {
	(void) renderer;
	(void) fps_font;
	
	static const float EPS = 1e-3;  // Values below this are treated as zero.
	static const size_t MAX_FPS = 512;
	static char const * const MORE_THAN_MAX_FPS = ">512";
	
	char fps_buffer[127 + 1] = {0};

	if (prev_frame_time < EPS) {
		// Слишком маленькое значение. Считаем, что кадров больше, чем максимально отображаемо.
		strncpy(fps_buffer, MORE_THAN_MAX_FPS, sizeof(fps_buffer));
	} else {
		// Immediate fps. Моментальный fps. Возможно, делать усреднение в будущем.
		float fps = 1.0f / prev_frame_time;
		
		if (fps >= MAX_FPS - EPS) {
			strncpy(fps_buffer, MORE_THAN_MAX_FPS, sizeof(fps_buffer));
		} else {
			snprintf(fps_buffer, sizeof(fps_buffer), "%.0f", fps);
		}
	}
	
	static const SDL_Color FPS_COLOR = {255, 255, 0, 255};
	
	SDL_Surface* fps_surface = TTF_RenderText_Solid(fps_font, fps_buffer, FPS_COLOR);
	if (fps_surface == NULL) {
		return false;
	}

	// Surfaces are using RAM. And textures are using VRAM (video card RAM) rather than RAM.
	// Surfaces are used in software rendering. We are doing hardware rendering, that's why
	// we are making a texture. But of course, we were rendering the font with processor
	// and it means we needed to use surface (my thinking).
	// Source: https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	SDL_Texture* fps_texture = SDL_CreateTextureFromSurface(renderer, fps_surface);
	if (fps_texture == NULL) {
		SDL_FreeSurface(fps_surface);
		return false;
	}
	
	int fps_box_width = 0;
	int fps_box_height = 0;
	
	if (TTF_SizeText(fps_font, fps_buffer, &fps_box_width, &fps_box_height) != 0) {
		SDL_DestroyTexture(fps_texture);
		SDL_FreeSurface(fps_surface);
		return false;
	}
	
	// Top left corner of the fps box.
	// Item's index in a zero-indexed array is number of items before it.
	// By substracting quantities we reserve required amount of pixels in
	// width and result is index.
	SDL_Rect fps_rect = {SCREEN_COLS - fps_box_width, 0, fps_box_width, fps_box_height};
	
	// Could've checked more for errors here, but the only thing to do is to log them.
	// It's a good idea though. Because fps is not a critical structure.
	SDL_RenderCopy(renderer, fps_texture, NULL, &fps_rect);
	
	SDL_DestroyTexture(fps_texture);
	SDL_FreeSurface(fps_surface);
	
	return true;
}

bool prepare_comp_mode_texture(
	enum computation_mode computation_mode,
	TTF_Font* mode_font,
	SDL_Renderer* renderer,
	SDL_Texture** mode_texture,
	int* mode_box_width,
	int* mode_box_height
) {
	char* mode_string = NULL;
	
	switch (computation_mode) {
		case COMPUTATION_MODE_PLAIN: {
			#if BUILD == BUILD_RELEASE
				mode_string = "Plain (-O2)";
			#elif BUILD == BUILD_DEBUG
				mode_string = "Plain (-Og)";
			#else
				mode_string = "Plain (-O?)";
			#endif
			break;
		}
		
		case COMPUTATION_MODE_SSE: {
			mode_string = "SSE";
			break;
		}
		
		case COMPUTATION_MODE_AVX: {
			mode_string = "AVX";
			break;
		}
		
		default: assert(false); __builtin_unreachable();
	}

	static const SDL_Color MODE_COLOR = {255, 255, 0, 255};
	
	// Text rendering (not texture rendering) happens quite rarely (when a key pressed),
	// so blended rendering could be done. It does antialiasing and it's slow, but it's
	// renders are of high quality.
	// Source of info about blended functions difference:
	// https://www.freepascal-meets-sdl.net/chapter-7-texts-fonts-surface-conversion/
	SDL_Surface* mode_surface = TTF_RenderText_Blended(mode_font, mode_string, MODE_COLOR);
	if (mode_surface == NULL) {
		return false;
	}

	// Surfaces are using RAM. And textures are using VRAM (video card RAM) rather than RAM.
	// Surfaces are used in software rendering. We are doing hardware rendering, that's why
	// we are making a texture. But of course, we were rendering the font with processor
	// and it means we needed to use surface (my thinking).
	// Source: https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	*mode_texture = SDL_CreateTextureFromSurface(renderer, mode_surface);
	if (*mode_texture == NULL) {
		SDL_FreeSurface(mode_surface);
		return false;
	}
	
	if (TTF_SizeText(mode_font, mode_string, mode_box_width, mode_box_height) != 0) {
		SDL_DestroyTexture(*mode_texture);
		SDL_FreeSurface(mode_surface);
		return false;
	}
	
	SDL_FreeSurface(mode_surface);
	
	return true;
}

void free_comp_mode_texture(SDL_Texture** mode_texture) {
	SDL_DestroyTexture(*mode_texture);
	
	*mode_texture = NULL;
}

void display_comp_mode(SDL_Renderer* renderer, SDL_Texture* mode_texture, int mode_box_width, int mode_box_height) {
	assert(mode_texture != NULL);
	
	// If even number of pixels, put in the right (of a greater index in an array of pixels) median the right
	// median pixel of box. Allocate smaller part from pixels before SCREEN_COLS / 2.
	// If odd number of pixels, median of the box is at the right median of the row of the screen.
	// Pixel perfect :)
	// One pixel +- doesn't matter, could have just wrote what I wrote.
	SDL_Rect dst_rect = {SCREEN_COLS / 2 - mode_box_width / 2, 0, mode_box_width, mode_box_height};
	
	SDL_RenderCopy(renderer, mode_texture, NULL, &dst_rect);
}

bool prepare_notes_texture(
	TTF_Font* notes_font,
	SDL_Renderer* renderer,
	SDL_Texture** notes_texture,
	int* notes_box_width,
	int* notes_box_height
) {
	static char const * const NOTES[] = {
		"Есть три режима: обычный, SSE и AVX. Если",
		"режим не поддерживается, при его",
		"активации вы получите сообщение.",
		"Enter    -- приблизить.",
		"Space    -- отдалить.",
		"Стрелки  -- навигация.",
		"Tab      -- переключить режим.",
		"Alt      -- вкл/выкл отображение текстов."
	};

	static const SDL_Color NOTES_COLOR = {255, 255, 255, 255};
	
	// Source: https://wiki.libsdl.org/SDL_CreateRGBSurface
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		static const uint32_t RED_MASK   = 0xff000000;
		static const uint32_t GREEN_MASK = 0x00ff0000;
		static const uint32_t BLUE_MASK  = 0x0000ff00;
		static const uint32_t ALPHA_MASK = 0x000000ff;
	#else
		static const uint32_t RED_MASK   = 0x000000ff;
		static const uint32_t GREEN_MASK = 0x0000ff00;
		static const uint32_t BLUE_MASK  = 0x00ff0000;
		static const uint32_t ALPHA_MASK = 0xff000000;
	#endif
	SDL_Surface* notes_surface = SDL_CreateRGBSurface(0, SCREEN_COLS, SCREEN_ROWS, 32, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
	if (notes_surface == NULL) {
		return false;
	}

	// Text rendering (not texture rendering) happens quite rarely (only once for notes),
	// so blended rendering could be done. It does antialiasing and it's slow, but it's
	// renders are of high quality.
	// Source of info about blended functions difference:
	// https://www.freepascal-meets-sdl.net/chapter-7-texts-fonts-surface-conversion/
	*notes_box_width = 0;
	*notes_box_height = 0;
	for (size_t i = 0; i < sizeof(NOTES) / sizeof(*NOTES); ++i) {
		SDL_Surface* line_surface = TTF_RenderUTF8_Blended(notes_font, NOTES[i], NOTES_COLOR);
		if (line_surface == NULL) {
			SDL_FreeSurface(notes_surface);
			return false;
		}
		
		int line_box_width = 0;
		int line_box_height = 0;
		if (TTF_SizeUTF8(notes_font, NOTES[i], &line_box_width, &line_box_height) != 0) {
			SDL_FreeSurface(line_surface);
			SDL_FreeSurface(notes_surface);
			return false;
		}
		
		SDL_Rect dst_rect = {0, *notes_box_height, line_box_width, line_box_height};
		if (SDL_BlitSurface(line_surface, NULL, notes_surface, &dst_rect) != 0) {
			SDL_FreeSurface(line_surface);
			SDL_FreeSurface(notes_surface);
			return false;
		}
		
		*notes_box_height += line_box_height;
		if (*notes_box_width < line_box_width) {
			*notes_box_width = line_box_width;
		}
		
		SDL_FreeSurface(line_surface);
	}
	
	// Surfaces are using RAM. And textures are using VRAM (video card RAM) rather than RAM.
	// Surfaces are used in software rendering. We are doing hardware rendering, that's why
	// we are making a texture. But of course, we were rendering the font with processor
	// and it means we needed to use surface (my thinking).
	// Source: https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	*notes_texture = SDL_CreateTextureFromSurface(renderer, notes_surface);
	if (*notes_texture == NULL) {
		SDL_FreeSurface(notes_surface);
		return false;
	}
	
	SDL_FreeSurface(notes_surface);
	return true;
}

void free_notes_texture(SDL_Texture** notes_texture) {
	SDL_DestroyTexture(*notes_texture);
	
	*notes_texture = NULL;
}

void display_notes(SDL_Renderer* renderer, SDL_Texture* notes_texture, int notes_box_width, int notes_box_height) {
	assert(notes_texture != NULL);
	
	(void) notes_box_width;
	(void) notes_box_height;
	
	SDL_RenderCopy(renderer, notes_texture, NULL, NULL);
}


int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при инициализации. Подробнее в логе (stderr).", L"Ошибка SDL2", 0);
		return 1;
	}
	
	if (TTF_Init() != 0) {
		fprintf(stderr, "TTF_Init failed: %s.\n", TTF_GetError());
		MessageBoxW(NULL, L"Ошибка при инициализации библиотеки для работы со шрифтами. Подробнее в логе (stderr).", L"Ошибка SDL2_TTF", 0);
		SDL_Quit();
		return 2;
	}
	
	SDL_Window* window = SDL_CreateWindow("Множество Мандельброта", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_COLS, SCREEN_ROWS, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании окна. Подробнее в логе (stderr).", L"Ошибка SDL2", 0);
		TTF_Quit();
		SDL_Quit();
		return 2;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "SDL_CreateRenderer failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании отрисовщика. Подробнее в логе (stderr).", L"Ошибка SDL2", 0);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 3;
	}
	
	// Looks like SDL orders these fields big-endian, so for the structure those will be RGBA.
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_COLS, SCREEN_ROWS);
	if (texture == NULL) {
		fprintf(stderr, "Failed to create texture. SDL_CreateTexture failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании текстуры.  Подробнее в логе (stderr).", L"Ошибка SDL2", 0);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 5;
	}
	
	static const char FPS_FONT_FILE[] = "ubuntu-mono/UbuntuMono-Regular.ttf";
	static const int FPS_FONT_SIZE = 32; // In points.
	TTF_Font* fps_font = TTF_OpenFont(FPS_FONT_FILE, FPS_FONT_SIZE);
	if (fps_font == NULL) {
		fprintf(stderr, "Failed to open fps font. TTF_OpenFont failed: %s.\n", TTF_GetError());
		MessageBoxW(NULL, L"Ошибка при загрузке шрифта для счётчика FPS.  Подробнее в логе (stderr).", L"Ошибка SDL2_TTF", 0);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 6;
	}

	static const char MODE_FONT_FILE[] = "ubuntu-mono/UbuntuMono-Regular.ttf";
	static const int MODE_FONT_SIZE = 32; // In points.
	TTF_Font* mode_font = TTF_OpenFont(MODE_FONT_FILE, MODE_FONT_SIZE);
	if (mode_font == NULL) {
		fprintf(stderr, "Failed to open mode font. TTF_OpenFont failed: %s.\n", TTF_GetError());
		MessageBoxW(NULL, L"Ошибка при загрузке шрифта для отображения режима.  Подробнее в логе (stderr).", L"Ошибка SDL2_TTF", 0);
		TTF_CloseFont(mode_font);
		TTF_CloseFont(fps_font);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 7;
	}

	static const char NOTES_FONT_FILE[] = "ubuntu-mono/UbuntuMono-Regular.ttf";
	static const int NOTES_FONT_SIZE = 14; // In points.
	TTF_Font* notes_font = TTF_OpenFont(NOTES_FONT_FILE, NOTES_FONT_SIZE);
	if (notes_font == NULL) {
		fprintf(stderr, "Failed to open mode font. TTF_OpenFont failed: %s.\n", TTF_GetError());
		MessageBoxW(NULL, L"Ошибка при загрузке шрифта для отображения дополнительной информации.  Подробнее в логе (stderr).", L"Ошибка SDL2_TTF", 0);
		TTF_CloseFont(notes_font);
		TTF_CloseFont(mode_font);
		TTF_CloseFont(fps_font);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 8;
	}

	
	SDL_Texture* comp_mode_texture = NULL;
	int mode_box_width = 0;
	int mode_box_height = 0;
	
	SDL_Texture* notes_texture = NULL;
	int notes_box_width = 0;
	int notes_box_height = 0;
	if (!prepare_notes_texture(notes_font, renderer, &notes_texture, &notes_box_width, &notes_box_height)) {
		#if BUILD == BUILD_DEBUG
			fprintf(stderr, "Failed to prepare notes texture.\n");
		#endif
	}
	
	bool display_texts = true;
	
	enum computation_mode computation_mode = COMPUTATION_MODE_PLAIN;
	if (!prepare_comp_mode_texture(computation_mode, mode_font, renderer, &comp_mode_texture, &mode_box_width, &mode_box_height)) {
		#if BUILD == BUILD_DEBUG
			fprintf(stderr, "Failed to prepare computation mode texture.\n");
		#endif
	}
	
	// Для самого первого кадра 1, чтобы FPS был равен 1.
	// Точное значение fps для первого кадра не известно (я отображаю моментальный fps
	// предыдущего кадра в текущем, иначе, если брать время этого кадра при
	// вычислении fps, то его нужно рисовать после RenderPresent, а это уже другой
	// кадр, нужно заново рисовать... Т.е. RenderPresent тогда во временной промежуток
	// подготовки кадра не попадает.
	float prev_frame_time = 1.0f;
	
	struct screen_state screen_state = INITIAL_SCREEN_STATE;	

	// TODO: проверить с санитайзерами.
	bool exiting = false;
	while (!exiting) {
		uint64_t time_started = SDL_GetPerformanceCounter();
	
		SDL_Event event = {0};
		
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					switch (event.key.keysym.scancode) {
						// TODO: consider precision losses here by limiting scaling and moving.
						case SDL_SCANCODE_RETURN: {
							screen_state.scale /= SCALE_TIMES_STEP;
							break;
						}
						
						case SDL_SCANCODE_SPACE: {
							screen_state.scale *= SCALE_TIMES_STEP;
							break;
						}
						
						case SDL_SCANCODE_LEFT: {
							screen_state.center.x -= PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_RIGHT: {
							screen_state.center.x += PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_UP: {
							screen_state.center.y += PIXEL_STEP * screen_state.scale;
							break;
						}

						case SDL_SCANCODE_DOWN: {
							screen_state.center.y -= PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_TAB: {
							int mode = (int) computation_mode;
							mode = (mode + 1) % NUM_COMPUTATION_MODES;
							
							if (mode == (int) COMPUTATION_MODE_SSE && !compute_check_sse_supported()) {
								MessageBoxW(NULL, L"Процессор не поддерживает набор инструкций SSE. Режим пропущен.", L"Режим недоступен", MB_ICONINFORMATION);
								mode = (mode + 1) % NUM_COMPUTATION_MODES;
							}
							
							if (mode == (int) COMPUTATION_MODE_AVX && !compute_check_avx_supported()) {
								MessageBoxW(NULL, L"Процессор не поддерживает набор инструкций AVX. Режим пропущен.", L"Режим недоступен", MB_ICONINFORMATION);
								mode = (mode + 1) % NUM_COMPUTATION_MODES;
							}
							
							computation_mode = (enum computation_mode) mode;
							
							free_comp_mode_texture(&comp_mode_texture);
							if (!prepare_comp_mode_texture(computation_mode, mode_font, renderer, &comp_mode_texture, &mode_box_width, &mode_box_height)) {
								#if BUILD == BUILD_DEBUG
									fprintf(stderr, "Failed to prepare computation mode texture.\n");
								#endif
							}

							break;
						}
						
						case SDL_SCANCODE_LALT:
						case SDL_SCANCODE_RALT: {
							display_texts = !display_texts;
						
							break;
						}
						
						default: break;
					}
					break;
				}
				
				case SDL_QUIT: {
					exiting = true;
					break;
				}
			}
		}
				
		int row_size = 0;                                              // In bytes
		struct rgba* pixels = NULL;
		SDL_LockTexture(texture, NULL, (void**) &pixels, &row_size);
		switch (computation_mode) {
			case COMPUTATION_MODE_PLAIN: {
				compute_nosse(pixels, &screen_state);
				break;
			}

			case COMPUTATION_MODE_SSE: {
				compute_sse(pixels, &screen_state);
				break;
			}

			case COMPUTATION_MODE_AVX: {
				compute_avx(pixels, &screen_state);
				break;
			}
		}
		SDL_UnlockTexture(texture);
		
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		if (display_texts) {
			display_fps(renderer, fps_font, prev_frame_time);
			display_comp_mode(renderer, comp_mode_texture, mode_box_width, mode_box_height);
			display_notes(renderer, notes_texture, notes_box_width, notes_box_height);
		}
		SDL_RenderPresent(renderer);

		uint64_t time_ended = SDL_GetPerformanceCounter();
		
		prev_frame_time = (time_ended - time_started) / ((float) SDL_GetPerformanceFrequency());
	}
	
	free_notes_texture(&notes_texture);
	free_comp_mode_texture(&comp_mode_texture);

	TTF_CloseFont(notes_font);
	TTF_CloseFont(mode_font);
	TTF_CloseFont(fps_font);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}