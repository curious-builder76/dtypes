#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<time.h>

#include "dtypes/deque.h"
#include "test_alloc.h"


#define debug(...)do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)

void benchmark(){
	deque_t* deque=NULL;
	deque=deque_custom(sizeof(int),malloc,free);
	if(!deque){
		perror("");
		return;
	}
	int times=10000;
	clock_t start;
	clock_t end;
	double time_elapsed;
	char failed=0;
	int tests_failed=0;
	int total_tests=3;
	debug("====benchmark of deque====");
	debug("%d sequential appends.",times);
	start=clock();
	for(int i=0;i<times;i++){
		if(deque_append(deque,&i)){
			debug("Failed after %d appends.",i);
			failed=1;
			break;
		}
	}
	end=clock();
	if(!failed){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("deque_append: %d operations in %.6f seconds.",times,time_elapsed);
		debug("Test passed.");
	}else{
		tests_failed++;
		debug("Test failed.");
	}
	debug("%d sequential pops.",times);
	start=clock();
	failed=0;
	for(int i=0;i<times;i++){
		int* k=deque_popleft(deque);
		if(k==NULL || *k!=i){
			debug("Failed after %d pops.",i);
			failed=1;
			break;
		}
	}
	end=clock();
	if(!failed){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("deque_popleft: %d operations in %.3f seconds.",times,time_elapsed);
	}else{
		tests_failed++;
		debug("Test failed.");
	}
	srand(time(NULL));
	debug("%d random operations.",times);
	start=clock();
	failed=0;
	for(int i=0;i<times;i++){
		switch (rand()%4){
			case 0:
				if(deque_append(deque,&i))
					failed=1;
				break;
			case 1:
				if(deque_appendleft(deque,&i))
					failed=1;
				break;
			case 2:
				deque_pop(deque);
				break;
			case 3:
				deque_popleft(deque);
				break;
		}
		if(failed){
			debug("Failed after %d operations.",i);
			break;
		}
	}
	end=clock();
	if(!failed){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("%d random operations  were performed in %.6f seconds.",times,time_elapsed);
	}else{
		tests_failed++;
		debug("Test failed.");
	}

	deque_destroy(deque);
 	if(tests_failed){
		debug("%d tests failed out of %d",tests_failed,total_tests);
	}else
		debug("All tests passed.");
	memory_leak();

}
int main(){
	benchmark();
	return 0;
}

