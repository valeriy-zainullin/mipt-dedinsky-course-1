#pragma once

#include "font_opener.h"

class FpsCounter {
public:
	FpsCounter(const char* font_path, int font_ptsize)
	  : font_opener(font_path, font_ptsize) {}
	
	static const char FPS_FONT_FILE[] = "ubuntu-mono/UbuntuMono-Regular.ttf";
	static const int FPS_FONT_SIZE = 32; // In points.
	TTF_Font* fps_font = TTF_OpenFont(FPS_FONT_FILE, FPS_FONT_SIZE);
	if (fps_font == NULL) {
		fprintf(stderr, "Failed to open fps font. TTF_OpenFont failed: %s.\n", TTF_GetError());
		show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2_TTF", "Ошибка при загрузке шрифта для счётчика FPS. Подробнее в логе (stderr).", window);
		TTF_CloseFont(
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 6;
	}
	
private:
	FontOpener font_opener;

	// Для самого первого кадра 1, чтобы FPS был равен 1.
	// Точное значение fps для первого кадра не известно,
	// мы не можем посчитать время подготовки первого
	// кадра во время подготовки первого кадра.
	// Более детально: я отображаю моментальный fps
	// предыдущего кадра в текущем, иначе, если брать
	// время подготовки текущего кадра при вычислении
	// fps, то его нужно рисовать после RenderPresent,
	// а это уже другой кадр, т.е. тогда RenderPresent
	// во временной промежуток подготовки кадра не
	// попадает.
	float prev_frame_time;
