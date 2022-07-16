/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 421, annex A: language syntax summary, part 2: phrase structure grammar, C99. */

// TODO: read about bison algorithm.

%{
#include "token.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

int yylex();
void yyerror (char const * string) {
	fprintf(stderr, "%s\n", string);
}
%}

%token keyword_auto keyword_break keyword_case keyword_char keyword_const keyword_continue keyword_default
%token keyword_do keyword_double keyword_else keyword_enum keyword_extern keyword_float keyword_for
%token keyword_goto keyword_if keyword_inline keyword_int keyword_long keyword_register keyword_restrict
%token keyword_return keyword_short keyword_signed keyword_sizeof keyword_static keyword_struct
%token keyword_switch keyword_typedef keyword_union keyword_unsigned keyword_void keyword_volatile
%token keyword_while keyword__Bool keyword__Complex keyword__Imaginary

%token identifier constant string_literal

/*
TODO: standard reference here.
	[ ] ( ) { } . ->
	++ -- & * + - ~ !
	/ % << >> < > <= >= == != ^ | && ||
	? : ; ...
	= *= /= %= += -= <<= >>= &= ^= |=
	, # ##
	<: :> <% %> %: %:%:
*/
%token punctuator_left_square_bracket punctuator_right_square_bracket
%token punctuator_left_parenthesis punctuator_right_parenthesis
%token punctuator_left_brace punctuator_right_brace
%token punctuator_dot punctuator_arrow punctuator_increment punctuator_decrement punctuator_and
%token punctuator_star punctuator_plus punctuator_minus punctuator_tilde punctuator_exclamation_mark
%token punctuator_slash punctuator_percent punctuator_shift_left punctuator_shift_right
%token punctuator_less punctuator_greater punctuator_less_equal punctuator_greater_equal
%token punctuator_equal_equal punctuator_not_equal
%token punctuator_circumflex punctuator_or punctuator_and_and punctuator_or_or
%token punctuator_question_mark punctuator_colon punctuator_semicolon punctuator_ellipsis
%token punctuator_equal punctuator_star_equal punctuator_slash_equal punctuator_percent_equal
%token punctuator_plus_equal punctuator_minus_equal punctuator_shift_left_equal punctuator_shift_right_equal
%token punctuator_and_equal punctuator_circumflex_equal punctuator_or_equal
%token punctuator_comma punctuator_hash punctuator_hash_hash
%token punctuator_less_colon punctuator_colon_greater punctuator_less_percent punctuator_percent_greater
%token punctuator_percent_colon punctuator_percent_colon_percent_colon

%start translation_unit

%type translation_unit
%%

identifier_opt:
  %empty
| identifier
;

/*
(6.5.1) primary-expression:
	identifier
	constant
	string-literal
	( expression )
*/
primary_expression:
  identifier
| constant
| string_literal
| punctuator_left_parenthesis expression punctuator_right_parenthesis
;

/*
(6.5.2) postfix-expression:
	primary-expression
	postfix-expression [ expression ]
	postfix-expression ( argument-expression-list_opt )
	postfix-expression . identifier
	postfix-expression -> identifier
	postfix-expression ++
	postfix-expression --
	( type-name ) { initializer-list }
	( type-name ) { initializer-list , }
*/
postfix_expression:
  primary_expression
| postfix_expression punctuator_left_square_bracket expression punctuator_right_square_bracket
| postfix_expression punctuator_left_parenthesis argument_expression_list_opt punctuator_right_parenthesis
| postfix_expression punctuator_dot identifier
| postfix_expression punctuator_arrow identifier
| postfix_expression punctuator_increment
| postfix_expression punctuator_decrement
| punctuator_left_parenthesis type_name punctuator_right_parenthesis punctuator_left_brace initializer_list punctuator_right_brace
| punctuator_left_parenthesis type_name punctuator_right_parenthesis punctuator_left_brace initializer_list punctuator_comma punctuator_right_brace
;

/*
(6.5.2) argument-expression-list:
	assignment-expression
	argument-expression-list , assignment-expression
*/
argument_expression_list:
  assignment_expression
| argument_expression_list punctuator_comma assignment_expression
;

argument_expression_list_opt:
  %empty
| argument_expression_list
;

/*
(6.5.3) unary-expression:
	postfix-expression
	++ unary-expression
	-- unary-expression
	unary-operator cast-expression
	sizeof unary-expression
	sizeof ( type-name )
*/
unary_expression:
  postfix_expression
