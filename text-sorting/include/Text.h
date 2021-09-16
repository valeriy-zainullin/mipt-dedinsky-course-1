#pragma once

#include <stdbool.h>
#include <stddef.h>

struct Text {
	unsigned char* characters;
	size_t number_of_characters;
};
typedef struct Text Text;

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
inline ConstTextSubstring text_make_const_text_substring(TextSubstring substring) {
	return ConstTextSubstring{substring.first_character, substring.after_the_last_character};
}

struct TextLines {
	TextLine* lines;
	size_t number_of_lines;
};
typedef struct TextLines TextLines;
size_t text_count_lines(Text text);
bool text_select_lines(Text text, TextLines* lines_ptr);
void text_free_lines(TextLines lines);

int text_compare_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side);
int text_compare_const_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side);

int text_compare_reversed_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side);
int text_compare_reversed_const_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side);
