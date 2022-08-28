#include "mycc-ast/statement.h"

#include <assert.h>
#include <stdio.h>

int yyparse(struct ast_return_stmt** return_stmt/* struct ast_translation_unit** translation_unit*/);

extern int yydebug;

int main() {
	#if defined(YYDEBUG)
		yydebug = 1;
	#endif
	
	// struct ast_translation_unit* translation_unit = NULL;
	struct ast_return_stmt* return_stmt = NULL;

	int result = yyparse(&return_stmt);
	
	if (result != 0) {
		// TODO: maybe show warnings, errors and etc.
		printf("Input is incorrect. Error recovery is not possible.\n");
		return 1;
	}
	
	assert(return_stmt != NULL);
	ast_return_stmt_print(stdout, return_stmt, 0);
	
	return 0;
}
