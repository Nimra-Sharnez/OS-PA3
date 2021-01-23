#include <stdio.h>
#define ARRAY_SIZE 20
typedef struct Stack{
	int front;
	int end;
	int size;
	int n_allocated;
	char* shared[ARRAY_SIZE];
}stack;

int push(stack* s, char* name);

char* pop(stack* s);
