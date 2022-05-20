#include "blend.h"

#include <SDL2/SDL.h>

#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool extract_pixels(struct rgba * output_pixels, char const* image_path, bool transparent) {
	SDL_Surface* image_surface = SDL_LoadBMP(image_path);
	if (image_surface == NULL) {
		fprintf(stderr, "Failed to load image \"%s\". SDL_LoadBMP failed: %s.\n", image_path, SDL_GetError());
		return false;
	}
	
	if (image_surface->w != SCREEN_COLS || image_surface->h != SCREEN_ROWS) {
		fprintf(stderr, "Error while loading \"%s\": width and height don't correspond window dimensions.\n", image_path);
		return false;
	}
	
	// Looks like SDL orders these fields big-endian, so for the structure those will be RGBA.
	SDL_Surface* image_converted_surface = SDL_ConvertSurfaceFormat(image_surface, SDL_PIXELFORMAT_ABGR8888, 0);
	
	if (image_converted_surface == NULL) {
		fprintf(stderr, "Failed to convert image surface of \"%s\". SDL_ConvertSurfaceFormat failed: %s.\n", image_path, SDL_GetError());
		SDL_FreeSurface(image_surface);
		return false;
	}
	
	SDL_LockSurface(image_converted_surface);
	
	memcpy(output_pixels, image_converted_surface->pixels, SCREEN_ROWS * SCREEN_COLS * sizeof(struct rgba));
	
	SDL_UnlockSurface(image_converted_surface);
	
	SDL_FreeSurface(image_converted_surface);
	SDL_FreeSurface(image_surface);
	
	if (transparent) {
		// Would create another static const with name BLACK and initialize
		// TRANSPARENT_COLOR with it, but cannot do it in C :)
		static const struct rgba TRANSPARENT_COLOR = {0, 0, 0, 255};        // BLACK.
		for (size_t i = 0; i < SCREEN_ROWS * SCREEN_COLS; ++i) {
			if (
				output_pixels[i].red == TRANSPARENT_COLOR.red &&
				output_pixels[i].green == TRANSPARENT_COLOR.green &&
				output_pixels[i].blue == TRANSPARENT_COLOR.blue
			) {
				output_pixels[i].alpha = 0;
			}
		}
	}
	
	return true;
}

#if defined(_WIN32)
// https://github.com/ebassi/graphene/issues/83#issuecomment-264557239
#define aligned_alloc(ALIGNMENT, SIZE) _aligned_malloc(SIZE, ALIGNMENT)
#define aligned_free(PTR)              _aligned_free(PTR)
#else
#define aligned_free(PTR)              free(PTR)
#endif

static char const * const BACKGROUND_FILE = "Table.bmp";
static char const * const FOREGROUND_FILE = "Racket.bmp";

struct blend_pictures* blend_pictures_new() {
	struct blend_pictures* pictures = aligned_alloc(BLEND_BIGGEST_ALIGNMENT, sizeof(struct blend_pictures));
	if (pictures == NULL) {
		return NULL;
	}
	
	extract_pixels(pictures->background, BACKGROUND_FILE, false);
	extract_pixels(pictures->foreground, FOREGROUND_FILE, true);

	return pictures;
}

struct blend_pictures* blend_pictures_delete(struct blend_pictures* blend_pictures) {
	// Allow already freed objects.
	if (blend_pictures == NULL) {
		return NULL;
	}
	
	aligned_free(blend_pictures);
	return NULL;
}
