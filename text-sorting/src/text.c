#include "text.h"

#include <stddef.h>
#include <stdlib.h>

static ConstTextSubstring const_text_make_substring(TextSubstring substring) {
	ConstTextSubstring const_substring = {substring.first_character, substring.after_the_last_character};
	return const_substring;
}

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

bool text_is_supported(Text text) {
	// Assuming text is UTF-8.
	if (text->number_of_characters >= 3U && memcmp(text->characters, "\xef\xbb\xbf") == 0) {
		// File has BOM. Not supported.
		return false;
	}
	return true;
}

bool text_select_lines(Text text, TextLines* lines_ptr) {
	lines_ptr->number_of_lines = text_count_lines(text);
	lines_ptr->lines = malloc(lines_ptr->number_of_lines * sizeof(TextLine));
	if (lines_ptr->lines == NULL) {
		return false;
	}

	TextLine* current_line = lines_ptr->lines;
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
	return const_text_compare_substrings(
		const_text_make_substring(left_hand_side),
		const_text_make_substring(right_hand_side)
	);
}

int text_compare_reversed_substrings(TextSubstring left_hand_side, TextSubstring right_hand_side) {
	return const_text_compare_reversed_substrings(const_text_make_substring(left_hand_side), const_text_make_substring(right_hand_side));
}

#define TEXT_SUBSTR_IS_EMPTY(SUBSTR) (SUBSTR.first_character == SUBSTR.after_the_last_character)

int const_text_compare_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side) {
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

int const_text_compare_reversed_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side) {
	while (!TEXT_SUBSTR_IS_EMPTY(left_hand_side) && !TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		ConstTextIterator left_hand_side_last_character = left_hand_side.after_the_last_character - 1;
		ConstTextIterator right_hand_side_last_character = right_hand_side.after_the_last_character - 1;
		if (*left_hand_side_last_character != *right_hand_side_last_character) {
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

TextStatus text_read_from_file(Text* text_ptr, const char* path) {
	assert(text_ptr != NULL);
	assert(path != NULL);

	FILE* stream = fopen(path, "r");
	if (stream == NULL) {
		return TEXT_FAILED_TO_OPEN_THE_FILE;
	}
	size_t file_size;
	if (!get_file_size(stream, &file_size)) {
		fclose(stream);
		return TEXT_FAILED_TO_GET_SIZE_OF_THE_FILE;
	}

	if (!text_check_file_is_supported(stream)) {
		return TEXT_NOT_SUPPORTED;
	}

	text_ptr->number_of_characters = file_size;
	text_ptr->characters = malloc(file_size * sizeof(unsigned char));
	if (text_ptr->characters == NULL) {
		fclose(stream);
		return TEXT_FAILED_TO_ALLOCATE_MEMORY;
	}
	if (
		fread(text_ptr->characters, sizeof(unsigned char), text_ptr->number_of_characters, stream) <
		text_ptr->number_of_characters
	) {
		free(text_ptr->characters);
		fclose(stream);
		return TEXT_ERROR_WHILE_READING;
	}
	fclose(stream);
	return TEXT_SUCCESS;
}

void text_free(Text* text_ptr) {
	assert(text_ptr != NULL);
	assert(text_ptr->characters != NULL);

	free(text_ptr->characters);
#if defined(TEXT_DEBUG)
	text_ptr->characters = NULL;
#endif
}
