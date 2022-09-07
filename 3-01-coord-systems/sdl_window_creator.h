#pragma once

#include <SDL2/SDL.h>

#include <cstddef>
#include <stdint.h>

class SdlWindowOwner {
public:
	SdlWindowOwner(const char* title, int x_pos, int y_pos, int width, int height, uint32_t flags)
	  : window_handle_(NULL) {
		assert(SDL_WasInit(SDL_INIT_VIDEO) != 0);
		
		window_handle_ = SDL_CreateWindow(title, x_pos, y_pos, width, height, flags);

		if (window_handle_ == NULL) {
			fprintf(stderr, "SDL_CreateWindow(title=\"%s\",x_pos=%d,y_pos=%d,width=%d,height=%d,flags=%" PRIu32 ") failed: %s.\n", title, x_pos, y_pos, width, height, flags, SDL_GetError());
		}
	}
	
	SDL_Window* get() {
		return window_handle_;
	}
	
	~SdlWindowOwner() {
		if (window_handle_ != NULL) {
			SDL_DestroyWindow(window_handle_);
			window_handle_ = NULL;
		}
	}
private:
	SDL_Window* window_handle_;
};
