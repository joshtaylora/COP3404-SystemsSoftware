#include <stdio.h>

int main() {
	int x = 10;
	int* xPointer = &x;

	printf("x's value: %d\nx's memory location: %p\n", x, xPointer);
	printf("Using x's pointer to get the value of x: %d\n", *xPointer);
}