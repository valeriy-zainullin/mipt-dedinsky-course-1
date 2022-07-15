#pragma once

#include <stddef.h>
#include <stdint.h>

// Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, C99.

// Contains token types and all token syntax in comments.

// We do token types at start and pass pointers to them, because it's better to show
// token type in type itself as well, it's better for the code understanding later.

// There is a second syntax used in tokenizer itself. It's EBNF based on these descriptions.
// See it in token_stream.c
// TODO: do such syntax. Start parsing the program. Dump tokens for now to stdout. In text form. Also there will be functionality to dump tokens to stdout or a text file.. Добавить это в план. Чтобы потом было понятно, какие фичи есть.

typedef size_t token_pos;

/*
token:
	keyword
	identifier
	constant
	string-literal
	punctuator
*/
enum token_type {
	TOKEN_EOP,              // End of program token received when a tokenstream ends.
	TOKEN_KEYWORD,
	TOKEN_IDENTIFIER,
	TOKEN_CONSTANT,
	TOKEN_STRING_LITERAL,
	TOKEN_PUNCTUATOR
};

extern const struct token TOKEN_END_OF_PROGRAM;

// Limit file size. Not more than 1GB.
typedef uint32_t token_offset;

#define MAX_INTEGER_CONSTANT_LEN 80
struct token {
	enum token_type token_type;
	token_pos start_pos;
	char text[MAX_INTEGER_CONSTANT_LEN + 1]; // All text between token start and end position (including both positions).
};

/*
keyword: one of
	auto  break case char const continue default do double else enum extern
	float for goto if inline int long register restrict return short
	signed sizeof static struct switch typedef union unsigned
	void volatile while _Bool _Complex _Imaginary
*/
struct token_keyword {
	enum token_type token_type;
	token_offset token_start;
	enum token_keyword_name {
		TOKEN_KEYWORD_AUTO,
		TOKEN_KEYWORD_BREAK,
		TOKEN_KEYWORD_CASE,
		TOKEN_KEYWORD_CHAR,
		TOKEN_KEYWORD_CONST,
		TOKEN_KEYWORD_CONTINUE,
		TOKEN_KEYWORD_DEFAULT,
		TOKEN_KEYWORD_DO,
		TOKEN_KEYWORD_DOUBLE,
		TOKEN_KEYWORD_ELSE,
		TOKEN_KEYWORD_ENUM,
		TOKEN_KEYWORD_EXTERN,
		TOKEN_KEYWORD_FLOAT,
		TOKEN_KEYWORD_FOR,
		TOKEN_KEYWORD_GOTO,
		TOKEN_KEYWORD_IF,
		TOKEN_KEYWORD_INLINE,
		TOKEN_KEYWORD_INT,
		TOKEN_KEYWORD_LONG,
		TOKEN_KEYWORD_REGISTER,
		TOKEN_KEYWORD_RESTRICT,
		TOKEN_KEYWORD_RETURN,
		TOKEN_KEYWORD_SHORT,
		TOKEN_KEYWORD_SIGNED,
		TOKEN_KEYWORD_SIZEOF,
		TOKEN_KEYWORD_STATIC,
		TOKEN_KEYWORD_STRUCT,
		TOKEN_KEYWORD_SWITCH,
		TOKEN_KEYWORD_TYPEDEF,
		TOKEN_KEYWORD_UNION,
		TOKEN_KEYWORD_UNSIGNED,
		TOKEN_KEYWORD_VOID,
		TOKEN_KEYWORD_VOLATILE,
		TOKEN_KEYWORD_WHILE,
		TOKEN_KEYWORD_UNDERSCORE_BOOL,
		TOKEN_KEYWORD_UNDERSCORE_COMPLEX,
		TOKEN_KEYWORD_UNDERSCORE_IMAGINARY
	} name;
};

int token_process_keyword(char const* text);
void token_keyword_init(struct token_keyword* keyword_token, enum token_keyword_name keyword_name, char* text);

/*
identifier:
	identifier-nondigit
	identifier identifier-nondigit
	identifier digit

identifier-nondigit:
	nondigit
	universal-character-name
	other implementation-defined characters (no in our case)

nondigit: one of
	_ a b c d e f g h i j k l m
	  n o p q r s t u v w x y z
	  A B C D E F G H I J K L M
	  N O P Q R S T U V W X Y Z
digit: one of
	0 1 2 3 4 5 6 7 8 9
*/
#define TOKEN_IDENTIFIER_MAX_LEN 1023
struct token_identifier {
	struct token token;
};

