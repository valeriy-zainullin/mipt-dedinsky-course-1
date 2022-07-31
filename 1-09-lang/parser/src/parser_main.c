#include "mycc-ast/translation_unit.h"

#include <assert.h>

int yyparse();

extern int yydebug;

int main() {
	#if defined(YYDEBUG)
		yydebug = 1;
	#endif
	
	struct ast_translation_unit_node* translation_unit = NULL;

	int result = yyparse(&translation_unit);
	
	if (result != 0) {
		// TODO: maybe show warnings, errors and etc.
		printf("Input is incorrect. Error recovery is not possible.\n");
	}
	
	assert(translation_unit != NULL);
	ast_print_translation_unit_node(stdout, translation_unit, 0);
	
	return 0;
}
