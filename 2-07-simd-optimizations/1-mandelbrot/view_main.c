#include "builds.h"
#include "compute.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <windows.h>

#include <assert.h>
#include <stdbool.h>
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

void display_fps(SDL_Renderer* renderer, TTF_Font* fps_font, float prev_frame_time) {
	(void) renderer;
	(void) fps_font;
	
	// Immediate fps. Моментальный fps. Возможно, делать усреднение в будущем.
	float fps = 1.0f / prev_frame_time;

	char fps_buffer[127 + 1] = {0};
	snprintf(fps_buffer, sizeof(fps_buffer), "%.0f", fps); 
	
	static const SDL_Color fps_color = {255, 255, 0, 255};
	
	SDL_Surface* fps_surface = TTF_RenderText_Solid(fps_font, fps_buffer, fps_color);
	if (fps_surface == NULL) {
		// TODO: report an error.
		return;
	}

	// Surfaces are using RAM. And textures are using VRAM (video card RAM) rather than RAM.
	// Surfaces are used in software rendering. We are doing hardware rendering, that's why
	// we are making a texture. But of course, we were rendering the font with processor
	// and it means we needed to use surface (my thinking).
	// Source: https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	SDL_Texture* fps_texture = SDL_CreateTextureFromSurface(renderer, fps_surface);
	if (fps_texture == NULL) {
		// TODO: report an error.
		return;
	}
	
	int fps_box_width = 0;
	int fps_box_height = 0;
	
	if (TTF_SizeText(fps_font, fps_buffer, &fps_box_width, &fps_box_height) != 0) {
		// TODO: report an error.
		SDL_DestroyTexture(fps_texture);
		SDL_FreeSurface(fps_surface);
		return;
	}
	
	// Top left corner of the fps box.
	// Item's index in a zero-indexed array is number of items before it.
	// By substracting quantities we reserve required amount of pixels in
	// width and result is index.
	SDL_Rect fps_rect = {SCREEN_COLS - fps_box_width, 0, fps_box_width, fps_box_height};
	
	SDL_RenderCopy(renderer, fps_texture, NULL, &fps_rect);
	
	SDL_DestroyTexture(fps_texture);
	SDL_FreeSurface(fps_surface);
}

void prepare_comp_mode_texture(
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

	static const SDL_Color mode_color = {255, 255, 0, 255};
	
	SDL_Surface* mode_surface = TTF_RenderText_Solid(mode_font, mode_string, mode_color);
	if (mode_surface == NULL) {
		// TODO: report an error.
		return;
	}

	// Surfaces are using RAM. And textures are using VRAM (video card RAM) rather than RAM.
	// Surfaces are used in software rendering. We are doing hardware rendering, that's why
	// we are making a texture. But of course, we were rendering the font with processor
	// and it means we needed to use surface (my thinking).
	// Source: https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	*mode_texture = SDL_CreateTextureFromSurface(renderer, mode_surface);
	if (*mode_texture == NULL) {
		// TODO: report an error.
		return;
	}
	
	if (TTF_SizeText(mode_font, mode_string, mode_box_width, mode_box_height) != 0) {
		// TODO: report an error.
		SDL_DestroyTexture(*mode_texture);
		SDL_FreeSurface(mode_surface);
		return;
	}
	
	SDL_FreeSurface(mode_surface);
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
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 7;
	}
	
	SDL_Texture* comp_mode_texture = NULL;
	int mode_box_width = 0;
	int mode_box_height = 0;
	
	enum computation_mode computation_mode = COMPUTATION_MODE_PLAIN;
	prepare_comp_mode_texture(computation_mode, mode_font, renderer, &comp_mode_texture, &mode_box_width, &mode_box_height);
	
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
							prepare_comp_mode_texture(computation_mode, mode_font, renderer, &comp_mode_texture, &mode_box_width, &mode_box_height);

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
			// TODO: bad. More descriptive.
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
		display_fps(renderer, fps_font, prev_frame_time);
		display_comp_mode(renderer, comp_mode_texture, mode_box_width, mode_box_height);
		SDL_RenderPresent(renderer);

		uint64_t time_ended = SDL_GetPerformanceCounter();
		
		prev_frame_time = (time_ended - time_started) / ((float) SDL_GetPerformanceFrequency());
	}

	TTF_CloseFont(mode_font);
	TTF_CloseFont(fps_font);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}