/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 421, annex A: language syntax summary, part 2: phrase structure grammar, C99. */

// TODO: read about bison algorithm.

%define parse.error verbose

// TODO: figure out if this grammar is LR(1), maybe fix conflicts then, if LR(2), generating GLR parser is fine, no much overhead.
// https://stackoverflow.com/a/70601680
%glr-parser

%locations

%{
#include "tokenizer.h"

#include "mycc-ast/code_block.h"
#include "mycc-ast/constant.h"

#include "mycc-libs/vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

int yylex();
// void yyerror(struct ast_translation_unit_node** translation_unit, char const * string) {
//void yyerror(struct ast_code_block_node** code_block_node, char const * string) {
void yyerror(struct ast_return_stmt** return_stmt, char const * string) {
	// (void) translation_unit;
	//(void) code_block_node;
	(void) return_stmt;
	
	fprintf(stderr, "%s\n", string);
}
%}

// Use token table. Otherwise there are too many keywords and punctuators, code gets noticebly messier.
// We would have to provide separate functions for every keyword and punctuator, have many rules in the
// tokenizer, list every keyword and punctuator as separate rules in the rule section there. One repetition.
// Then function definitions in the standalone tokenizer, which just prints the tokens out to stdout. Not
// needed to figure out what particular keyword it is, also a repetition.
// And we would have to have them here, a lot more functions. Could make a macro, but token table and having
// general functions for keywords and punctuators are just better approach. No repetitons, only listed in
// table once and as tokens here, which we would have to do anyways if we would make separate functions for
// every keyword. We would have to declare those functions for handling keywords and punctuators in the
// "tokenizer.h", but then it's just too much work for every keyword and punctuator to have a function, they
// possibly declare it. It provokes to make a macro etc.
// %token-table
// Two links about token tables. yytoknum got removed, so yytname is not helpful. Define token list ourselves.
// 1. https://stackoverflow.com/a/43107075
// 2. https://dev.gnupg.org/T5616


/* TODO: keyword syntax here. */
%token keyword_auto keyword_break keyword_case keyword_char keyword_const keyword_continue keyword_default
%token keyword_do keyword_double keyword_else keyword_enum keyword_extern keyword_float keyword_for
%token keyword_goto keyword_if keyword_inline keyword_int keyword_long keyword_register keyword_restrict
%token keyword_return keyword_short keyword_signed keyword_sizeof keyword_static keyword_struct
%token keyword_switch keyword_typedef keyword_union keyword_unsigned keyword_void keyword_volatile
%token keyword_while keyword__Bool keyword__Complex keyword__Imaginary

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
%union {
	// TODO: delete "_node" from types.
	// TODO: implement reference counting. ast_acquire_ref, ast_release_ref, ast_get_num_refs
	// TODO: should be the following:
	//struct ast_translation_unit* translation_unit;
	//struct ast_external_decl*    external_decl;
	//struct ast_code_block*       code_block;
	// also align all other members here after these changes.
	struct ast_translation_unit_node* translation_unit;
	struct ast_external_decl_node*    external_decl;
	struct ast_code_block_node*       code_block;
	
	//struct ast_jump_stmt*             jump_stmt;
	struct ast_return_stmt*           jump_stmt;

	struct ast_bit_shift_expr*        bit_shift_expr;
	struct ast_add_expr*              add_expr;
	struct ast_mult_expr*             mult_expr;
	struct ast_primary_expr*          primary_expr;
	struct ast_constant*              constant;
	struct ast_int_constant*          int_constant;
}

%token identifier
%token<int_constant> integer_constant
%token floating_constant
%token character_constant
%token string_literal

// %start translation_unit
// %start block_item_list
%start jump_statement

// %parse-param { struct ast_translation_unit_node** output }
//%parse-param { struct ast_code_block_node** output }
%parse-param { struct ast_return_stmt** output }

%type<translation_unit> translation_unit
%type<external_decl> external_declaration
%type<code_block> block_item_list

%type<jump_stmt> statement
// labeled_statement
// compound_statement
// expression_statement
// selection_statement
// iteration_statement
%type<jump_stmt> jump_statement