int token_process_identifier(char const* text);
void token_identifier_init(struct token_identifier* identifier_token, char const* identifier);

/*
Paragraph 6.4.2.1, semantics:
4. When preprocessing tokens are converted to tokens during translation phase 7, if a
     preprocessing token could be converted to either a keyword or an identifier, it
     is converted to a keyword.
*/

/*
constant:
	integer-constant
	floating-constant
	enumeration-constant
	character-constant
*/
// Enum constant is essentially an identifier. We won't have them as a separate entity,
// otherwise we won't be able to distinguish them from identifiers at tokenizing stage.
// Enum members will come as identifiers to the parser.
struct token_constant {
	struct token token;
	enum {
		TOKEN_CONSTANT_INTEGER,
		TOKEN_CONSTANT_FLOATING,
		TOKEN_CONSTANT_ENUMERATION,
		TOKEN_CONSTANT_CHARACTER
	} constant_type;
};

// For integer and floating constants suffixes are included into the text.

/*
integer-constant:
	decimal-constant integer-suffix_opt
	octal-constant integer-suffix_opt
	hexadecimal-constant integer-suffix_opt

decimal-constant:
	nonzero-digit
	decimal-constant digit

octal-constant:
	0
	octal-constant octal-digit

hexadecimal-constant:
	hexadecimal-prefix hexadecimal-digit
	hexadecimal-constant hexadecimal-digit

hexadecimal-prefix: one of
	0x 0X

nonzero-digit: one of
	1 2 3 4 5 6 7 8 9

octal-digit: one of
	0 1 2 3 4 5 6 7
	
integer-suffix:
	unsigned-suffix long-suffix_opt
	unsigned-suffix long-long-suffix
	long-suffix unsigned-suffix_opt
	long-long-suffix unsigned-suffix_opt

unsigned-suffix: one of
	u U
long-suffix: one of
	l L
long-long-suffix: one of
	ll LL
*/
struct token_integer_constant {
	struct token_constant constant;
	enum token_integer_constant_base {
		TOKEN_INTEGER_CONSTANT_DECIMAL_BASE,
		TOKEN_INTEGER_CONSTANT_OCTAL_BASE,
		TOKEN_INTEGER_CONSTANT_HEXADECIMAL_BASE
	} base;
	enum token_integer_constant_suffix {
		TOKEN_INTEGER_CONSTANT_UNS_SUFFIX,
		TOKEN_INTEGER_CONSTANT_UNSLONG_SUFFIX,
		TOKEN_INTEGER_CONSTANT_UNSLONGLONG_SUFFIX,

		TOKEN_INTEGER_CONSTANT_LONG_SUFFIX,
		TOKEN_INTEGER_CONSTANT_LONGUNS_SUFFIX,

		TOKEN_INTEGER_CONSTANT_LONGLONG_SUFFIX,
		TOKEN_INTEGER_CONSTANT_LONGLONGUNS_SUFFIX,
	} suffix;
};

int token_process_integer_constant(enum token_integer_constant_base base, char const* text);
void token_integer_constant_init(struct token_integer_constant* integer_constant_token, enum token_integer_constant_base base, enum token_integer_constant_suffix suffix, char const* text);

/*
floating-constant:
	decimal-floating-constant
	hexadecimal-floating-constant

decimal-floating-constant:
	fractional-constant exponent-part_opt floating-suffix_opt
	digit-sequence exponent-part floating-suffix_opt

hexadecimal-floating-constant:
	hexadecimal-prefix hexadecimal-fractional-constant binary-exponent-part floating-suffix_opt
	hexadecimal-prefix hexadecimal-digit-sequence binary-exponent-part floating-suffix_opt

fractional-constant:
	digit-sequence_opt . digit-sequence
	digit-sequence .

exponent-part:
	e sign_opt digit-sequence
	E sign_opt digit-sequence

sign: one of
	+ -

digit-sequence:
	digit
	digit-sequence digit

hexadecimal-fractional-constant:
	hexadecimal-digit-sequence_opt .
	hexadecimal-digit-sequence
	hexadecimal-digit-sequence .
binary-exponent-part:
	p sign_opt digit-sequence
	P sign_opt digit-sequence
hexadecimal-digit-sequence:
	hexadecimal-digit
	hexadecimal-digit-sequence hexadecimal-digit
floating-suffix: one of
	f l F L
*/
struct token_floating_constant {
	struct token_constant constant;
	enum token_floating_constant_base {
		TOKEN_FLOATING_CONSTANT_DECIMAL_BASE,
		TOKEN_FLOATING_CONSTANT_HEXADECIMAL_BASE
	} base;
	enum token_floating_constant_suffix {
		TOKEN_FLOATING_CONSTANT_FLOAT_SUFFIX,
		TOKEN_FLOATING_CONSTANT_LONG_SUFFIX
	} suffix;
	// TODO: more fields if and when needed.
};

