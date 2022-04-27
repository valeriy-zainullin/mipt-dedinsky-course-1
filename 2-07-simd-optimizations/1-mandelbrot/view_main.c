#include "compute.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <windows.h>

#include <stdbool.h>
#include <stdlib.h>

// Workaround strange SDL2 behaviour when it redefines main function, but then with -lSDL2Main -lSDL2 it doesn't compile.
// Based on https://stackoverflow.com/a/29848468
#if defined(main)
#undef main
#endif

void display_fps(SDL_Renderer* renderer, TTF_Font* fps_font, float prev_frame_time) {
	(void) renderer;
	(void) fps_font;
	
	// Immediate fps. Моментальный fps. Возможно, делать усреднение в будущем.
	float fps = 1.0f / prev_frame_time;

	char fps_buffer[127 + 1] = {0};
	snprintf(fps_buffer, sizeof(fps_buffer), "%.0f", fps); 
	
	fprintf(stderr, "%s\n", fps_buffer);
	fflush(stderr);
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
	
	int mode = 0;
	static const int NUM_MODES = 3;
	
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
							mode = (mode + 1) % NUM_MODES;
							
							if (mode == 1 && !compute_check_sse_supported()) {
								MessageBoxW(NULL, L"Процессор не поддерживает набор инструкций SSE. Режим пропущен.", L"Режим недоступен", MB_ICONINFORMATION);
								mode = (mode + 1) % NUM_MODES;
							}
							
							if (mode == 2 && !compute_check_avx_supported()) {
								MessageBoxW(NULL, L"Процессор не поддерживает набор инструкций AVX. Режим пропущен.", L"Режим недоступен", MB_ICONINFORMATION);
								mode = (mode + 1) % NUM_MODES;
							}

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
		switch (mode) {
			// TODO: bad. More descriptive.
			case 0: {
				compute_nosse(pixels, &screen_state);
				break;
			}

			case 1: {
				compute_sse(pixels, &screen_state);
				break;
			}

			case 2: {
				compute_avx(pixels, &screen_state);
				break;
			}
		}
		SDL_UnlockTexture(texture);
		
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		display_fps(renderer, fps_font, prev_frame_time);
		SDL_RenderPresent(renderer);

		uint64_t time_ended = SDL_GetPerformanceCounter();
		
		prev_frame_time = (time_ended - time_started) / ((float) SDL_GetPerformanceFrequency());
	}
	
	TTF_CloseFont(fps_font);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}