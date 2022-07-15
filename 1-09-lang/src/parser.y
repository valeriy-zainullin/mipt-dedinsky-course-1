/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 421, annex A: language syntax summary, part 2: phrase structure grammar, C99. */

// TODO: read about bison algorithm.

%start translation_unit

%type translation_unit

%token keyword
%token identifier

%token decimal_constant
%token octal_constant
%token hexadecimal_constant

%token decimal_floating_constant
%token hexadecimal_floating_constant

%token character_constant
%token string_literal

%token punctuator

%%

identifier_opt:
  %empty
| identifier
;

/* From tokenizer grammar. Could've returned struct token_constant from there, but decided to not change tokenizer. */
/*
constant:
	integer-constant
	floating-constant
	enumeration-constant
	character-constant
*/
/* Enum constant is essentially an identifier. We won't have them as a separate entity, otherwise we won't be able to distinguish them from identifiers at tokenizing stage. */
/* Enum members will come as identifiers to the parser. */
constant:
/* integer-constant */
  decimal_constant
| octal_constant
| hexadecimal_constant
/* floating-constant */
| decimal_floating_constant
| hexadecimal_floating_constant
/* enumeration-constant comes as an identifier, primary-expression has identifier subrule. */
/* character-constant */
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
| constant
| string_literal
| '(' expression ')'
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
| postfix_expression '[' expression ']'
| postfix_expression '(' argument_expression_list_opt ')'
| postfix_expression '.' identifier
| postfix_expression "->" identifier
| postfix_expression "++"
| postfix_expression "--"
| '(' type_name ')' '{' initializer_list '}'
| '(' type_name ')' '{' initializer_list ',' '}'
;

/*
(6.5.2) argument-expression-list:
	assignment-expression
	argument-expression-list , assignment-expression
*/
argument_expression_list:
  assignment_expression
| argument_expression_list ',' assignment_expression
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
| "++" unary_expression
| "--" unary_expression
| unary_operator cast_expression
| "sizeof" unary_expression
| "sizeof" '(' type_name ')'
;

/*
(6.5.3) unary-operator: one of
	& * + - ~ !
*/
unary_operator:
  '&' | '*' | '+' | '-' | '~' | '!';

/*
(6.5.4) cast-expression:
	unary-expression
	( type-name ) cast-expression
*/
cast_expression:
  unary_expression
| '(' type_name ')' cast_expression
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
| shift_expression "<<" additive_expression
| shift_expression ">>" additive_expression
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
| relational_expression '<' shift_expression
| relational_expression '>' shift_expression
| relational_expression "<=" shift_expression
| relational_expression ">=" shift_expression
;

/*
(6.5.9) equality-expression:
	relational-expression
	equality-expression == relational-expression
	equality-expression != relational-expression
*/
equality_expression:
  relational_expression
| equality_expression "==" relational_expression
| equality_expression "!=" relational_expression
;

/*
(6.5.10) AND-expression:
	equality-expression
	AND-expression & equality-expression
*/
AND_expression:
  equality_expression
| AND_expression '&' equality_expression
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
| logical_AND_expression "&&" inclusive_OR_expression
;

/*
(6.5.14) logical-OR-expression:
	logical-AND-expression
	logical-OR-expression || logical-AND-expression
*/
logical_OR_expression:
  logical_AND_expression
| logical_OR_expression "||" logical_AND_expression
;

/*
(6.5.15) conditional-expression:
	logical-OR-expression
	logical-OR-expression ? expression : conditional-expression
*/
conditional_expression:
  logical_OR_expression
| logical_OR_expression '?' expression ':' conditional_expression
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
  '='
| "*="
| "/="
| "%="
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
| expression ',' assignment_expression
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
| init_declarator_list ',' init_declarator
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
  "typedef"
| "extern"
| "static"
| "auto"
| "register"
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
  "void"
| "char"
| "short"
| "int"
| "long"
| "float"
| "double"
| "signed"
| "unsigned"
| "_Bool"
| "_Complex"
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
  struct_or_union identifier_opt '{' struct_declaration_list '}'
| struct_or_union identifier
;

/*
(6.7.2.1) struct-or-union:
	struct
	union
*/
struct_or_union:
  "struct"
| "union"
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
| struct_declarator_list ',' struct_declarator
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
  "enum" identifier_opt '{' enumerator_list '}'
| "enum" identifier_opt '{' enumerator_list ',' '}'
| "enum" identifier
;

/*
(6.7.2.2) enumerator-list:
	enumerator
	enumerator-list , enumerator
*/
enumerator_list:
  enumerator
| enumerator_list ',' enumerator
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
  "const"
| "restrict"
| "volatile"
;

/*
(6.7.4) function-specifier:
	inline
*/
function_specifier:
	"inline"
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
| '(' declarator ')'
| direct_declarator '[' type_qualifier_list_opt assignment_expression_opt ']'
| direct_declarator '[' "static" type_qualifier_list_opt assignment_expression ']'
| direct_declarator '[' type_qualifier_list "static" assignment_expression ']'
| direct_declarator '[' type_qualifier_list_opt '*' ']'
| direct_declarator '(' parameter_type_list ')'
| direct_declarator '(' identifier_list_opt ')'
;

/*
(6.7.5) pointer:
	* type-qualifier-list_opt
	* type-qualifier-list_opt pointer
*/
pointer:
  '*' type_qualifier_list_opt
| '*' type_qualifier_list_opt pointer;
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
| parameter_list ',' "..."
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
| identifier_list ',' identifier
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
  '(' abstract_declarator ')'
| direct_abstract_declarator_opt '[' type_qualifier_list_opt
| direct_abstract_declarator_opt '[' "static" type_qualifier_list_opt assignment_expression ']'
| direct_abstract_declarator_opt '[' type_qualifier_list "static" assignment_expression ']'
| direct_abstract_declarator_opt '[' '*' ']'
| direct_abstract_declarator_opt '(' parameter_type_list_opt ')'
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
| '{' initializer_list '}'
| '{' initializer_list ',' '}'
;

/*
(6.7.8) initializer-list:
	designation_opt initializer
	initializer-list , designation_opt initializer
*/
initializer_list:
  designation_opt initializer
| initializer_list ',' designation_opt initializer
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
  '[' constant_expression ']'
| '.' identifier
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
| "case" constant_expression ':' statement
| "default" ':' statement
;

/*
(6.8.2) compound-statement:
	{ block-item-list_opt }
*/
compound_statement:
  '{' block_item_list_opt '}'
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
  "if" '(' expression ')' statement
| "if" '(' expression ')' statement "else" statement
| "switch" '(' expression ')' statement
;

/*
(6.8.5) iteration-statement:
	while ( expression ) statement
	do statement while ( expression ) ;
	for ( expression_opt ; expression_opt ; expression_opt ) statement
	for ( declaration expression_opt ; expression_opt ) statement
*/
iteration_statement:
  "while" '(' expression ')' statement
| "do" statement "while" '(' expression ')'
| "for" '(' expression_opt ';' expression_opt ';' expression_opt ')' statement
| "for" '(' declaration expression_opt ';' expression_opt ')' statement
;

/*
(6.8.6) jump-statement:
	goto identifier ;
	continue ;
	break ;
	return expression_opt ;
*/
jump_statement:
	"goto" identifier ';'
	"continue" ';'
	"break" ';'
	"return" expression_opt ';'
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