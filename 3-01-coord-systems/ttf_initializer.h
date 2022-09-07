#pragma once

#include <SDL2/SDL_ttf.h>

class TtfInitializer {
public:
	TtfInitializer()
	  : ttf_init_result(0) {
		assert(TTF_WasInit() == 0);
		ttf_init_result = TTF_Init();

		if (!initialized()) {
			fprintf(stderr, "TTF_Init failed: %s.\n", TTF_GetError());
		}
	}
	
	bool initialized() {
		return ttf_init_result == 0;
	}
			
	~TtfInitializer() {
		if (ttf_init_result == 0) {
			TTF_Quit();
		}
	}
private:
	int ttf_init_result;
};
