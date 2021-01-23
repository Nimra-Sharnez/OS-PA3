#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "multi-lookup.h"
#include "util.h"
#define MAX_NAME_LENGTH 1025
#define MAX_RESOLVER_THREADS 5
#define MAX_REQUESTER_THREADS 10
#define MAX_INPUT_FILES 10
#define ARRAY_SIZE 20
#define MAX_IP_LENGTH INET_ADDRSTRLEN

void* requester(thread_info* info){
	int count=0;
	size_t line_size = MAX_NAME_LENGTH;
	char* hostname = malloc(line_size);
	pthread_mutex_lock(&info->request_lock);
	while(info->f < info->n_files){
		FILE* file = fopen(info->files[info->f], "r");
		info->f++;
		pthread_mutex_unlock(&info->request_lock);
		if(file == NULL){
			fprintf(stderr, "Bogus input filepath\n");
			/*before continuing it must grab a lock */
			/* and increment the serviced */
			pthread_mutex_lock(&info->big_lock);
			info->serviced_count++;
			pthread_mutex_unlock(&info->big_lock);
			pthread_mutex_lock(&info->request_lock);
			continue;
		}
		while(getline(&hostname, &line_size, file)!=-1){ 
			hostname[strlen(hostname)-1] = '\0';
			/* Circular Wait */
			while(1){
				pthread_mutex_lock(&info->stack_lock);
				if(push(&info->s, hostname)==0){
					hostname[strlen(hostname)-1] = '\n';
					pthread_mutex_unlock(&info->stack_lock);
					break;
				}
				else{
					pthread_mutex_unlock(&info->stack_lock);
				}		
			}
		}
		fclose(file);
		count++;
		pthread_mutex_lock(&info->big_lock);
		info->serviced_count++;
		pthread_mutex_unlock(&info->big_lock);

		pthread_mutex_lock(&info->request_lock);
	}
	/* terminate */
	fprintf(info->fp_s, "Thread %lu serviced %d files\n", pthread_self(), count);
	pthread_mutex_unlock(&info->request_lock);
	free(hostname);
	return 0;
	//pthread_exit(NULL);
}


void* resolver(thread_info* info){
	char* name;		
	char ip[MAX_IP_LENGTH];
	while(1){
		pthread_mutex_lock(&info->stack_lock);
		name = pop(&info->s);
		if(name != NULL){
			if(dnslookup(name, ip, sizeof(ip))){
				fprintf(stderr, "Bogus Hostname: %s\n", name);
				strcpy(ip, "");
			}
			fprintf(info->fp_r, "%s,%s \n", name, ip);
			free(name);
			pthread_mutex_unlock(&info->stack_lock);
		}
		else{
			pthread_mutex_unlock(&info->stack_lock);
			pthread_mutex_lock(&info->big_lock);
			if(info->serviced_count == info->n_files){
				pthread_mutex_unlock(&info->big_lock);
				break;
			}
			pthread_mutex_unlock(&info->big_lock);
		}

	}
	//pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[]){
	struct timeval start, end;
	gettimeofday(&start, NULL);

	if (argc<5) { return -1;}
	thread_info info;
	//stack s;
	/* info specifications */
	//info.s = s;
	info.serviced_count = 0;
	if (atoi(argv[1]) <= MAX_REQUESTER_THREADS){
		info.requester_threads = atoi(argv[1]);
	} else { return -1; }

	if (atoi(argv[2]) <= MAX_RESOLVER_THREADS){
		info.resolver_threads = atoi(argv[2]);
	} else { return -1; }
		info.serviced = argv[3];
	info.fp_s = fopen(info.serviced, "w");
	info.results = argv[4];
	info.fp_r = fopen(info.results, "w");
	/*check to see if file path is bogus or not */
	if(info.fp_r == NULL){
		fprintf(stderr, "Bogus Output File Path\n");
		exit(0);
	}

	for (int i=5; i<argc && i-5 < MAX_INPUT_FILES; i++){

		info.files[i-5] = argv[i];
		//printf("%s\n", info.files[i]);
		
	}

		/* starting file */
	info.f = 0;
	info.n_files = argc-5;
	info.line = 0;
	//because s is a pointer to stack -> I changed s to not be a pointer! (we'd use . now)
	//info.s = malloc(ARRAY_SIZE);
	info.s.size = 0;


	//mutex init locks
	pthread_mutex_init(&info.request_lock, NULL);
	pthread_mutex_init(&info.stack_lock, NULL);
	pthread_mutex_init(&info.big_lock, NULL);

	//create requester pool
	pthread_t requesters[MAX_REQUESTER_THREADS];
	for(int i=0; i< info.requester_threads; i++){
		pthread_create(&requesters[i], NULL, (void*) requester,(void*) &info);
	}
	//create resolver pool
	pthread_t resolvers[MAX_RESOLVER_THREADS];
	for (int i=0; i<info.resolver_threads; i++){
		pthread_create(&resolvers[i], NULL, (void*) resolver,(void*) &info);
	}
	//wait for requesters	
	for (int j=0; j< info.requester_threads; j++){
		pthread_join(requesters[j], NULL);
	}
	//wait for resolvers
	for (int j=0; j<info.resolver_threads; j++){
		pthread_join(resolvers[j], NULL);

	}
	gettimeofday(&end, NULL);
	printf("./multi-lookup: total time is %f seconds\n", (double)(end.tv_sec+((double)end.tv_usec/1000000)-start.tv_sec+((double)end.tv_usec/1000000)));

	//close files
	fclose(info.fp_s);
	fclose(info.fp_r);

	//mutex destroy locks
	pthread_mutex_destroy(&info.request_lock);
	pthread_mutex_destroy(&info.stack_lock);
	pthread_mutex_destroy(&info.big_lock);


}