| punctuator_increment unary_expression
| punctuator_decrement unary_expression
| unary_operator cast_expression
| keyword_sizeof unary_expression
| keyword_sizeof punctuator_left_parenthesis type_name punctuator_right_parenthesis
;

/*
(6.5.3) unary-operator: one of
	& * + - ~ !
*/
unary_operator:
  punctuator_and | '*' | '+' | '-' | punctuator_tilde | punctuator_exclamation_mark;

/*
(6.5.4) cast-expression:
	unary-expression
	( type-name ) cast-expression
*/
cast_expression:
  unary_expression
| punctuator_left_parenthesis type_name punctuator_right_parenthesis cast_expression
;

/*
(6.5.5) multiplicative-expression:
	cast-expression
	multiplicative-expression * cast-expression
	multiplicative-expression / cast-expression
	multiplicative-expression % cast-expression
*/
multiplicative_expression:
  cast_expression
| multiplicative_expression '*' cast_expression
| multiplicative_expression '/' cast_expression
| multiplicative_expression '%' cast_expression
;

/*
(6.5.6) additive-expression:
	multiplicative-expression
	additive-expression + multiplicative-expression
	additive-expression - multiplicative-expression
*/
additive_expression:
  multiplicative_expression
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

/*
(6.5.7) shift-expression:
	additive-expression
	shift-expression << additive-expression
	shift-expression >> additive-expression
*/
shift_expression:
  additive_expression
| shift_expression punctuator_shift_left additive_expression
| shift_expression punctuator_shift_right additive_expression
;

/*
(6.5.8) relational-expression:
	shift-expression
	relational-expression < shift-expression
	relational-expression > shift-expression
	relational-expression <= shift-expression
	relational-expression >= shift-expression
*/
relational_expression:
  shift_expression
| relational_expression punctuator_less shift_expression
| relational_expression punctuator_greater shift_expression
| relational_expression punctuator_less_equal shift_expression
| relational_expression punctuator_greater_equal shift_expression
;

/*
(6.5.9) equality-expression:
	relational-expression
	equality-expression == relational-expression
	equality-expression != relational-expression
*/
equality_expression:
  relational_expression
| equality_expression punctuator_equal_equal relational_expression
| equality_expression punctuator_not_equal relational_expression
;

/*
(6.5.10) AND-expression:
	equality-expression
	AND-expression & equality-expression
*/
AND_expression:
  equality_expression
| AND_expression punctuator_and equality_expression
;

/*
(6.5.11) exclusive-OR-expression:
	AND-expression
	exclusive-OR-expression ^ AND-expression
*/
exclusive_OR_expression:
  AND_expression
| exclusive_OR_expression '^' AND_expression
;

/*
(6.5.12) inclusive-OR-expression:
	exclusive-OR-expression
	inclusive-OR-expression | exclusive-OR-expression
*/
inclusive_OR_expression:
  exclusive_OR_expression
| inclusive_OR_expression '|' exclusive_OR_expression
;

/*
(6.5.13) logical-AND-expression:
	inclusive-OR-expression
	logical-AND-expression && inclusive-OR-expression
*/
logical_AND_expression:
  inclusive_OR_expression
| logical_AND_expression punctuator_and_and inclusive_OR_expression
;

/*
(6.5.14) logical-OR-expression:
	logical-AND-expression
	logical-OR-expression || logical-AND-expression
*/
logical_OR_expression:
  logical_AND_expression
| logical_OR_expression punctuator_or_or logical_AND_expression
;

/*
(6.5.15) conditional-expression:
	logical-OR-expression
	logical-OR-expression ? expression : conditional-expression
*/
conditional_expression:
  logical_OR_expression
| logical_OR_expression punctuator_question_mark expression punctuator_colon conditional_expression
;

/*
(6.5.16) assignment-expression:
	conditional-expression
	unary-expression assignment-operator assignment-expression
*/
assignment_expression:
  conditional_expression
| unary_expression assignment_operator assignment_expression
;

assignment_expression_opt:
  %empty
| assignment_expression
;

/*
(6.5.16) assignment-operator: one of
	= *= /= %= += -= <<= >>= &= ^= |=
*/
assignment_operator:
  punctuator_equal
