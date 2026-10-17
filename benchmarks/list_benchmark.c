
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdint.h>

#include "dtypes/list.h"
#include "print.h"

int main(){

	puts("====benchmark of linked list====");
	clock_t start;
	clock_t end;
	double time_elapsed;

	int tests_failed=0;
	int total_tests=4;
	int failure_flag=0;

	size_t times=100000;
	list_t* list=list_new(sizeof(size_t));
	if(!list){
		return 1;
	}
	start=clock();
	for(size_t idx=0;idx<times;idx++){
		if(list_append(list,&idx)!=0){
			puts("Test failed.");
			failure_flag=1;
			tests_failed++;
			break;
		}
	}

	end=clock();
	if(!failure_flag){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC; 
		print_benchmark("APPEND",times,time_elapsed);
	}

	start=clock();
	for(size_t idx=0;idx<times;idx++){
		size_t* got=list_get(list,idx);
		if(got==NULL || *got!=idx){
			puts("Test failed.");
			if(!got){
				printf("list get(iteration)\nExpected %zu got null\n",idx);
			}else
				printf("list get(iteration)\nExpected %zu got %zu\n",idx,*got);
			tests_failed++;
			failure_flag=1;
			break;
		}
	}
	end=clock();
	if(!failure_flag){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC; 
		print_benchmark("GET (iteration)",times,time_elapsed);
	}

	start=clock();

	for(size_t idx=0;idx<times;idx++){
		size_t* got=list_get(list,times-1);
		if( got==NULL ||  *got!=times-1){
			tests_failed++;
			puts("Test failed.");
			if(!got) puts("list get (access last element)\nUnexpected null return");
			else{
				printf("list get (access last element)\nExpected %zu got %zu\n",idx,*got);
			}
			failure_flag=1;
			break;
		}
	}
	end=clock();
	if(!failure_flag){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC; 
		print_benchmark("GET (access last element)",times,time_elapsed);
	}
	start=clock();
	for(size_t idx=times;idx!=0;idx--){
		if(list_delete(list,idx-1)!=0){
			printf("Test failed at %zu\n",idx);
			failure_flag=1;
			tests_failed++;
			break;
		}
	}
	end=clock();
	if(!failure_flag){
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC; 
		print_benchmark("DELETE (0th element)",times,time_elapsed);
	}

//	test_delete();
//	debug_list(list);
	list_destroy(list);
	if(tests_failed){
		printf("%d tests failed out of %d\n",tests_failed,total_tests);
	}else{
		puts("All tests passed.");
	}
	return 0;
}
