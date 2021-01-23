#include "stack.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ARRAY_SIZE 20
#define MAX_NAME_LENGTH 1025

int push(stack* s, char* name){
	if(s->size <= ARRAY_SIZE-1){
		if(s->size == 0){
			// allocate space for each string in array
			s->shared[0]=malloc(MAX_NAME_LENGTH);
			strcpy(s->shared[0], name);
			// s->shared[0] = *name;
			/* increment */
			s->size = 1;
			s->front = s->end = 0;
		}
		else{
			s->end++;
			s->shared[s->end]=malloc(MAX_NAME_LENGTH);
			strcpy(s->shared[s->end], name);
			//s->end++;
			s->size++;
		}
		return 0;
	}
	else{ /*full*/
		return -1;
	}
}

char* pop(stack* s){
	if(s->size != 0){
		//char* temp = s->shared[s->end+1];
		//s->shared[s->end+1] = NULL;
		/*pop off the end of stack */
		s->end--;
		s->size--;
		return s->shared[s->end+1]; 
	}
	else{ /*empty*/
		return NULL;
	}


}

