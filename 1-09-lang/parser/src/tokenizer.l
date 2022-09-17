/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 61, part 6.4, C99. */

/* TODO: if comments are stripped by preprocessor, move comments to preprocessor. */

/* TODO: put locations of grammar rules as done in parser. */

/* yywrap function called at the end of file, so the program could open the next file and continue scanning. */
/* We don't support this for now, but we will have to support compiling multiple files, but into a single exe, right? Not to a single .o file. I think. I should check what GCC implementation does. */
/* For debugging you could use "%option noyywrap debug verbose" as well here. */
%option noyywrap

/* To silence warnings -Werror=unused-function for input and yyunput */
%option noinput nounput

/* Getting rid of -Werror=implicit-function for fileno */
/* More info at https://stackoverflow.com/a/46223160 */
%option never-interactive

%{
	#include "tokenizer.h"
%}

/* Track line numbers as well. */
%option yylineno 
%{
	#include <assert.h>
	#define YY_USER_ACTION assert(yylineno > 0); tokenizer_current_location.line = (size_t) yylineno;
	#include "tokenizer.h"
	struct tokenizer_location tokenizer_current_location = {0};
	
	#include <string.h>
	void accnt_mltln_comment_in_location(char const* text);
%}

/*
token:
	keyword
	identifier
	constant
	string-literal
	punctuator
;
*/

blank [ \t]

/*
keyword: one of
	auto  break case char const continue default do double else enum extern
	float for goto if inline int long register restrict return short
	signed sizeof static struct switch typedef union unsigned
	void volatile while _Bool _Complex _Imaginary
*/
keyword (auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while|_Bool|_Complex|_Imaginary)

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
nondigit   [_a-zA-Z]
digit      [0-9]
identifier {nondigit}({nondigit}|{digit})*

/*
constant:
	integer-constant
	floating-constant
	enumeration-constant
	character-constant
*/
/* Enum constant is essentially an identifier. We won't have them as a separate entity, otherwise we won't be able to distinguish them from identifiers at tokenizing stage. */
/* Enum members will come as identifiers to the parser. */
/*
We will be able to implement enum constant though by using flex start conditions.
https://westes.github.io/flex/manual/Flex-is-not-matching-my-patterns-in-the-same-order-that-I-defined-them_002e.html
*/

/*
integer-constant:
	decimal-constant integer-suffix_opt
	octal-constant integer-suffix_opt
	hexadecimal-constant integer-suffix_opt
*/
integer_constant ({decimal_constant}{integer_suffix}?)|({octal_constant}{integer_suffix}?)|({hexadecimal_constant}{integer_suffix}?)
/*
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
unsigned_suffix u|U
long_suffix l|L
long_long_suffix ll|LL
integer_suffix (({unsigned_suffix}{long_suffix}?)|({unsigned_suffix}{long_long_suffix})|({long_suffix}{unsigned_suffix}?)|({long_long_suffix}{unsigned_suffix}?))
/*
nonzero-digit: one of
	1 2 3 4 5 6 7 8 9
*/
nonzero_digit [1-9]
/*
decimal-constant:
	nonzero-digit
	decimal-constant digit
*/
decimal_constant {nonzero_digit}{digit}*
/*
octal-digit: one of
	0 1 2 3 4 5 6 7
*/
octal_digit [0-7]
/*
octal-constant:
	0
	octal-constant octal-digit
*/
octal_constant 0{octal_digit}*
/*
hexadecimal-digit: one of
	0 1 2 3 4 5 6 7 8 9
	a b c d e f
	A B C D E F
*/
hexadecimal_digit [0-9a-fA-F]
/*
hexadecimal-prefix: one of
	0x 0X
*/
hexadecimal_prefix 0[xX]
/*
hexadecimal-constant:
	hexadecimal-prefix hexadecimal-digit
	hexadecimal-constant hexadecimal-digit
*/
hexadecimal_constant {hexadecimal_prefix}{hexadecimal_digit}{hexadecimal_digit}*

