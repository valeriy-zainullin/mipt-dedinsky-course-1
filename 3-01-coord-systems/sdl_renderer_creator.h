#pragma once

#include <SDL2/SDL.h>

#include <cstddef>
#include <stdint.h>

class SdlRendererOwner {
public:
	SdlRendererOwner(SDL_Window* window, int index, uint32_t flags)
	  : renderer_handle_(NULL) {
		assert(SDL_WasInit(SDL_INIT_VIDEO) != 0);
		
		renderer_handle_ = SDL_CreateRenderer(window, index, flags);

		if (renderer_handle_ == NULL) {
			fprintf(stderr, "SDL_CreateRenderer(window=%p,index=%d,flags=%" PRIu32 ") failed: %s.\n", window, index, flags, SDL_GetError());
		}
	}
	
	SDL_Renderer* get() {
		return renderer_handle_;
	}
	
	~SdlRendererOwner() {
		if (renderer_handle_ != NULL) {
			SDL_DestroyWindow(renderer_handle_);
			renderer_handle_ = NULL;
		}
	}
private:
	SDL_Renderer* renderer_handle_;
};
