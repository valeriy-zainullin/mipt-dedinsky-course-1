#pragma once

#include <SDL2/SDL.h>

#include <stdint.h>

class SdlInitializer {
public:
	SdlInitializer(uint32_t flags)
	  : sdl_init_result(0) {
		assert(SDL_WasInit(SDL_INIT_EVERYTHING) == 0);
		sdl_init_result = SDL_Init(flags);
		
		if (!initialized()) {
			fprintf(stderr, "SDL_Init(flags=0x%x) failed: %s.\n", flags, SDL_GetError());
		}
	}
	
	bool initialized() {
		return sdl_init_result == 0;
	}
			
	~SdlInitializer() {
		if (sdl_init_result == 0) {
			SDL_Quit();
		}
	}
private:
	int sdl_init_result;
};