%type<bit_shift_expr> expression
%type<bit_shift_expr> assignment_expression
%type<bit_shift_expr> conditional_expression
%type<bit_shift_expr> logical_OR_expression
%type<bit_shift_expr> logical_AND_expression
%type<bit_shift_expr> inclusive_OR_expression
%type<bit_shift_expr> exclusive_OR_expression
%type<bit_shift_expr> AND_expression
%type<bit_shift_expr> equality_expression
%type<bit_shift_expr> relational_expression
%type<bit_shift_expr> shift_expression
%type<add_expr>       additive_expression
%type<mult_expr>      multiplicative_expression
%type<primary_expr>   cast_expression
%type<primary_expr>   unary_expression
%type<primary_expr>   postfix_expression
%type<primary_expr>   primary_expression
%type<constant>       constant

%type<bit_shift_expr> expression_opt

//%destructor { *output = $$; } <translation_unit>
// %destructor { if ($$ != NULL) { $$ = ast_external_decl_node_delete($$); } } <external_decl>
/*%destructor {
	if ($$ != *output) {
		$$ = ast_code_block_delete($$);
	}
} <code_block>*/

// More about destructors: https://stackoverflow.com/a/6401973
// I think, I will be able to figure out what to do, when I will be dealing with compilation errors.
// It's not now. It's some time in the future.
// %destructor { if ($$ != NULL) $$ = ast_return_stmt_delete($$); /* ast_jump_statement_delete($$); */ } <jump_statement>

// %destructor { if ($$ != NULL) $$ = ast_constant_delete($$); } <constant>
// %destructor { if ($$ != NULL) $$ = ast_int_constant_delete($$); } <int_constant>

%%

identifier_opt:
  %empty
| identifier
;

constant:
  integer_constant {
      $$ = (struct ast_constant*) $1;
  }
| floating_constant
| character_constant
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
| constant {
      $$ = (struct ast_primary_expr*) $1;
  }
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
// Need to create a separate ast_init_list_expr, to not store initializer list information in
// postfix_expression structure, structure members get messy otherwise. Refactoring
// initializer lists into a separate rule so that syntax corresponds to ast type system.
postfix_expression:
  primary_expression { $$ = $1; }
| postfix_expression punctuator_left_square_bracket expression punctuator_right_square_bracket
| postfix_expression punctuator_left_parenthesis argument_expression_list_opt punctuator_right_parenthesis
| postfix_expression punctuator_dot identifier
| postfix_expression punctuator_arrow identifier
| postfix_expression punctuator_increment
| postfix_expression punctuator_decrement
| initializer_list_expr
;
initializer_list_expr:
  punctuator_left_parenthesis type_name punctuator_right_parenthesis punctuator_left_brace initializer_list punctuator_right_brace
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
  postfix_expression { $$ = $1; }
| punctuator_increment unary_expression
| punctuator_decrement unary_expression
| unary_operator cast_expression
| keyword_sizeof unary_expression

  // Not refactoring this to a separate sizeof_type_expression to correspond ast types,
  // because syntax is getting worse in my opinion, now sizeof in two places, but this
  // is handled so that ast_size_of_type_expr is created.
| keyword_sizeof punctuator_left_parenthesis type_name punctuator_right_parenthesis
;

/*
(6.5.3) unary-operator: one of
	& * + - ~ !
*/
unary_operator:
  punctuator_and | punctuator_star | punctuator_plus | punctuator_minus | punctuator_tilde | punctuator_exclamation_mark;

