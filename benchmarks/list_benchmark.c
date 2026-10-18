
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdint.h>

#include "dtypes/list.h"
#include "print.h"

int test_delete(){
	list_t* list=list_new(sizeof(size_t));
	if(!list){
		perror("list_new()");
		return 1;
	}
	for(size_t i=0;i<10;i++){
		if(list_append(list,&i)!=0){
			printf("Appending items failed.\n");
			return 1;
		}
	}
	puts("----Before deleting----");
	for(size_t idx=0;idx<10;idx++){
		size_t* got=list_get(list,idx);
		if(got!=NULL){
			printf("| %zu ",*got);
		}
	}
	puts("");
	for(size_t k=0;k<10;k++){
		puts("\nDeleting 9th item.");
		if(list_delete(list,9)!=0){
			puts("Failed to delete\n");
			list_destroy(list);
			return 1;
		}
		puts("----After deleting----");
		for(size_t idx=0;;idx++){
			size_t* got=list_get(list,idx);
			if(got!=NULL){
				printf("| %zu ",*got);
			}else{
				break;
			}
		}
		printf("\n----Before appending %zu----\n",k);
		for(size_t idx=0;;idx++){
			size_t* got=list_get(list,idx);
			if(got!=NULL){
				printf("| %zu ",*got);
			}else{
				break;
			}
		}
		if(list_append(list,&k)!=0){
			printf("Failed to append.");
		}
		printf("\n----After appending %zu----\n",k);
		for(size_t idx=0;;idx++){
			size_t* got=list_get(list,idx);
			if(got!=NULL){
				printf("| %zu ",*got);
			}else{
				break;
			}
		}
	}
	puts("");
	size_t delete_idx=1;
	for(size_t i=0;i<10;i++){
		puts("----Before deleting----");
		for(size_t idx=0;idx<10;idx++){
			size_t* got=list_get(list,idx);
			if(got!=NULL){
				printf("| %zu ",*got);
			}
			else{
				break;
			}
		}

		if(list_delete(list,delete_idx)!=0){
			printf("\nFailed to delete item: %zu\n",delete_idx);
			debug_list(list);
			puts("---------");
			list_destroy(list);
			return 1;
		}

		puts("");
		puts("----After deleting----");
		for(size_t idx=0;;idx++){
			size_t* got=list_get(list,idx);
			if(got!=NULL){
				printf("| %zu ",*got);
			}else{
				break;

			}
		}
		puts("");

	}
	list_destroy(list);
	return 0;
}


int test_list(){
	list_t* list=list_new(sizeof(size_t));
	if(!list) return 1;

	// append items from 0 to 999999
	// So the list shall look like
	// | 0 | 1 | 2 | 3 | ... | 99999 |
	for(size_t idx=0;idx<1000000;idx++){
		if(list_append(list,&idx)!=0){
			printf("Failed to append item %zu\n",idx);
		}
	}
	// If append is working correct then this shall too.
	for(size_t idx=0;idx<1000000;idx++){
		size_t* got=list_get(list,idx);
		if(got !=NULL && *got!=idx){
			printf("Append is broken.");
			list_destroy(list);
			return 1;
		}
	}
	// This is python equivalent of
	// for i in range(999995):
	//   del list[i]
	for(size_t idx=0;idx<999995;idx++){
		if(list_delete(list,2)!=0){
			printf("Failed to delete 2nd item.");
		}
	}
	for(size_t idx=0;;idx++){
		size_t* got=list_get(list,idx);
		if(got!=NULL){
			printf("| %zu ",*got);
		}else{
			break;

		}
	}

	puts("");
	debug_list(list);
	list_destroy(list);
	return 0;
}

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
		if(list_delete(list,0)!=0){
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

	test_delete();
	test_list();
	debug_list(list);
	list_destroy(list);
	if(tests_failed){
		printf("%d tests failed out of %d\n",tests_failed,total_tests);
	}else{
		puts("All tests passed.");
	}
	return 0;
}