| punctuator_star_equal
| punctuator_slash_equal
| punctuator_percent_equal
| "+="
| "-="
| "<<="
| ">>="
| "&="
| "^="
| "|="
;

/*
(6.5.17) expression:
	assignment-expression
	expression , assignment-expression
*/
expression:
  assignment_expression
| expression punctuator_comma assignment_expression
;

expression_opt:
  %empty
| expression
;

/*
(6.6) constant-expression:
	conditional-expression
*/
constant_expression:
  conditional_expression
;

/*
(6.7) declaration:
	declaration-specifiers init-declarator-list_opt ;
*/
declaration:
  declaration_specifiers init_declarator_list_opt
;

/*
(6.7) declaration-specifiers:
	storage-class-specifier declaration-specifiers_opt
	type-specifier declaration-specifiers_opt
	type-qualifier declaration-specifiers_opt
	function-specifier declaration-specifiers_opt
*/
declaration_specifiers:
  storage_class_specifier declaration_specifiers_opt
| type_specifier declaration_specifiers_opt
| type_qualifier declaration_specifiers_opt
| function_specifier declaration_specifiers_opt
;

declaration_specifiers_opt:
  %empty
| declaration_specifiers
;

/*
(6.7) init-declarator-list:
	init-declarator
	init-declarator-list , init-declarator
*/
init_declarator_list:
  init_declarator
| init_declarator_list punctuator_comma init_declarator
;

init_declarator_list_opt:
  %empty
| init_declarator_list
;

/*
(6.7) init-declarator:
	declarator
	declarator = initializer
*/
init_declarator:
  declarator
| declarator '=' initializer
;

/*
(6.7.1) storage-class-specifier:
	typedef
	extern
	static
	auto
	register
*/
storage_class_specifier:
  keyword_typedef
| keyword_extern
| keyword_static
| keyword_auto
| keyword_register
;

/*
(6.7.2) type-specifier:
	void
	char
	short
	int
	long
	float
	double
	signed
	unsigned
	_Bool
	_Complex
	struct-or-union-specifier ∗
	enum-specifier
	typedef-name
*/
type_specifier:
  keyword_void
| keyword_char
| keyword_short
| keyword_int
| keyword_long
| keyword_float
| keyword_double
| keyword_signed
| keyword_unsigned
| keyword__Bool
| keyword__Complex
| struct_or_union_specifier '*'
| enum_specifier
| typedef_name
;

/*
(6.7.2.1) struct-or-union-specifier:
	struct-or-union identifier_opt { struct-declaration-list }
	struct-or-union identifier
*/
struct_or_union_specifier:
  struct_or_union identifier_opt punctuator_left_brace struct_declaration_list punctuator_right_brace
| struct_or_union identifier
;

/*
(6.7.2.1) struct-or-union:
	struct
	union
*/
struct_or_union:
  keyword_struct
| keyword_union
;

/*
(6.7.2.1) struct-declaration-list:
	struct-declaration
	struct-declaration-list struct-declaration
*/
struct_declaration_list:
  struct_declaration
| struct_declaration_list struct_declaration
;

/*
(6.7.2.1) struct-declaration:
	specifier-qualifier-list struct-declarator-list ;
*/
struct_declaration:
  specifier_qualifier_list struct_declarator_list
;

/*
(6.7.2.1) specifier-qualifier-list:
	type-specifier specifier-qualifier-list_opt
	type-qualifier specifier-qualifier-list_opt
*/
specifier_qualifier_list:
  type_specifier specifier_qualifier_list_opt
| type_qualifier specifier_qualifier_list_opt
;

specifier_qualifier_list_opt:
  %empty
| specifier_qualifier_list
;

/*
(6.7.2.1) struct-declarator-list:
	struct-declarator
	struct-declarator-list , struct-declarator
*/
struct_declarator_list:
  struct_declarator
| struct_declarator_list punctuator_comma struct_declarator
;

/*
(6.7.2.1) struct-declarator:
	declarator
	declarator_opt : constant-expression
*/
struct_declarator:
  declarator
| declarator_opt ':' constant_expression
;

/*
(6.7.2.2) enum-specifier:
	enum identifier_opt { enumerator-list }
	enum identifier_opt { enumerator-list , }
	enum identifier
*/
enum_specifier:
  keyword_enum identifier_opt punctuator_left_brace enumerator_list punctuator_right_brace
