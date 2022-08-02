#include "mycc-ast/translation_unit.h"

#include <assert.h>

int yyparse();

extern int yydebug;

int main() {
	#if defined(YYDEBUG)
		yydebug = 1;
	#endif
	
	struct ast_translation_unit_node* return_stmt = NULL;

	int result = yyparse(&return_stmt);
	
	if (result != 0) {
		// TODO: maybe show warnings, errors and etc.
		printf("Input is incorrect. Error recovery is not possible.\n");
	}
	
	assert(return_stmt != NULL);
	ast_print_return_stmt(stdout, return_stmt, 0);
	
	return 0;
}
