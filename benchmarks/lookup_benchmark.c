#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>

#include "test_alloc.h"
#include "dtypes/lookup.h"


#define debug(...)do {\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)

typedef struct{
	uint32_t n1;
	uint32_t n2;
}point_t;


void benchmark(){
	debug("====benchmark of lookup====");
	lookup_t* lookup=lookup_custom(
			sizeof(point_t),
			512, // page size
			NULL, // hash function.
			xmalloc,
			xfree
			    );
	if(!lookup){
		perror("");
		return;
	}
	
	debug("Key put test.");
	int broke=0;
	int tests_failed=0;
	int times=1000000;
	int inner_loop_iterates=100;
	int outer_loop_iterates=times/inner_loop_iterates;
	clock_t start;
	clock_t end;
	double time_elapsed;
	int total_tests=3;
	
	start=clock();
	for(int i=0;i<outer_loop_iterates;i++){
		for(int j=0;j<inner_loop_iterates;j++){
			point_t p= {
				(uint32_t) i,
				(uint32_t) j,
			};
			if(lookup_add(lookup,&p)!=0){
				debug("Failed after %d puts",i*j);
				broke=1;
				tests_failed++;
				break;
			}
		}
	}
	end=clock();
	
	if(!broke){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("Test passed.");
		debug("%d operations in %.3f seconds",times,time_elapsed);
	} else{
		debug("Test failed.");
	}
	
	point_t nonexistent={ (uint32_t)(times+1), (uint32_t)(times+33) };

	debug("Nonexistent key test.");
	// This shall only happen when a hash function is way too terrible.
	// e.g
	//
	// uint32_t hash(void* p,uint32_t s) {
	// return 42;
	// }
	if(lookup_contains(lookup,&nonexistent)){
		tests_failed++;
		debug("Test failed");
	}else{
		debug("Test passed.");
	}

	debug("Missing entries test.");
	int missing_entries=0;
	broke=0;
	start=clock();
	for(int i=0;i<outer_loop_iterates;i++){
		for(int j=0;j<inner_loop_iterates;j++){
			point_t p={
				(uint32_t)i,
				(uint32_t)j,
			};
			if(!lookup_contains(lookup,&p)){
				if(!broke){
					debug("Failed after %d gets",j*i);
					tests_failed++;
					broke=1;
				}
				missing_entries++;

			}
		}
	}
	end=clock();
	if(missing_entries){
		tests_failed++;
		debug("Out of %d entries %d entries were missing.",times,missing_entries);
	}else{
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("%d operations in %.3f seconds",times,time_elapsed);
		debug("No missing entries were found.");
		debug("Test passed.");
	}
	lookup_destroy(lookup);
	lookup=NULL;
	if(tests_failed){
		debug("%d tests failed out of %d",tests_failed,total_tests);
		return;
	}
	debug("All tests passed.");
	memory_leak();
}

int main(){
	benchmark();
	return 0;
}