| keyword_enum identifier_opt punctuator_left_brace enumerator_list punctuator_comma punctuator_right_brace
| keyword_enum identifier
;

/*
(6.7.2.2) enumerator-list:
	enumerator
	enumerator-list , enumerator
*/
enumerator_list:
  enumerator
| enumerator_list punctuator_comma enumerator
;

/*
(6.7.2.2) enumerator:
	enumeration-constant
	enumeration-constant = constant-expression
*/
/* Enumeration constant is essentially an identifier. They will come to the parser as identifiers, otherwise we can't distinguish them at tokenizing stage. */
/*
enumerator:
  enumeration_constant
| enumeration_constant '=' constant_expression
;
*/
enumerator:
  identifier
| identifier '=' constant_expression
;

/*
(6.7.3) type-qualifier:
	const
	restrict
	volatile
*/
type_qualifier:
  keyword_const
| keyword_restrict
| keyword_volatile
;

/*
(6.7.4) function-specifier:
	inline
*/
function_specifier:
	keyword_inline
;

/*
(6.7.5) declarator:
	pointer_opt direct-declarator
*/
declarator:
  pointer_opt direct_declarator
;

declarator_opt:
  %empty
| declarator
;

/*
(6.7.5) direct-declarator:
	identifier
	( declarator )
	direct-declarator [ type-qualifier-list_opt assignment-expression_opt ]
	direct-declarator [ static type-qualifier-list_opt assignment-expression ]
	direct-declarator [ type-qualifier-list static assignment-expression ]
	direct-declarator [ type-qualifier-list_opt * ]
	direct-declarator ( parameter-type-list )
	direct-declarator ( identifier-list_opt )
*/
direct_declarator:
  identifier
| punctuator_left_parenthesis declarator punctuator_right_parenthesis
| direct_declarator punctuator_left_square_bracket type_qualifier_list_opt assignment_expression_opt punctuator_right_square_bracket
| direct_declarator punctuator_left_square_bracket "static" type_qualifier_list_opt assignment_expression punctuator_right_square_bracket
| direct_declarator punctuator_left_square_bracket type_qualifier_list "static" assignment_expression punctuator_right_square_bracket
| direct_declarator punctuator_left_square_bracket type_qualifier_list_opt '*' punctuator_right_square_bracket
| direct_declarator punctuator_left_parenthesis parameter_type_list punctuator_right_parenthesis
| direct_declarator punctuator_left_parenthesis identifier_list_opt punctuator_right_parenthesis
;

/*
(6.7.5) pointer:
	* type-qualifier-list_opt
	* type-qualifier-list_opt pointer
*/
pointer:
  punctuator_star type_qualifier_list_opt
| punctuator_star type_qualifier_list_opt pointer;
pointer_opt:
  %empty
| pointer
;

/*
(6.7.5) type-qualifier-list:
	type-qualifier
	type-qualifier-list type-qualifier
*/
type_qualifier_list:
  type_qualifier
| type_qualifier_list type_qualifier
;

type_qualifier_list_opt:
  %empty
| type_qualifier_list
;

/*
(6.7.5) parameter-type-list:
	parameter-list
	parameter-list , ...
*/
parameter_type_list:
  parameter_list
| parameter_list punctuator_comma punctuator_ellipsis
;

parameter_type_list_opt:
  %empty
| parameter_type_list
;

/*
(6.7.5) parameter-list:
	parameter-declaration
	parameter-list , parameter-declaration
*/
parameter_list:
  parameter_declaration
;

/*
(6.7.5) parameter-declaration:
	declaration-specifiers declarator
	declaration-specifiers abstract-declarator_opt
*/
parameter_declaration:
  declaration_specifiers declarator
| declaration_specifiers abstract_declarator_opt
;

/*
(6.7.5) identifier-list:
	identifier
	identifier-list , identifier
*/
identifier_list:
  identifier
| identifier_list punctuator_comma identifier
;

identifier_list_opt:
  %empty
| identifier_list
;

/*
(6.7.6) type-name:
	specifier-qualifier-list abstract-declarator_opt
*/
type_name:
  specifier_qualifier_list abstract_declarator_opt
;

/*
(6.7.6) abstract-declarator:
	pointer
	pointer_opt direct-abstract-declarator
*/
abstract_declarator:
  pointer
| pointer_opt direct_abstract_declarator
;

abstract_declarator_opt:
  %empty
| abstract_declarator
;

