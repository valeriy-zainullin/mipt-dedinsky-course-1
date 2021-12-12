#include "differentiation.h"
#include "parsing.h"

#include <stddef.h>
#include <stdlib.h>

int main() {
	Reader reader = {};
	if (!reader_init(&reader, stdin, true)) {
		fprintf(stderr, "Не удалось инициализировать читатель.\n");
		return 1;
	}
	
	printf("Введите выражение: ");
	fflush(stdout);
	
	Tree tree = {};
	
	if (!parsing_read_expression(reader, tree)) {
		fprintf(stderr, "Не удалось разобрать выражение.");
		return 2;
	}
	
	reader_deinit(&reader);
	
	return 0;
}
