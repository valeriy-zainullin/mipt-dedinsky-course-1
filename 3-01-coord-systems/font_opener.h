#pragma once

#include <SDL2/SDL_ttf.h>

#include <assert.h>
#include <stdbool.h>
#include <cstddef>

class FontOpener {
public:
	FontOpener(const char* font_path, int font_ptsize)
	  : font_handle_(NULL) {
		assert(TTF_WasInit() == 1);
		font_handle_ = TTF_OpenFont(font_path, font_ptsize);
		if (font_handle_ == NULL) {
			fprintf(stderr, "TTF_OpenFont(path=\"%s\",size=%dpt) failed: %s.\n", font_path, font_ptsize, TTF_GetError());
		}
	}
	
	TTF_Font* get() {
		return font_handle_;
	}
	  	
	~FontOpener() {
		if (font_handle_ != NULL) {
			TTF_CloseFont(font_handle_);
			font_handle_ = NULL;
		}
	}
private:
	TTF_Font* font_handle_;
};