/*
(6.7.6) direct-abstract-declarator:
	( abstract-declarator )
	direct-abstract-declarator_opt [ type-qualifier-list_opt assignment-expression_opt ]
	direct-abstract-declarator_opt [ static type-qualifier-list_opt assignment-expression ]
	direct-abstract-declarator_opt [ type-qualifier-list static assignment-expression ]
	direct-abstract-declarator_opt [ * ]
	direct-abstract-declarator_opt ( parameter-type-list_opt )
*/
/* TODO: find an example of this! Are these some features of C I have never seen? */
direct_abstract_declarator:
  punctuator_left_parenthesis abstract_declarator punctuator_right_parenthesis
| direct_abstract_declarator_opt punctuator_left_square_bracket type_qualifier_list_opt
| direct_abstract_declarator_opt punctuator_left_square_bracket "static" type_qualifier_list_opt assignment_expression punctuator_right_square_bracket
| direct_abstract_declarator_opt punctuator_left_square_bracket type_qualifier_list "static" assignment_expression punctuator_right_square_bracket
| direct_abstract_declarator_opt punctuator_left_square_bracket '*' punctuator_right_square_bracket
| direct_abstract_declarator_opt punctuator_left_parenthesis parameter_type_list_opt punctuator_right_parenthesis
;

direct_abstract_declarator_opt:
  %empty
| direct_abstract_declarator
;

/*
(6.7.7) typedef-name:
	identifier
*/
typedef_name:
	identifier
;

/*
(6.7.8) initializer:
	assignment-expression
	{ initializer-list }
	{ initializer-list , }
*/
initializer:
  assignment_expression
| punctuator_left_brace initializer_list punctuator_right_brace
| punctuator_left_brace initializer_list punctuator_comma punctuator_right_brace
;

/*
(6.7.8) initializer-list:
	designation_opt initializer
	initializer-list , designation_opt initializer
*/
initializer_list:
  designation_opt initializer
| initializer_list punctuator_comma designation_opt initializer
;

/*
(6.7.8) designation:
	designator-list =
*/
designation:
  designator_list '='
;
designation_opt:
  %empty
| designation
;

/*
(6.7.8) designator-list:
	designator
	designator-list designator
*/
designator_list:
  designator
| designator_list designator
;

/*
(6.7.8) designator:
	[ constant-expression ]
	. identifier
*/
designator:
  punctuator_left_square_bracket constant_expression punctuator_right_square_bracket
| punctuator_dot identifier
;

/*
(6.8) statement:
	labeled-statement
	compound-statement
	expression-statement
	selection-statement
	iteration-statement
	jump-statement
*/
statement:
  labeled_statement
| compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

/*
(6.8.1) labeled-statement:
	identifier : statement
	case constant-expression : statement
	default : statement
*/
labeled_statement:
  identifier ':' statement
| keyword_case constant_expression ':' statement
| keyword_default ':' statement
;

/*
(6.8.2) compound-statement:
	{ block-item-list_opt }
*/
compound_statement:
  punctuator_left_brace block_item_list_opt punctuator_right_brace
;

/*
(6.8.2) block-item-list:
	block-item
	block-item-list block-item
*/
block_item_list:
  block_item
| block_item_list block_item
;

block_item_list_opt:
  %empty
| block_item_list
;

/*
(6.8.2) block-item:
	declaration
	statement
*/
block_item:
  declaration
| statement
;

/*
(6.8.3) expression-statement:
	expression_opt ;
*/
expression_statement:
  expression_opt ';'
;

/*
(6.8.4) selection-statement:
	if ( expression ) statement
	if ( expression ) statement else statement
	switch ( expression ) statement
*/
selection_statement:
  keyword_if punctuator_left_parenthesis expression punctuator_right_parenthesis statement
| keyword_if punctuator_left_parenthesis expression punctuator_right_parenthesis statement "else" statement
| keyword_switch punctuator_left_parenthesis expression punctuator_right_parenthesis statement
;

/*
(6.8.5) iteration-statement:
	while ( expression ) statement
	do statement while ( expression ) ;
	for ( expression_opt ; expression_opt ; expression_opt ) statement
	for ( declaration expression_opt ; expression_opt ) statement
*/
iteration_statement:
  keyword_while punctuator_left_parenthesis expression punctuator_right_parenthesis statement
