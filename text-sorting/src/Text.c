#include "Text.h"

#include <stddef.h>

size_t text_count_lines(Text text) {
	size_t number_of_lines = 0;
	for (size_t position = 0; position < text.number_of_characters; ++position) {
		if (text.characters[position] == '\n') {
			number_of_lines += 1;
		}
	}
	if (text.characters[text.number_of_characters - 1] != '\n') {
		number_of_lines += 1;
	}
	return number_of_lines;
}

bool text_select_lines(Text text, TextLines* lines_ptr) {
	lines_ptr->number_of_lines = text_count_lines(text);
	lines_ptr->lines = malloc(lines_ptr->number_of_lines * sizeof(Line));
	if (lines_ptr->lines == NULL) {
		return false;
	}

	Line* current_line = lines_ptr->lines;
	for (size_t position = 0; position < text.number_of_characters; ++position) {
		current_line->first_character = text.characters + position;
		current_line->after_the_last_character = current_line->first_character;

		while (position < text.number_of_characters && text.characters[position] != '\n') {
			current_line->after_the_last_character += 1;
			position += 1;
		}
		current_line->after_the_last_character += 1;

		current_line += 1;
	}

	return true;
}
void text_free_lines(TextLines lines) {
	free(lines.lines);
}

int text_compare_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side) {
	return text_compare_const_substrings(
		text_make_substring_const(left_hand_side),
		text_make_substring_const(right_hand_side)
	);
}

int text_compare_reversed_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side) {
	return text_compare_reversed_const_substrings(text_make_substring_const(left_hand_side), text_make_substring_const(right_hand_side));
}

#define TEXT_SUBSTR_IS_EMPTY(SUBSTR) (SUBSTR.first_character == SUBSTR.after_the_last_character)

int text_compare_const_substrings(TextConstSubstring left_hand_side, TextConstSubstring right_hand_side) {
	while (!TEXT_SUBSTR_IS_EMPTY(left_hand_side) && !TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		if (*left_hand_side.first_character != *right_hand_side.first_character) {
			return (int) *left_hand_side.first_character - *right_hand_side.first_character;
		}
		left_hand_side.first_character += 1;
		right_hand_side.first_character += 1;
	}
	if (TEXT_SUBSTR_IS_EMPTY(left_hand_side) && TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		return 0;
	} else if (TEXT_SUBSTR_IS_EMPTY(left_hand_side)) {
		return -*right_hand_side.first_character;
	} else {
		return *left_hand_side.first_character;
	}
}

int text_compare_reversed_const_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side) {
	while (!TEXT_SUBSTR_IS_EMPTY(left_hand_side) && !TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		TextIterator left_hand_side_last_character = left_hand_side.after_the_last_character - 1;
		TextIterator right_hand_side_last_character = right_hand_side.after_the_last_character - 1;
		if (*left_hand_side_last_character != *left_hand_side_last_character) {
			return (int) *left_hand_side_last_character - *right_hand_side_last_character;
		}
		left_hand_side.after_the_last_character = left_hand_side_last_character;
		right_hand_side.after_the_last_character = right_hand_side_last_character;
	}
	if (TEXT_SUBSTR_IS_EMPTY(left_hand_side) && TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		return 0;
	} else if (TEXT_SUBSTR_IS_EMPTY(left_hand_side)) {
		return -*(right_hand_side.after_the_last_character - 1);
	} else {
		return *(left_hand_side.after_the_last_character - 1);
	}
}
