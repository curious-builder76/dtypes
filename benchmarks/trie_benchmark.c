
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "dtypes/trie.h"

#include "print.h"
#include "test_alloc.h"

#define debug(...)do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)


#define INSERT(trie,what)do{\
	debug("INSERT: %s",what);\
	if(trie_insert(trie,what)!=0){\
		perror("");\
	};\
}while(0)

#define CONTAINS(trie,what)do{\
	debug("CONTAINS: %s  -->  %s",what, trie_contains(trie,what) ? "true":"false");\
}while(0)


void* get_data(size_t times, size_t input_size){
	return malloc(times*input_size);
}

char* prepare_data0(char* input,size_t input_size,size_t times,int random_flag){
	for(size_t idx=0;idx<times;idx++){
		char* ptr=input + idx*input_size;
		memset(ptr,0, input_size);
		int val=idx;
		if(random_flag){
			val=random() % times;
		}
		sprintf(ptr, "value_%d",val);
	}
	return input;
}

char* prepare_random(char* input, size_t input_size, size_t times){
	return prepare_data0(input, input_size, times, 1);
}

char* prepare_sequential(char* input, size_t input_size, size_t times){
	return prepare_data0(input, input_size, times, 0);
}

trie_t* insert(char* input, size_t input_size, size_t times, double* time_elapsed_out){
	int ret_code=0;
	trie_t* trie;
	do{
		trie=trie_custom(xmalloc,xrealloc,xfree);
	}
	while(trie==NULL);
	clock_t start=clock();
	for(size_t index=0;index<times;index++){
		char* ptr=input+input_size*index;
		if(trie_insert(trie,ptr)!=0){
			debug("Failed after %zu inserts.",index);
			ret_code=1;
			break;
		}
	}
	clock_t end=clock();
	if(ret_code){
		trie_destroy(trie);
		return NULL;
	}
	double time_elapsed= (double)(end-start)/CLOCKS_PER_SEC;

	*time_elapsed_out=time_elapsed;
	return trie;
}

int random_lookups(trie_t* trie,char* input, size_t input_size, size_t times, double* time_elapsed){
	if(!trie){
		debug("NULL trie found");
		return 1;
	}
	clock_t start=clock();
	for(size_t i=0;i<times;i++){
		char* ptr=input + input_size*i;
		if(!trie_contains(trie,ptr)){
			debug("Failed after %zu operations",i);
			return 1;
		}
	}
	clock_t end=clock();
	*time_elapsed= (end-start)/CLOCKS_PER_SEC;
	return 0;
}

void benchmark(){
	debug("==== benchmark of trie.====");
	trie_t* trie;
	do{
		trie=trie_custom(
			xmalloc,
			xrealloc,
			xfree
			);
	}
	while(!trie);
	INSERT(trie,"apple");
	INSERT(trie,"aplication");
	INSERT(trie,"armour");
	INSERT(trie,"banana");
	CONTAINS(trie,"apple");
	debug("Nonexistent entry test");
	CONTAINS(trie,"panda");
	debug("Existent entry test.");
	CONTAINS(trie,"apple");
	trie_destroy(trie);
	trie=NULL;

	double time_elapsed;
	int tests_failed=0;
	int times=50000000;

	// int times = 5000000;
	char* input=get_data(times, 32);
	if(!input){
		printf("Failed to prepare data!\n");
		return ;
	}

	debug("Random inserts.");
	prepare_random(input, 32, times);
	trie=insert(input, 32, times, &time_elapsed );
	if(!trie){
		debug("Test failed.");
		tests_failed++;
	}else{
		debug("Test passed");
		print_benchmark("INSERT (random)",times,time_elapsed);

	}

	trie_destroy(trie);
	trie=NULL;

	debug("Sequential inserts.");
	prepare_sequential(input, 32, times);
	trie=insert(input, 32, times, &time_elapsed );
	if(!trie){
		debug("Test failed.");
		tests_failed++;
	}else{
		debug("Test passed");
		print_benchmark("INSERT (sequential)",times,time_elapsed);

	}
	debug("Random lookups.");
	prepare_random(input, 32, times); 
	if(random_lookups(trie,input, 32, times,&time_elapsed)!=0){
		debug("Test failed.");
		tests_failed++;
	}else{
		debug("Test passed.");
		print_benchmark("CONTAINS (random)",times,time_elapsed);
	}
	if(trie){
		trie_destroy(trie);
	}
	if(!tests_failed){
		debug("All tests passed.");
	}else{
		debug("%d tests failed out of 3.",tests_failed);
	}
	free(input);
	input=NULL;
	memory_leak();
}
int main(){
	benchmark();
	return 0;
}