| keyword_do statement keyword_while punctuator_left_parenthesis expression punctuator_right_parenthesis
| keyword_for punctuator_left_parenthesis expression_opt ';' expression_opt ';' expression_opt punctuator_right_parenthesis statement
| keyword_for punctuator_left_parenthesis declaration expression_opt ';' expression_opt punctuator_right_parenthesis statement
;

/*
(6.8.6) jump-statement:
	goto identifier ;
	continue ;
	break ;
	return expression_opt ;
*/
jump_statement:
	keyword_goto identifier ';'
	keyword_continue ';'
	keyword_break ';'
	keyword_return expression_opt ';'
;

/*
(6.9.1) declaration-list:
	declaration
	declaration-list declaration
*/
declaration_list:
  declaration
| declaration_list declaration
;

declaration_list_opt:
  %empty
| declaration_list
;

/*
(6.9.1) function-definition:
	declaration-specifiers declarator declaration-list_opt compound-statement
*/
function_definition:
  declaration_specifiers declarator declaration_list_opt compound_statement
;

/*
(6.9) external-declaration:
	function-definition
	declaration
*/
external_declaration:
  function_definition
| declaration
;

/*
(6.9) translation-unit:
	external-declaration
	translation-unit external-declaration
*/
translation_unit:
  external_declaration
| translation_unit external_declaration
;

%%

int token_process_keyword_auto()       { return keyword_auto;       }
int token_process_keyword_break()      { return keyword_break;      }
int token_process_keyword_case()       { return keyword_case;       }
int token_process_keyword_char()       { return keyword_char;       }
int token_process_keyword_const()      { return keyword_const;      }
int token_process_keyword_continue()   { return keyword_continue;   }
int token_process_keyword_default()    { return keyword_default;    }
int token_process_keyword_do()         { return keyword_do;         }
int token_process_keyword_double()     { return keyword_double;     }
int token_process_keyword_else()       { return keyword_else;       }
int token_process_keyword_enum()       { return keyword_enum;       }
int token_process_keyword_extern()     { return keyword_extern;     }
int token_process_keyword_float()      { return keyword_float;      }
int token_process_keyword_for()        { return keyword_for;        }
int token_process_keyword_goto()       { return keyword_goto;       }
int token_process_keyword_if()         { return keyword_if;         }
int token_process_keyword_inline()     { return keyword_inline;     }
int token_process_keyword_int()        { return keyword_int;        }
int token_process_keyword_long()       { return keyword_long;       }
int token_process_keyword_register()   { return keyword_register;   }
int token_process_keyword_restrict()   { return keyword_restrict;   }
int token_process_keyword_return()     { return keyword_return;     }
int token_process_keyword_short()      { return keyword_short;      }
int token_process_keyword_signed()     { return keyword_signed;     }
int token_process_keyword_sizeof()     { return keyword_sizeof;     }
int token_process_keyword_static()     { return keyword_static;     }
int token_process_keyword_struct()     { return keyword_struct;     }
int token_process_keyword_switch()     { return keyword_switch;     }
int token_process_keyword_typedef()    { return keyword_typedef;    }
int token_process_keyword_union()      { return keyword_union;      }
int token_process_keyword_unsigned()   { return keyword_unsigned;   }
int token_process_keyword_void()       { return keyword_void;       }
int token_process_keyword_volatile()   { return keyword_volatile;   }
int token_process_keyword_while()      { return keyword_while;      }
int token_process_keyword__Bool()      { return keyword__Bool;      }
int token_process_keyword__Complex()   { return keyword__Complex;   }
int token_process_keyword__Imaginary() { return keyword__Imaginary; }

int token_process_identifier(char const* text) {
	(void) text;
	
	return identifier;
}

int token_process_integer_constant(enum token_integer_constant_base base, char const* text) {
	(void) base;
	(void) text;
	
	return constant;
}

int token_process_floating_constant(enum token_floating_constant_base base, char const* text) {
	(void) base;
	(void) text;
	
	return constant;
}

int token_process_character_constant(char const* text) {
	(void) text;
	
	return constant;
}

int token_process_string_literal(char const* text) {
	(void) text;
	
	return string_literal;
}

