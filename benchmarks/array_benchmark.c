#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include "dtypes/array.h"

#include "test_alloc.h"

#define log(...) do {\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)



typedef struct{
	int digit;
	char string[32];
}digit_t;

void benchmark(){
	log("==== benchmark of array====");
	uint32_t times=1000000;
	
	digit_t* digits=malloc(sizeof(digit_t)*times);
	if(digits==NULL){
		perror("malloc()");
		return;
	}

	log("Initializing...");
	for(uint32_t idx=0;idx<times;idx++){
		digit_t* digit=digits+idx;
		memset(digit,0,sizeof(digit_t));
		digit->digit=0;
		sprintf(digit->string,"id_%d",idx);
	}
	log("done.");

	clock_t start;
	clock_t end;
	double time_elapsed;
	int test_failed=0;
	int total_tests=2;
	int tests_failed=0;
	
	array_t* array=array_custom(
			sizeof(digit_t),
			xmalloc,
			xrealloc,
			xfree
			);
	if(array==NULL){
		perror("");
		return;
	}
	log("Array put test.");
	start=clock();
	for(uint32_t idx=0;idx<times;idx++){
		if(array_put(array,digits+idx)!=0){
			test_failed=1;
			tests_failed++;
			log("Failed after %d puts.",idx);
			break;
		}
	}

	free(digits);
	digits=NULL;
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
	if(!test_failed){
		log("Test passed.");
		log("array_put: %d operations in %.3f seconds.",times,time_elapsed);
	}
	
	log("Array iteration test.");

	uint32_t size_estimated=array_getsize(array);
	uint32_t size_actual=0;

	start=clock();
	while(array_at(array,size_actual)!=NULL){
		size_actual++;
	};
	end=clock();

	if(size_estimated!=size_actual){

		test_failed++;
		log("Test failed.");
	}else{
		time_elapsed= (double)(end-start)/CLOCKS_PER_SEC;
		log("Test passed.");
		log("array_at: %u operations in %.3f seconds.", size_estimated, time_elapsed);
	}

	array_destroy(array);

	if(!tests_failed){
		log("All tests passed.");
	}else

	log("%d tests failed of  %d tests", test_failed,total_tests);

	memory_leak();
}


int main(){
	benchmark();
	return 0;
}

