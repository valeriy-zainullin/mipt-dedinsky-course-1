#include <stddef.h>
#include <stdlib.h>

struct abc {
	int a;
	int b;
	int c;
}* print() {
	struct abc* abc = calloc(1, sizeof(struct abc*));
	if (abc == NULL) {
		return abc;
	}
	
	abc->a = 0;
	abc->b = 1;
	abc->c = 2;
	
	return abc;
}