/*
floating-constant:
	decimal-floating-constant
	hexadecimal-floating-constant
*/
/*
floating-suffix: one of
	f l F L
*/
floating_suffix [flFL]
/*
fractional-constant:
	digit-sequence_opt . digit-sequence
	digit-sequence .
*/
fractional_constant ({digit_sequence}?\.{digit_sequence})|({digit_sequence}\.)
/*
sign: one of
	+ -
*/
sign [+-]
/*
digit-sequence:
	digit
	digit-sequence digit
*/
digit_sequence {digit}{digit}*
/*
exponent-part:
	e sign_opt digit-sequence
	E sign_opt digit-sequence
*/
exponent_part [eE]{sign}?{digit_sequence}
/*
decimal-floating-constant:
	fractional-constant exponent-part_opt floating-suffix_opt
	digit-sequence exponent-part floating-suffix_opt
*/
decimal_floating_constant (({fractional_constant}{exponent_part}?{floating_suffix}?)|({digit_sequence}{exponent_part}{floating_suffix}?))
/*
hexadecimal-fractional-constant:
	hexadecimal-digit-sequence_opt .
	hexadecimal-digit-sequence
	hexadecimal-digit-sequence .
*/
hexadecimal_fractional_constant (({hexadecimal_digit_sequence}?\.)|{hexadecimal_digit_sequence}|{hexadecimal_digit_sequence}\.)
/*
binary-exponent-part:
	p sign_opt digit-sequence
	P sign_opt digit-sequence
*/
binary_exponent_part [pP]{sign}?{digit_sequence}
/*
hexadecimal-digit-sequence:
	hexadecimal-digit
	hexadecimal-digit-sequence hexadecimal-digit
*/
hexadecimal_digit_sequence {hexadecimal_digit}{hexadecimal_digit}*
/*
hexadecimal-floating-constant:
	hexadecimal-prefix hexadecimal-fractional-constant binary-exponent-part floating-suffix_opt
	hexadecimal-prefix hexadecimal-digit-sequence binary-exponent-part floating-suffix_opt
*/
hexadecimal_floating_constant (({hexadecimal_prefix}{hexadecimal_fractional_constant}{binary_exponent_part}{floating_suffix}?)|({hexadecimal_prefix}{hexadecimal_digit_sequence}{binary_exponent_part}{floating_suffix}?))