/*
(6.5.4) cast-expression:
	unary-expression
	( type-name ) cast-expression
*/
cast_expression:
  unary_expression { $$ = $1; }
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
  cast_expression {
      struct ast_mult_expr* mult_expr = ast_mult_expr_new();
      if (mult_expr == NULL) {
          assert(false);
          // TODO: handle this case.
      }
      printf("vector_push: 1.\n");
      if (!vector_push(mult_expr->primary_exprs, &$1)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = mult_expr;
  }
| multiplicative_expression punctuator_star cast_expression {
      struct ast_mult_expr* mult_expr = $1;
      printf("vector_push: 2.\n");
      if (!vector_push(mult_expr->primary_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 3.\n");
      enum ast_mult_expr_op op = AST_MULTIPLICATIVE_EXPR_OPERATION_MULTIPLICATION;
      if (!vector_push(mult_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = mult_expr;
  }
| multiplicative_expression punctuator_slash cast_expression {
      printf("vector_push: 4.\n");
      struct ast_mult_expr* mult_expr = $1;
      if (!vector_push(mult_expr->primary_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 5.\n");
      enum ast_mult_expr_op op = AST_MULTIPLICATIVE_EXPR_OPERATION_DIVISION;
      if (!vector_push(mult_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = mult_expr;
  }
| multiplicative_expression punctuator_percent cast_expression {
      printf("vector_push: 6.\n");
      struct ast_mult_expr* mult_expr = $1;
      if (!vector_push(mult_expr->primary_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 7.\n");
      enum ast_mult_expr_op op = AST_MULTIPLICATIVE_EXPR_OPERATION_MODULO;
      if (!vector_push(mult_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = mult_expr;
  }
;

/*
(6.5.6) additive-expression:
	multiplicative-expression
	additive-expression + multiplicative-expression
	additive-expression - multiplicative-expression
*/
additive_expression:
  multiplicative_expression {
      struct ast_add_expr* add_expr = ast_add_expr_new();
      if (add_expr == NULL) {
          assert(false);
          // TODO: handle this case.
      }
      printf("vector_push: 1.\n");
      if (!vector_push(add_expr->mult_exprs, &$1)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = add_expr;
  }
| additive_expression punctuator_plus multiplicative_expression {
      printf("vector_push: 4.\n");
      struct ast_add_expr* add_expr = $1;
      if (!vector_push(add_expr->mult_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 5.\n");
      enum ast_add_expr_op op = AST_ADDITIVE_EXPR_OPERATION_ADDITION;
      if (!vector_push(add_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = add_expr;
  }
| additive_expression punctuator_minus multiplicative_expression {
      printf("vector_push: 4.\n");
      struct ast_add_expr* add_expr = $1;
      if (!vector_push(add_expr->mult_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 5.\n");
      enum ast_add_expr_op op = AST_ADDITIVE_EXPR_OPERATION_SUBTRACTION;
      if (!vector_push(add_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = add_expr;
  }
;

/*
(6.5.7) shift-expression:
	additive-expression
	shift-expression << additive-expression
	shift-expression >> additive-expression
*/
shift_expression:
  additive_expression {
      struct ast_bit_shift_expr* bit_shift_expr = ast_bit_shift_expr_new();
      if (bit_shift_expr == NULL) {
          assert(false);
          // TODO: handle this case.
      }
      printf("vector_push: 1.\n");
      if (!vector_push(bit_shift_expr->add_exprs, &$1)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = bit_shift_expr;
  }
| shift_expression punctuator_shift_left additive_expression {
      printf("vector_push: 4.\n");
      struct ast_bit_shift_expr* bit_shift_expr = $1;
      if (!vector_push(bit_shift_expr->add_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 5.\n");
      enum ast_bit_shift_expr_op op = AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_LEFT;
      if (!vector_push(bit_shift_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = bit_shift_expr;
  }
| shift_expression punctuator_shift_right additive_expression {
      printf("vector_push: 4.\n");
      struct ast_bit_shift_expr* bit_shift_expr = $1;
      if (!vector_push(bit_shift_expr->add_exprs, &$3)) {
          assert(false);
          // TODO: handle this case.
      }
      
      printf("vector_push: 5.\n");
      enum ast_bit_shift_expr_op op = AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_RIGHT;
      if (!vector_push(bit_shift_expr->ops, &op)) {
          assert(false);
          // TODO: handle this case.
      }
      
      $$ = bit_shift_expr;
  }
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
  shift_expression { $$ = $1; }
| relational_expression punctuator_less shift_expression
| relational_expression punctuator_greater shift_expression
| relational_expression punctuator_less_equal shift_expression
| relational_expression punctuator_greater_equal shift_expression
;

/*
relational_expression:
  shift_expression:
    additive_expression:
  +
  shift_expression:
Количество пробелов -- длина макс оператора по длине + 1. indentation_level -> 
*/

/*
(6.5.9) equality-expression:
	relational-expression
	equality-expression == relational-expression
	equality-expression != relational-expression
*/
equality_expression:
  relational_expression { $$ = $1; }
| equality_expression punctuator_equal_equal relational_expression
| equality_expression punctuator_not_equal relational_expression
;

/*
(6.5.10) AND-expression:
	equality-expression
	AND-expression & equality-expression
*/
AND_expression:
  equality_expression { $$ = $1; }
| AND_expression punctuator_and equality_expression
;

/*
(6.5.11) exclusive-OR-expression:
	AND-expression
	exclusive-OR-expression ^ AND-expression
*/
exclusive_OR_expression:
  AND_expression { $$ = $1; }
| exclusive_OR_expression punctuator_circumflex AND_expression
;

/*
(6.5.12) inclusive-OR-expression:
	exclusive-OR-expression
	inclusive-OR-expression | exclusive-OR-expression
*/
inclusive_OR_expression:
  exclusive_OR_expression { $$ = $1; }
| inclusive_OR_expression punctuator_or exclusive_OR_expression
;

/*
(6.5.13) logical-AND-expression:
	inclusive-OR-expression
	logical-AND-expression && inclusive-OR-expression
*/
logical_AND_expression:
  inclusive_OR_expression { $$ = $1; }
| logical_AND_expression punctuator_and_and inclusive_OR_expression
;

/*
(6.5.14) logical-OR-expression:
	logical-AND-expression
	logical-OR-expression || logical-AND-expression
*/
logical_OR_expression:
  logical_AND_expression { $$ = $1; }
| logical_OR_expression punctuator_or_or logical_AND_expression
;

/*
(6.5.15) conditional-expression:
	logical-OR-expression
	logical-OR-expression ? expression : conditional-expression
*/
conditional_expression:
  logical_OR_expression { $$ = $1; }
| logical_OR_expression punctuator_question_mark expression punctuator_colon conditional_expression
;

/*
(6.5.16) assignment-expression:
	conditional-expression
	unary-expression assignment-operator assignment-expression
*/
assignment_expression:
  conditional_expression { $$ = $1; }
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
| punctuator_plus_equal
| punctuator_minus_equal
| punctuator_shift_left_equal
| punctuator_shift_right_equal
| punctuator_and_equal
| punctuator_circumflex_equal
| punctuator_or_equal
;

/*
(6.5.17) expression:
	assignment-expression
	expression , assignment-expression
*/
expression:
  assignment_expression { $$ = $1; }
| expression punctuator_comma assignment_expression
;

expression_opt:
  %empty {
      $$ = NULL;
  }
| expression {
      $$ = $1;
  }
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
  declaration_specifiers init_declarator_list_opt punctuator_semicolon
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
| type_specifier declaration_specifiers_opt {
      /* if ($2.present == NULL) {
          
      } */
  }
| type_qualifier declaration_specifiers_opt
| function_specifier declaration_specifiers_opt
;
/* TODO: storage class specifier can be typedef. I suppose it's invalid for function definitions. Check it on a different compiler, gcc maybe. If it is so, diallow typedef for functions in semantics. */
/* TODO: also not more than one same storage_class_specifier for a declaration. */

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
| declarator punctuator_equal initializer
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
	struct-or-union-specifier *
	enum-specifier
	typedef-name
*/
// Looks like this star is not needed. The parser can't read the following:
/*
struct simple_struct {
	int a;
	int b;
	int c;
};

int main() {
	struct simple_struct instance = {1, 2, 3};
	return instance.a + instance.b + instance.c;
}
*/
// Dropping it. Maybe it is removed when this standard is transitioned from draft to standard.
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
| struct_or_union_specifier
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
  specifier_qualifier_list struct_declarator_list punctuator_semicolon
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
| declarator_opt punctuator_colon constant_expression
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
| enumeration_constant punctuator_equal constant_expression
;
*/
enumerator:
  identifier
| identifier punctuator_equal constant_expression
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
| direct_declarator punctuator_left_square_bracket keyword_static type_qualifier_list_opt assignment_expression punctuator_right_square_bracket
| direct_declarator punctuator_left_square_bracket type_qualifier_list keyword_static assignment_expression punctuator_right_square_bracket
| direct_declarator punctuator_left_square_bracket type_qualifier_list_opt punctuator_star punctuator_right_square_bracket
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
| punctuator_star type_qualifier_list_opt pointer
;
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
| direct_abstract_declarator_opt punctuator_left_square_bracket keyword_static type_qualifier_list_opt assignment_expression punctuator_right_square_bracket
| direct_abstract_declarator_opt punctuator_left_square_bracket type_qualifier_list keyword_static assignment_expression punctuator_right_square_bracket
| direct_abstract_declarator_opt punctuator_left_square_bracket punctuator_star punctuator_right_square_bracket
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
  designator_list punctuator_equal
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
| jump_statement {
      $$ = $1;
      printf("jump_statement = %p.\n", jump_statement);
  }
;

/*
(6.8.1) labeled-statement:
	identifier : statement
	case constant-expression : statement
	default : statement
*/
/* TODO: 1st is ast_label_decl, 2nd is ast_case_decl, 3rd is ast_default_case_decl. */
labeled_statement:
  identifier punctuator_colon statement
| keyword_case constant_expression punctuator_colon statement
| keyword_default punctuator_colon statement
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
/* TODO: named Block in printed output. */
block_item_list:
  block_item {
  }
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
  expression_opt punctuator_semicolon
;

/*
(6.8.4) selection-statement:
	if ( expression ) statement
	if ( expression ) statement else statement
	switch ( expression ) statement
*/
selection_statement:
  keyword_if punctuator_left_parenthesis expression punctuator_right_parenthesis statement
| keyword_if punctuator_left_parenthesis expression punctuator_right_parenthesis statement keyword_else statement
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
| keyword_for punctuator_left_parenthesis expression_opt punctuator_semicolon expression_opt punctuator_semicolon expression_opt punctuator_right_parenthesis statement
| keyword_for punctuator_left_parenthesis declaration expression_opt punctuator_semicolon expression_opt punctuator_right_parenthesis statement
;

/*
(6.8.6) jump-statement:
	goto identifier ;
	continue ;
	break ;
	return expression_opt ;
*/
/* TODO: 1st is ast_goto_stmt, 2nd is continue_stmt, 3rd is break_stmt, 4th is return_stmt expression ptr*/
jump_statement:
  keyword_goto identifier punctuator_semicolon
| keyword_continue punctuator_semicolon
| keyword_break punctuator_semicolon
| keyword_return expression_opt punctuator_semicolon {
      $$ = ast_return_stmt_new($2);
      *output = $$;
      // TODO: handle out of memory errors.
  }
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
  declaration_specifiers declarator declaration_list_opt compound_statement {
      /* $$ = ast_function_def_node_new($1, $4);
      if ($$ == NULL) {
          YYNOMEM;
      }*/
  }
;

/*
(6.9) external-declaration:
	function-definition
	declaration
*/
external_declaration:
  function_definition { $$ = NULL; /* $$ = (struct ast_external_decl_node*) $1; */ }
| declaration         { $$ = NULL; /* (struct ast_external_decl_node*) $1; */ }
;

/*
(6.9) translation-unit:
	external-declaration
	translation-unit external-declaration
*/
translation_unit:
  external_declaration {
      // TODO: have a vector where memory is allocated and translation unit is just initialized there.
      // This way in case of an error memory could be freed.
      $$ = ast_translation_unit_node_new();
      if ($$ == NULL) {
          YYNOMEM;
      }
      if (!vector_push($$->external_decls, &$1)) {
          // FIXME: other objects on the stack are not deallocated!! Use bison %destructor keyword.
          // extern struct vector* ast_memory_pool;
          $$ = ast_translation_unit_node_delete($$);
          YYNOMEM;
      }
  }
| translation_unit external_declaration {
      // FIXME: other objects on the stack are not deallocated!! Have a memory pool as a vector structure to allocate memory at.
      // extern struct vector* ast_memory_pool;
      // FIXME: When this is done, no need to deallocate memory here.
      // TODO: Also, make a test case for this scenario and check memory is actually deallocated when memory pool will be implemented.
      if (!vector_push($1->external_decls, &$2)) {
          ast_translation_unit_node_delete($1);
          YYNOMEM;
      }
  }
;

%%

#include <stdbool.h>
#include <stddef.h>

#define VERIFY(EXPR) if (!(EXPR)) { printf("File %s, line %d: expr %s is false. Program was terminated.\n", __FILE__, __LINE__, #EXPR); abort(); }

struct token_table_entry {
	char const* token_text;
	int symbol_index;
};

#define TO_STRING2(value) #value
#define TO_STRING(value) TO_STRING2(value)

#define KEYWORD_ENTRY(name) { #name , keyword_ ## name }
#define PUNCTUATOR_ENTRY(text, name) { text, punctuator_ ## name }

static const struct token_table_entry token_table[] = {
	KEYWORD_ENTRY(auto),     KEYWORD_ENTRY(break),    KEYWORD_ENTRY(case),     KEYWORD_ENTRY(char),
	KEYWORD_ENTRY(const),    KEYWORD_ENTRY(continue), KEYWORD_ENTRY(default),  KEYWORD_ENTRY(do),
	KEYWORD_ENTRY(double),   KEYWORD_ENTRY(else),     KEYWORD_ENTRY(enum),     KEYWORD_ENTRY(extern),
	KEYWORD_ENTRY(float),    KEYWORD_ENTRY(for),      KEYWORD_ENTRY(goto),     KEYWORD_ENTRY(if),
	KEYWORD_ENTRY(inline),   KEYWORD_ENTRY(int),      KEYWORD_ENTRY(long),     KEYWORD_ENTRY(register),
	KEYWORD_ENTRY(restrict), KEYWORD_ENTRY(return),   KEYWORD_ENTRY(short),    KEYWORD_ENTRY(signed),
	KEYWORD_ENTRY(sizeof),   KEYWORD_ENTRY(static),   KEYWORD_ENTRY(struct),   KEYWORD_ENTRY(switch),
	KEYWORD_ENTRY(typedef),  KEYWORD_ENTRY(union),    KEYWORD_ENTRY(unsigned), KEYWORD_ENTRY(void),
	KEYWORD_ENTRY(volatile), KEYWORD_ENTRY(while),    KEYWORD_ENTRY(_Bool),    KEYWORD_ENTRY(_Complex),
	KEYWORD_ENTRY(_Imaginary),
	
	PUNCTUATOR_ENTRY(  "[", left_square_bracket),  PUNCTUATOR_ENTRY(   "]",        right_square_bracket),
	PUNCTUATOR_ENTRY(  "(",    left_parenthesis),  PUNCTUATOR_ENTRY(   ")",           right_parenthesis),
	PUNCTUATOR_ENTRY(  "{",          left_brace),  PUNCTUATOR_ENTRY(   "}",                 right_brace),
	PUNCTUATOR_ENTRY(  ".",                 dot),  PUNCTUATOR_ENTRY(  "->",                       arrow),
	PUNCTUATOR_ENTRY( "++",           increment),  PUNCTUATOR_ENTRY(  "--",                   decrement),
	PUNCTUATOR_ENTRY(  "&",                 and),  PUNCTUATOR_ENTRY(   "*",                        star),
	PUNCTUATOR_ENTRY(  "+",                plus),  PUNCTUATOR_ENTRY(   "-",                       minus),
	PUNCTUATOR_ENTRY(  "~",               tilde),  PUNCTUATOR_ENTRY(   "!",            exclamation_mark),
	PUNCTUATOR_ENTRY(  "/",               slash),  PUNCTUATOR_ENTRY(   "%",                     percent),
	PUNCTUATOR_ENTRY( "<<",          shift_left),  PUNCTUATOR_ENTRY(  ">>",                 shift_right),
	PUNCTUATOR_ENTRY(  "<",                less),  PUNCTUATOR_ENTRY(   ">",                     greater),
	PUNCTUATOR_ENTRY( "<=",          less_equal),  PUNCTUATOR_ENTRY(  ">=",               greater_equal),
	PUNCTUATOR_ENTRY( "==",         equal_equal),  PUNCTUATOR_ENTRY(  "!=",                   not_equal),
	PUNCTUATOR_ENTRY(  "^",          circumflex),  PUNCTUATOR_ENTRY(   "|",                          or),
	PUNCTUATOR_ENTRY( "&&",             and_and),  PUNCTUATOR_ENTRY(  "||",                       or_or),
	PUNCTUATOR_ENTRY(  "?",       question_mark),  PUNCTUATOR_ENTRY(   ":",                       colon),
	PUNCTUATOR_ENTRY(  ";",           semicolon),  PUNCTUATOR_ENTRY( "...",                    ellipsis),
	PUNCTUATOR_ENTRY(  "=",               equal),  PUNCTUATOR_ENTRY(  "*=",                  star_equal),
	PUNCTUATOR_ENTRY( "/=",         slash_equal),  PUNCTUATOR_ENTRY(  "%=",               percent_equal),
	PUNCTUATOR_ENTRY( "+=",          plus_equal),  PUNCTUATOR_ENTRY(  "-=",                 minus_equal),
	PUNCTUATOR_ENTRY("<<=",    shift_left_equal),  PUNCTUATOR_ENTRY( ">>=",           shift_right_equal),
	PUNCTUATOR_ENTRY( "&=",           and_equal),  PUNCTUATOR_ENTRY(  "^=",            circumflex_equal),
	PUNCTUATOR_ENTRY( "|=",            or_equal),  PUNCTUATOR_ENTRY(   ",",                       comma),
	PUNCTUATOR_ENTRY(  "#",                hash),  PUNCTUATOR_ENTRY(  "##",                   hash_hash),
	PUNCTUATOR_ENTRY( "<:",          less_colon),  PUNCTUATOR_ENTRY(  ":>",               colon_greater),
	PUNCTUATOR_ENTRY( "<%",        less_percent),  PUNCTUATOR_ENTRY(  "%>",             percent_greater),
	PUNCTUATOR_ENTRY( "%:",       percent_colon),  PUNCTUATOR_ENTRY("%:%:", percent_colon_percent_colon)
};

#define NUM_TOKENS sizeof(token_table) / sizeof(struct token_table_entry)

#undef KEYWORD_ENTRY
#undef PUNCTUATOR_ENTRY

static int find_literal_token(char const* text) {
	for (size_t i = 0; i < NUM_TOKENS; ++i) {
		if (strcmp(token_table[i].token_text, text) == 0) {
			return token_table[i].symbol_index;
		}
	}
	
	VERIFY(false);
	__builtin_unreachable();
}

int tokenizer_handle_keyword(char const* text) {
	return find_literal_token(text);
}

int tokenizer_handle_punctuator(char const* text) {
	return find_literal_token(text);
}

int tokenizer_handle_identifier(char const* text) {
	(void) text;
	
	return identifier;
}

int tokenizer_handle_integer_constant(char const* text) {
	yylval.int_constant = ast_int_constant_new_from_token(text);
	// TODO: check for null pointer, returned if an allocation fails.
	
	return integer_constant;
}

int tokenizer_handle_floating_constant(char const* text) {
	(void) text;
	
	return floating_constant;
}

int tokenizer_handle_character_constant(char const* text) {
	(void) text;
	
	return character_constant;
}

int tokenizer_handle_string_literal(char const* text) {
	(void) text;
	
	return string_literal;
}

int tokenizer_handle_invalid_token(char const* text) {
	(void) text;
	
	return -1;
}
