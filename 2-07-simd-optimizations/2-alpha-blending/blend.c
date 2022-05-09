#include "blend.h"

#include <SDL2/SDL.h>

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

struct blend_pictures* blend_pictures_new(char const* background_path, char const* foreground_path) {
	struct blend_pictures* pictures = calloc(1, sizeof(struct blend_pictures));
	if (pictures == NULL) {
		return NULL;
	}
	
	// extract_pixels(pictures->background, background_path);
	// extract_pixels(pictures->foreground, foreground_path);
		
	extract_pixels(pictures->background, "Table1x4.bmp", false);
	extract_pixels(pictures->foreground, "Racket1x4.bmp", true);

	(void) background_path;
	(void) foreground_path;
		
	// memcpy(pictures->background, background, SCREEN_ROWS * SCREEN_COLS * sizeof(struct blend_pictures));
	// memcpy(pictures->foreground, foreground, SCREEN_ROWS * SCREEN_COLS * sizeof(struct blend_pictures));
	
	return pictures;
}

struct blend_pictures* blend_pictures_delete(struct blend_pictures* blend_pictures) {
	// Allow already freed objects.
	if (blend_pictures == NULL) {
		return NULL;
	}
	
	free(blend_pictures);
	return NULL;
}