/*
simple-escape-sequence: one of
	\' \" \? \\ \a \b \f \n \r \t \v
*/
simple_escape_sequence (\\\'|\\\"|\\\?|\\\\|\\a|\\b|\\f|\\n|\\r|\\t|\\v)
/*
octal-escape-sequence:
	\ octal-digit
	\ octal-digit octal-digit
	\ octal-digit octal-digit octal-digit
*/
octal_escape_sequence (\\{octal_digit}|\\{octal_digit}{octal_digit}|\\{octal_digit}{octal_digit}{octal_digit})
/*
hexadecimal-escape-sequence:
	\x hexadecimal-digit
	hexadecimal-escape-sequence hexadecimal-digit
*/
hexadecimal_escape_sequence (\\x{hexadecimal_digit})(\\x{hexadecimal_digit})*
/*
escape-sequence:
	simple-escape-sequence
	octal-escape-sequence
	hexadecimal-escape-sequence
	universal-character-name
*/
/*
Paragraph 6.4.2.1, semantics:
3. Each universal character name in an identifier shall designate a character whose encoding
     in ISO/IEC 10646 falls into one of the ranges specified in annex D.60) The initial
     character shall not be a universal character name designating a digit. An implementation
     may allow multibyte characters that are not part of the basic source character set to
     appear in identifiers; which characters and their correspondence to universal character
     names is implementation-defined.
*/
/* We don't support universal character names for now. */
escape_sequence ({simple_escape_sequence}|{octal_escape_sequence}|{hexadecimal_escape_sequence})
/*
c-char:
	any member of the source character set except the single-quote ', backslash \, or new-line character
	escape-sequence
*/
c_char ([^'\\\n]|{escape_sequence})
/*
c-char-sequence:
	c-char
	c-char-sequence c-char
*/
c_char_sequence {c_char}{c_char}*
/*
character-constant:
	' c-char-sequence '
	L' c-char-sequence '
*/
character_constant (\'{c_char_sequence}\'|L\'{c_char_sequence}\')

/*
s-char:
	any member of the source character set except the double-quote ", backslash \, or new-line character
	escape-sequence
*/
s_char ([^"\\\n]|{escape_sequence})
/*
s-char-sequence:
	s-char
	s-char-sequence s-char
*/
s_char_sequence {s_char}{s_char}*
/*
string-literal:
	" s-char-sequence_opt "
	L" s-char-sequence_opt "
*/
string_literal (\"{s_char_sequence}?\"|L\"{s_char_sequence}\")

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
/* Seems to select the longest case here too. Checked in practice. Great! */
punctuator ([\[\](){}.&*+\-~!/%<>^|?:;=,#]|->|"++"|"--"|<<|>>|<=|>=|==|!=|&&|"||"|"..."|"*="|"/="|%=|"+="|-=|<<=|>>=|&=|^=|\|=|##|<:|:>|<%|%>|%:|%:%:)

/*
https://stackoverflow.com/a/25396611
	"This one is easy because in lex/flex, `.` won't match a newline. So the
	following will match from `//` to the end of the line, and then do nothing."
	"//".*
*/

/* https://stackoverflow.com/a/28766594 Flex takes the longest match, so I think, we are good with identifiers! Of course, flex should be well-developed tool for various cases of parsing and tokenizing.*/
/* To do that manually with a recursive descend, I think, we would have to make a tree of reading (DFA) and then make a new grammar, recursive descend grammar. The flex, however, uses regular expressions, which are, without backtracking, mathematically equivalent to DFAs.*/
/*
Paragraph 6.4.2.1, semantics:
4. When preprocessing tokens are converted to tokens during translation phase 7, if a
     preprocessing token could be converted to either a keyword or an identifier, it
     is converted to a keyword.
*/

/* TODO: do we need to extract the numbers right away? And their bases? And the same for floating point numbers as well?
   If so, do this. It will be clear when codegenereation will be considered.
   Maybe we will be codegenerating something our arch doesn't support. As a cross-compilator. So maybe we should leave values as is for code generator. Just check their correctness as tokens.
*/

/* TODO: if decimal and hexadecimal floating constants are distinguished at parsing stage, unify them here. */

/* Seems like flex selects the rule matching the most text and if there are multiple of them, selects the
   top one. Mb check if needed.
*/

%%
{blank}                                  { tokenizer_current_location.col += 1;                }
\r\n?|\n                                 { tokenizer_current_location.col = 1;                 }

"//".*                                   { /* location column is updated in the rule for \r and \n. */ }
"/*"([^*]|[*][^/])*"*/"                  { accnt_mltln_comment_in_location(yytext);            }

{keyword}                                { return tokenizer_handle_keyword(yytext);            }

{identifier}                             { return tokenizer_handle_identifier(yytext);         }

{integer_constant}                       { return tokenizer_handle_integer_constant(yytext);   }

{decimal_floating_constant}              { return tokenizer_handle_floating_constant(yytext);  }
{hexadecimal_floating_constant}          { return tokenizer_handle_floating_constant(yytext);  }

{character_constant}                     { return tokenizer_handle_character_constant(yytext); }
{string_literal}                         { return tokenizer_handle_string_literal(yytext);     }

{punctuator}                             { return tokenizer_handle_punctuator(yytext);         }

.                                        { return tokenizer_handle_invalid_token(yytext);      }
%%

#include <stdint.h>

// TODO: check newlines aren't counted twice for multiline comments.
static uint32_t count_newlines(char const* text) {
	// TODO: check file size before even reading it. Not more than 512MB. Select a different data type. uint32_t can hold 512MB.
	// TODO: use uint32_t in token_location and ast_location.
	uint32_t num_newlines = 0;
	for (; *text != '\0'; ++text) {
		if (*text == '\r') {
			num_newlines += 1;
			if (text[1] == '\n') {
				++text;
			}
		}
		if (*text == '\n') {
			num_newlines += 1;
		}
	}
	return num_newlines;
}

void accnt_mltln_comment_in_location(const char* text) {
	uint32_t num_lines = count_newlines(text);
	tokenizer_current_location.line += num_lines;
	if (num_lines > 0) {
		tokenizer_current_location.col = 1;
	}
}

void tokenizer_notify_token_handled() {
	tokenizer_current_location.col += strlen(yytext);
}