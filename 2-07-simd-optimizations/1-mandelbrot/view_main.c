#include "compute.h"

#include <SDL2/SDL.h>

#include <windows.h>

#include <stdbool.h>
#include <stdlib.h>

// Workaround strange SDL2 behaviour when it redefines main function, but then with -lSDL2Main -lSDL2 it doesn't compile.
// Based on https://stackoverflow.com/a/29848468
#if defined(main)
#undef main
#endif

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при инициализации.", L"Ошибка SDL2", 0);
		return 1;
	}
	
	SDL_Window* window = SDL_CreateWindow("Множество Мандельброта", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_COLS, SCREEN_ROWS, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании окна.", L"Ошибка SDL2", 0);
		SDL_Quit();
		return 2;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "SDL_CreateRenderer failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании отрисовщика.", L"Ошибка SDL2", 0);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 3;
	}
	
	// Looks like SDL orders these fields big-endian, so for the structure those will be RGBA.
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_COLS, SCREEN_ROWS);
	if (texture == NULL) {
		fprintf(stderr, "Failed to create texture. SDL_CreateTexture failed: %s.\n", SDL_GetError());
		MessageBoxW(NULL, L"Ошибка при создании текстуры", L"Ошибка SDL2", 0);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 5;
	}
	
	int mode = 0;
	static const int NUM_MODES = 2;
	
	struct screen_state screen_state = INITIAL_SCREEN_STATE;
	

	// TODO: проверить с санитайзерами.
	bool exiting = false;
	while (!exiting) {
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
						
						case SDL_SCANCODE_LSHIFT:
						case SDL_SCANCODE_RSHIFT: {
							mode = (mode + 1) % NUM_MODES;
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
				
		int row_size = 0;                                              // in bytes
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
		}
		SDL_UnlockTexture(texture);
		
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
	
	SDL_DestroyRenderer(renderer);
	
	SDL_DestroyWindow(window);
	
	SDL_Quit();

	return 0;
}