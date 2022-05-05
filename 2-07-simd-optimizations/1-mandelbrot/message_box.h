#pragma once

#if defined(_WIN32)

// Simply using SDL_ShowSimpleMessageBox on windows is not an idea as it doesn't look
// native if compiled with cygwin's mingw.

#include <windows.h>

#include <stdio.h>

#include <SDL2/SDL_syswm.h>

#define MESSAGEBOX_ERROR       MB_ICONERROR
#define MESSAGEBOX_WARNING     MB_ICONWARNING
#define MESSAGEBOX_INFORMATION MB_ICONINFORMATION

static HWND get_sdl_window_native_handle(SDL_Window* window) {
	if (window == NULL) {
		return NULL;
	}

	SDL_SysWMInfo system_info;
	SDL_VERSION(&system_info.version);
	if (!SDL_GetWindowWMInfo(window, &system_info)) {
		fprintf(stderr, "Failed to get SDL_Window native handle. Using NULL instead (no parent for dialogs). SDL_GetWindowWMInfo failed: %s.\n", SDL_GetError());
		return NULL;
	}

	HWND handle = system_info.info.win.window;

	return handle;
}

#define show_message_box(TYPE, TITLE, TEXT, WINDOW) MessageBoxW(get_sdl_window_native_handle(WINDOW), L ## TEXT, L ## TITLE, TYPE)

#else

#include <SDL2/SDL.h>

#define MESSAGEBOX_ERROR       SDL_MESSAGEBOX_ERROR
#define MESSAGEBOX_WARNING     SDL_MESSAGEBOX_WARNING
#define MESSAGEBOX_INFORMATION SDL_MESSAGEBOX_INFORMATION

static void show_message_box(unsigned int type, char const * title, char const* text, SDL_Window* owner_window) {
	if (SDL_ShowSimpleMessageBox(type, title, text, owner_window) != 0) {
		fprintf(stderr, "Failed to show message box. SDL_ShowSimpleMessageBox failed: %s.\n", SDL_GetError());
	}
}

#endif
