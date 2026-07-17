#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<time.h>

#include "dtypes/bitset.h"

#define debug(...) do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)


#define CODE_BLOCK(condition)\
	if(condition){\
			debug("Failed after %d operations",i);\
			failed=1;\
			break;\
		}

#define FMT_STRING "%zu operations in %.3f seconds"

#define BENCHMARK(test_name,condition, times,failure_variable)do{\
	clock_t start;\
	clock_t end;\
	double time_elapsed;\
	char failed=0;\
	start=clock();\
	debug(test_name);\
	for(int i=0;i<times;i++){\
		condition;\
	}\
	end=clock();\
	time_elapsed= (double)(end-start)/CLOCKS_PER_SEC;\
	if(!failed){\
		debug("Test passed");\
		debug(FMT_STRING,(size_t)times, time_elapsed);\
	}\
	else{\
		debug("Test failed.");\
		failure_variable ++;\
	}\
}while(0)



void benchmark(){
	debug("====benchmark of bitset====");
	// Create an bitset of 10000 bits
	int times=100000;
	bitset_t* bitset=bitset_new(times);
	if(!bitset){
		perror("bitset_new()");
		return;
	}
	int tests_failed=0;
	int total_tests=5;
	debug("Nonexistent bit test.");
	if(bitset_checkbit(bitset,random()%times)){
			debug("Test failed");
			tests_failed++;
	}else{
		debug("Test passed.");
	}
	BENCHMARK("setbit test",CODE_BLOCK(bitset_setbit(bitset,i)), times, tests_failed);
	BENCHMARK("checkbit test",CODE_BLOCK(bitset_checkbit(bitset,i)!=1), times, tests_failed);
	BENCHMARK("clear test",CODE_BLOCK(bitset_clearbit(bitset,i)), times, tests_failed);
#undef CODE_BLOCK
#undef FMT_STRING 
#define FMT_STRING "%zu random operations in %.3f seconds"
#define CODE_BLOCK(code) code
	bitset_destroy(bitset);
	bitset=NULL;
	bitset=bitset_new(times);
	if(!bitset){
		perror("bitset_new()");
		return;
	}
	BENCHMARK("100000 random operations",
			CODE_BLOCK(
				switch(random() % 3){
				case 0:
				if(!bitset_clearbit(bitset,i) && bitset_checkbit(bitset,i)){
				debug("Failed at clear and check simultaneous.");
				failed=1;
				}
				break;
				case 1:
				if(!bitset_setbit(bitset,i) && !bitset_checkbit(bitset,i)){
				debug("Failed at set and check simultaneous.");
				failed=1;
				}
				break;
				case 2:
				if(bitset_checkbit(bitset,i) && !bitset_togglebit(bitset,i) && bitset_checkbit(bitset,i)){
				debug("Failed at check toggle and check simultaneous.");
				failed=1;
				}
				break;
				}
				),
				times,
				tests_failed
					);
	bitset_destroy(bitset);
	bitset=NULL;
	if(!tests_failed){
		debug("All tests passed.");
	}else{
		debug("%d tests failed out of %d",tests_failed,total_tests);
	}
}

int main(){
	benchmark();
	return 0;
}
