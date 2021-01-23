#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#define MAX_NAME_LENGTH 1025
#define MAX_RESOLVER_THREADS 5
#define MAX_REQUESTER_THREADS 10
#define MAX_INPUT_FILES 10
#define ARRAY_SIZE 20
#define MAX_IP_LENGTH INET_ADDRSTRLEN

typedef struct thread_info
{
	pthread_mutex_t stack_lock;
	pthread_mutex_t request_lock;
	pthread_mutex_t big_lock;
	int serviced_count;
	int requester_threads;
	int resolver_threads;
	char* serviced;
	char* results;
	char* files[MAX_INPUT_FILES];
	int f;
	int n_files;
	int line;
	stack s;
	FILE* fp_s;
	FILE* fp_r;
}thread_info;
