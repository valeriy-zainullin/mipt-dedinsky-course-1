#include "text.h"

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: maybe process somehow bad utf-8 files. Meaning that do something if there's an error. What to do with qsort interface - I don't know. Maybe validate file before sorting? Must be a good idea.

static bool get_file_size(FILE* stream, size_t* size) {
	if (fseek(stream, 0, SEEK_END) != 0) {
		return false;
	}
	long size_as_long = ftell(stream);
	if (size_as_long == -1L) {
		return false;
	}
	rewind(stream);

	*size = size_as_long;
	return true;
}

static void text_delete_carriage_returns(Text* text_ptr) {
	TextIterator character_to_be_replaced = text_ptr->characters;
	TextIterator iterator = text_ptr->characters;
	TextIterator after_the_last = text_ptr->characters + text_ptr->number_of_characters;
	for (; iterator != after_the_last; ++iterator) {
		if (*iterator == '\r') {
			text_ptr->number_of_characters -= 1;
			continue;
		}
		*character_to_be_replaced = *iterator;
		character_to_be_replaced += 1;
	}
}

static const char BOM_SIGNATURE[] = "\xef\xbb\xbf";
static const size_t BOM_SIGNATURE_LENGTH = sizeof(BOM_SIGNATURE) / sizeof(char) - 1;
TextStatus text_read_from_file(Text* text_ptr, const char* path) {
	assert(text_ptr != NULL);
	assert(path != NULL);

	// In MinGW64 build, just "r" would skip '\r' bytes, but we support them.
	FILE* stream = fopen(path, "rb");
	if (stream == NULL) {
		return TEXT_FAILED_TO_OPEN_THE_FILE;
	}
	size_t file_size;
	if (!get_file_size(stream, &file_size)) {
		fclose(stream);
		return TEXT_FAILED_TO_GET_SIZE_OF_THE_FILE;
	}

	// Assuming text is UTF-8.
	char beginning[BOM_SIGNATURE_LENGTH];
	if (fread(beginning, sizeof(char), BOM_SIGNATURE_LENGTH, stream) < BOM_SIGNATURE_LENGTH) {
		return TEXT_ERROR_WHILE_READING;
	}
	if (memcmp(beginning, BOM_SIGNATURE, BOM_SIGNATURE_LENGTH) == 0) {
		// File has BOM. Skipping.
		assert(file_size >= BOM_SIGNATURE_LENGTH);
		file_size -= BOM_SIGNATURE_LENGTH;
	} else {
		rewind(stream);
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
	text_delete_carriage_returns(text_ptr);

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

static ConstTextSubstring const_text_make_substring(TextSubstring substring) {
	ConstTextSubstring const_substring = {substring.first_character, substring.after_the_last_character};
	return const_substring;
}

size_t text_count_lines(Text text) {
	if (text.number_of_characters == 0) {
		return 0;
	}

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
	lines_ptr->lines = malloc(lines_ptr->number_of_lines * sizeof(TextLine));
	if (lines_ptr->lines == NULL) {
		return false;
	}

	TextLine* current_line = lines_ptr->lines;
	size_t position = 0;
	while (position < text.number_of_characters) {
		current_line->first_character = text.characters + position;
		while (position < text.number_of_characters && text.characters[position] != '\n') {
			position += 1;
		}
		current_line->after_the_last_character = text.characters + position;

		current_line += 1;
		if (position < text.number_of_characters) {
			assert(text.characters[position] == '\n');
			position += 1;
		}
	}
	assert((size_t) (current_line - lines_ptr->lines) == lines_ptr->number_of_lines);

	return true;
}
void text_free_lines(TextLines* lines_ptr) {
	free(lines_ptr->lines);
#if defined(TEXT_DEBUG)
	lines_ptr->lines = NULL;
#endif
}

void text_terminate_lines(Text text) {
	for (size_t i = 0; i < text.number_of_characters; ++i) {
		if (text.characters[i] == '\n') {
			text.characters[i] = 0;
		}
	}
}

void text_remove_empty_lines(TextLines* lines_ptr) {
	TextLine* item_to_be_replaced = lines_ptr->lines;
	TextLine* iterator = lines_ptr->lines;
	TextLine* after_the_last = lines_ptr->lines + lines_ptr->number_of_lines;
	for (; iterator != after_the_last; ++iterator) {
		if (iterator->first_character != iterator->after_the_last_character) {
			*item_to_be_replaced = *iterator;
			item_to_be_replaced += 1;
		} else {
			lines_ptr->number_of_lines -= 1;
		}
	}
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

static size_t text_get_number_of_bytes_in_utf8_character(unsigned char character) {
#if defined(__GNUC__)
	size_t number_of_leading_ones = (size_t) __builtin_clz(
		~(
			((unsigned int) character) << ((sizeof(unsigned int) - sizeof(unsigned char)) * CHAR_BIT)
		)
	);
#else
	size_t number_of_leading_ones = 0;
	static const unsigned char FIRST_BYTE_MASK = 1 << (sizeof(unsigned char) * CHAR_BIT - 1);
	while ((character & FIRST_BYTE_MASK) != 0) {
		number_of_leading_ones += 1;
		character <<= 1;
	}
#endif
	if (number_of_leading_ones == 0) {
		// One octet. Probably an ASCII character.
		return 1;
	}
	assert(number_of_leading_ones != 1); // Can happen if file is invalid. Should not be an assert, but an if.
	return number_of_leading_ones;
}

static int text_compare_utf8_characters(const unsigned char* left_hand_side, const unsigned char* right_hand_side) {
	// printf("left_hand_side = %p, right_hand_side = %p.\n", left_hand_side, right_hand_side);
	size_t left_hand_side_length = text_get_number_of_bytes_in_utf8_character(*left_hand_side);
	size_t right_hand_side_length = text_get_number_of_bytes_in_utf8_character(*right_hand_side);
	if (left_hand_side_length != right_hand_side_length) {
		return (int) left_hand_side_length - (int) right_hand_side_length;
	}
	for (size_t i = 0; i < left_hand_side_length; ++i) {
		if (left_hand_side[i] != right_hand_side[i]) {
			return (int) left_hand_side[i] - right_hand_side[i];
		}
	}
	return 0;
}

static bool text_byte_is_utf8_continuation_byte(const unsigned char byte) {
	static const unsigned char CONTINUATION_BYTE_MASK = (1 << (sizeof(unsigned char) * CHAR_BIT - 1)) | (1 << (sizeof(unsigned char) * CHAR_BIT - 2));
	static const unsigned char CONTINUATION_BYTE_BEGINNING = 1 << (sizeof(unsigned char) * CHAR_BIT - 1);
	return (byte & CONTINUATION_BYTE_MASK) == CONTINUATION_BYTE_BEGINNING;
}

// ENH: better validation.
static const unsigned char* text_get_next_utf8_character(const unsigned char* character, const unsigned char* past_the_right_border) {
	character += text_get_number_of_bytes_in_utf8_character(*character);
	if ((void*) character >= (void*) past_the_right_border) {
		return NULL;
	}
	return character;
}

static const unsigned char* text_find_utf8_character_beginning(const unsigned char* byte, const unsigned char* left_border) {
	assert(byte >= left_border);
	while (byte != left_border && text_byte_is_utf8_continuation_byte(*byte)) {
		byte -= 1;
	}
	if (text_byte_is_utf8_continuation_byte(*byte)) {
		return NULL;
	}
	return byte;
}

#define TEXT_SUBSTR_IS_EMPTY(SUBSTR) ((void*) SUBSTR.first_character >= (void*) SUBSTR.after_the_last_character)

int const_text_compare_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side) {
	while (!TEXT_SUBSTR_IS_EMPTY(left_hand_side) && !TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		int comparison_result = text_compare_utf8_characters(left_hand_side.first_character, right_hand_side.first_character);
		if (comparison_result != 0) {
			return comparison_result;
		}
		left_hand_side.first_character = text_get_next_utf8_character(left_hand_side.first_character, left_hand_side.after_the_last_character);
		right_hand_side.first_character = text_get_next_utf8_character(right_hand_side.first_character, right_hand_side.after_the_last_character);
		if (left_hand_side.first_character == NULL || right_hand_side.first_character == NULL) {
			// Buffer for one of the hands ended before the character's encoding. Think that there's no characters then.
			return (int) ((left_hand_side.first_character == NULL) - (right_hand_side.first_character == NULL));
		}
	}
	if (TEXT_SUBSTR_IS_EMPTY(left_hand_side) && TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		return 0;
	} else if (TEXT_SUBSTR_IS_EMPTY(left_hand_side)) {
		return -1;
	} else {
		return 1;
	}
}

#define SUBSTRING_LENGTH(SUBSTRING) ((SUBSTRING).after_the_last_character - (SUBSTRING).first_character)

int const_text_compare_reversed_substrings(ConstTextSubstring left_hand_side, ConstTextSubstring right_hand_side) {
	while (!TEXT_SUBSTR_IS_EMPTY(left_hand_side) && !TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		ConstTextIterator left_hand_side_last_character = text_find_utf8_character_beginning(left_hand_side.after_the_last_character - 1, left_hand_side.first_character);
		ConstTextIterator right_hand_side_last_character = text_find_utf8_character_beginning(right_hand_side.after_the_last_character - 1, right_hand_side.first_character);
		if (left_hand_side_last_character == NULL || right_hand_side_last_character == NULL) {
			// We are skipping invalid UTF-8 byte sequences. NULL is returned if it turned out that no valid UTF-8 characters are left.
			return (int) ((left_hand_side_last_character == NULL) - (right_hand_side_last_character == NULL));
		}
		int comparison_result = text_compare_utf8_characters(left_hand_side_last_character, right_hand_side_last_character);
		if (comparison_result != 0) {
			return comparison_result;
		}
		left_hand_side.after_the_last_character = left_hand_side_last_character;
		right_hand_side.after_the_last_character = right_hand_side_last_character;
	}
	if (TEXT_SUBSTR_IS_EMPTY(left_hand_side) && TEXT_SUBSTR_IS_EMPTY(right_hand_side)) {
		return 0;
	} else if (TEXT_SUBSTR_IS_EMPTY(left_hand_side)) {
		return -1;
	} else {
		return 1;
	}
}
