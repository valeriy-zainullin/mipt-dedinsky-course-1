#include <stdio.h>

int main() {
	int a = 0;
	scanf("%d", &a);
	switch (a) {
		case 3: printf("0\n"); break;
		case 8: printf("11\n"); break;
		case 9: printf("111\n"); break;
		case 10: printf("1111\n"); break;
		case 15: printf("111110\n"); break;
		default: printf("default\n"); break;
	}
	
	return 0;
}