int token_process_punctuator_left_square_bracket()         { return punctuator_left_square_bracket;         }
int token_process_punctuator_right_square_bracket()        { return punctuator_right_square_bracket;        }
int token_process_punctuator_left_parenthesis()            { return punctuator_left_parenthesis;            }
int token_process_punctuator_right_parenthesis()           { return punctuator_right_parenthesis;           }
int token_process_punctuator_left_brace()                  { return punctuator_left_brace;                  }
int token_process_punctuator_right_brace()                 { return punctuator_right_brace;                 }
int token_process_punctuator_dot()                         { return punctuator_dot;                         }
int token_process_punctuator_arrow()                       { return punctuator_arrow;                       }
int token_process_punctuator_increment()                   { return punctuator_increment;                   }
int token_process_punctuator_decrement()                   { return punctuator_decrement;                   }
int token_process_punctuator_and()                         { return punctuator_and;                         }
int token_process_punctuator_star()                        { return punctuator_star;                        }
int token_process_punctuator_plus()                        { return punctuator_plus;                        }
int token_process_punctuator_minus()                       { return punctuator_minus;                       }
int token_process_punctuator_tilde()                       { return punctuator_tilde;                       }
int token_process_punctuator_exclamation_mark()            { return punctuator_exclamation_mark;            }
int token_process_punctuator_slash()                       { return punctuator_slash;                       }
int token_process_punctuator_percent()                     { return punctuator_percent;                     }
int token_process_punctuator_shift_left()                  { return punctuator_shift_left;                  }
int token_process_punctuator_shift_right()                 { return punctuator_shift_right;                 }
int token_process_punctuator_less()                        { return punctuator_less;                        }
int token_process_punctuator_greater()                     { return punctuator_greater;                     }
int token_process_punctuator_less_equal()                  { return punctuator_less_equal;                  }
int token_process_punctuator_greater_equal()               { return punctuator_greater_equal;               }
int token_process_punctuator_equal_equal()                 { return punctuator_equal_equal;                 }
int token_process_punctuator_not_equal()                   { return punctuator_not_equal;                   }
int token_process_punctuator_circumflex()                  { return punctuator_circumflex;                  }
int token_process_punctuator_or()                          { return punctuator_or;                          }
int token_process_punctuator_and_and()                     { return punctuator_and_and;                     }
int token_process_punctuator_or_or()                       { return punctuator_or_or;                       }
int token_process_punctuator_question_mark()               { return punctuator_question_mark;               }
int token_process_punctuator_colon()                       { return punctuator_colon;                       }
int token_process_punctuator_semicolon()                   { return punctuator_semicolon;                   }
int token_process_punctuator_ellipsis()                    { return punctuator_ellipsis;                    }
int token_process_punctuator_equal()                       { return punctuator_equal;                       }
int token_process_punctuator_star_equal()                  { return punctuator_star_equal;                  }
int token_process_punctuator_slash_equal()                 { return punctuator_slash_equal;                 }
int token_process_punctuator_percent_equal()               { return punctuator_percent_equal;               }
int token_process_punctuator_plus_equal()                  { return punctuator_plus_equal;                  }
int token_process_punctuator_minus_equal()                 { return punctuator_minus_equal;                 }
int token_process_punctuator_shift_left_equal()            { return punctuator_shift_left_equal;            }
int token_process_punctuator_shift_right_equal()           { return punctuator_shift_right_equal;           }
int token_process_punctuator_and_equal()                   { return punctuator_and_equal;                   }
int token_process_punctuator_circumflex_equal()            { return punctuator_circumflex_equal;            }
int token_process_punctuator_or_equal()                    { return punctuator_or_equal;                    }
int token_process_punctuator_comma()                       { return punctuator_comma;                       }
int token_process_punctuator_hash()                        { return punctuator_hash;                        }
int token_process_punctuator_hash_hash()                   { return punctuator_hash_hash;                   }
int token_process_punctuator_less_colon()                  { return punctuator_less_colon;                  }
int token_process_punctuator_colon_greater()               { return punctuator_colon_greater;               }
int token_process_punctuator_less_percent()                { return punctuator_less_percent;                }
int token_process_punctuator_percent_greater()             { return punctuator_percent_greater;             }
int token_process_punctuator_percent_colon()               { return punctuator_percent_colon;               }
int token_process_punctuator_percent_colon_percent_colon() { return punctuator_percent_colon_percent_colon; }

/*
int token_process_punctuator(char const* text) {
	(void) text;
	
	return punctuator;
}
*/

int token_process_invalid_token(char const* text) {
	(void) text;
	
	return -1;
}