int token_process_floating_constant(enum token_floating_constant_base base, char const* text);
void token_floating_constant_init(struct token_floating_constant* floating_constant_token, enum token_floating_constant_base base, enum token_floating_constant_suffix suffix, char* text);


/*
character-constant:
	' c-char-sequence '
	L' c-char-sequence '
c-char-sequence:
	c-char
	c-char-sequence c-char
c-char:
	any member of the source character set except the single-quote ', backslash \, or new-line character
	escape-sequence
escape-sequence:
	simple-escape-sequence
	octal-escape-sequence
	hexadecimal-escape-sequence
	universal-character-name
simple-escape-sequence: one of
	\' \" \? \\ \a \b \f \n \r \t \v
octal-escape-sequence:
	\ octal-digit
	\ octal-digit octal-digit
	\ octal-digit octal-digit octal-digit
hexadecimal-escape-sequence:
	\x hexadecimal-digit
	hexadecimal-escape-sequence hexadecimal-digit
*/
// c-char for us is encoded in a codepage with all 256 bytes defined, compatible with ASCII (all
// ASCII-defined bytes are same as in ASCII). We additionally don't allow raw zero-bytes inside
// string literals, as zero byte used as end of the text inside the character reader. If we
// would support it, we would need to store token length inside struct token, to not rely on
// zero byte occurence to detect length of the text member. In particular this may mean that
// source files with UTF-16 char constants and string literals are not supported (maybe, for
// now).
// It may seem char constants may have indefinite length, maybe that's true, but they are used to
// initialize int. So that's the way to limit their length. Say, their length is 15 chars max, so
// they fit into the text member of token, fine.
struct token_character_constant {
	struct token_constant constant;
	enum token_character_constant_prefix {
		TOKEN_CHARACTER_CONSTANT_NO_PREFIX,          // Normal character constant
		TOKEN_CHARACTER_CONSTANT_WIDE_CHAR_PREFIX    // Wide-character constant
	} prefix;
};

int token_process_character_constant(char const* text);
void token_character_constant_init(struct token_character_constant* character_constant_token, enum token_character_constant_prefix prefix, char* text);


/*
string-literal:
	" s-char-sequence_opt "
	L" s-char-sequence_opt "
s-char-sequence:
	s-char
	s-char-sequence s-char
s-char:
	any member of the source character set except the double-quote ", backslash \, or new-line character
		escape-sequence
*/
// s-char for us is encoded in a codepage with all 256 bytes defined, compatible with ASCII (all
// ASCII-defined bytes are same as in ASCII). We additionally don't allow raw zero-bytes inside
// string literals, as zero byte used as end of the text inside the character reader. If we
// would support it, we would need to store token length inside struct token, to not rely on
// zero byte occurence to detect length of the text member. In particular this may mean that
// source files with UTF-16 char constants and string literals are not supported (maybe, for
// now).
struct token_string_literal {
	struct token_constant constant;
	enum token_string_literal_prefix {
		TOKEN_STRING_LITERAL_NO_PREFIX,           // Normal string literal
		TOKEN_STRING_LITERAL_WIDE_CHAR_PREFIX     // Wide-char string literal
	} type;
};

int token_process_string_literal(char const* text);
void token_string_literal_init(struct token_string_literal* string_literal_token, enum token_string_literal_prefix prefix, char* text);

/*
punctuator: one of
	[ ] ( ) { } . ->
	++ -- & * + - ~ !
	/ % << >> < > <= >= == != ^ | && ||
	? : ; ...
	= *= /= %= += -= <<= >>= &= ^= |=
	, # ##
	<: :> <% %> %: %:%:
*/
struct token_punctuator {
	struct token_constant constant;
};

int token_process_punctuator(char const* text);
void token_punctuator_init(struct token_punctuator* punctuator_token, char* text);

int token_process_invalid_token(char const* text);