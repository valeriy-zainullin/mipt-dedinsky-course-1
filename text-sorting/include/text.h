#pragma once

#include <stdbool.h>
#include <stddef.h>

enum TextStatus {
	TEXT_SUCCESS,
	TEXT_FAILED_TO_OPEN_THE_FILE,
	TEXT_FAILED_TO_GET_SIZE_OF_THE_FILE,
	TEXT_FAILED_TO_ALLOCATE_MEMORY,
	TEXT_ERROR_WHILE_READING
};
typedef enum TextStatus TextStatus;

struct Text {
	unsigned char* characters;
	size_t number_of_characters;
};
typedef struct Text Text;
TextStatus text_read_from_file(Text* text_ptr, const char* path);
void text_free(Text* text_ptr);

struct ConstText {
	const unsigned char * const characters;
	const size_t number_of_characters;
};
typedef struct ConstText ConstText;

typedef unsigned char* TextIterator;
typedef const unsigned char* ConstTextIterator;

struct TextSubstring {
	TextIterator first_character;
	TextIterator after_the_last_character;
};
typedef struct TextSubstring TextSubstring;
typedef TextSubstring TextLine;

struct ConstTextSubstring {
	ConstTextIterator first_character;
	ConstTextIterator after_the_last_character;
};
typedef struct ConstTextSubstring ConstTextSubstring;
/* inline static ConstTextSubstring const_text_make_substring(TextSubstring substring) {
	ConstTextSubstring const_substring = {substring.first_character, substring.after_the_last_character};
	return const_substring;
} */

struct TextLines {
	TextLine* lines;
	size_t number_of_lines;
};
typedef struct TextLines TextLines;
size_t text_count_lines(Text text);
bool text_select_lines(Text text, TextLines* lines_ptr);
void text_free_lines(TextLines* lines_ptr);
void text_remove_empty_lines(TextLines* lines_ptr);

int text_compare_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side);
int text_compare_reversed_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side);

int const_text_compare_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side);
int const_text_compare_reversed_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side);

