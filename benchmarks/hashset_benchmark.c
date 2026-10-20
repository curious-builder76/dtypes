#include<stdio.h>
#include<string.h>
#include<time.h>

#define __HASHSET_DEBUG
#include "print.h"
#include "dtypes/hashset.h"
#include "dtypes/hash_function.h"
#include "test_alloc.h"

void* prepare_data(size_t times, size_t input_size){
	size_t mem_required=times*input_size;

	char* memory=malloc(mem_required);
	if(!memory){
		return NULL;
	}
	memset(memory,0,mem_required);
	size_t pos=0;
	for(size_t idx=0;idx<times;idx++){
		char* ptr=memory+pos;
		sprintf(ptr,"key_%zu",idx);
		pos+=input_size;
	}

	return memory;
}
void benchmark(){

	hashset_debug("====benchmark of hashset====");
	size_t key_size=32;
	hashset_t* set;
	size_t table_size=8;
	size_t bucket_capacity=64;
	do{
		set=hashset_custom(
				table_size, bucket_capacity,
				key_size, default_hash_function, // from hash function.c
				memcmp, xmalloc, xfree, NULL
				);
	}
	while(set==NULL);
	printf(
			"Initial table size: %zu\n"
			"Object size: %zu\n"
			"Bucket capacity (lifetime constant): %zu\n"
			"Hash function(default): djb hash\n",
		        table_size, 
			key_size, 
			bucket_capacity
	      );
	int times=5000000;
	char* data=prepare_data(times,key_size);
	if(!data){
		hashset_destroy(set);
		printf("Failed to prepare data\n");
		return;
	}

	int tests_failed=0;
	
	clock_t start;
	clock_t end;
	char put_test_failed=0;
	hashset_debug("Key put test");
	start=clock();
	for(int i=0;i<times;i++){
		char* key=data + key_size*i;
		int ret=hashset_put(set,key);
		if(ret!=0){
			hashset_debug("Failed after %d keys",i);
			tests_failed+=1;
			put_test_failed=1;
			break;
		}
	}
	end=clock();
	if(!put_test_failed){
		double time_elapsed= (double)(end-start)/CLOCKS_PER_SEC;
		print_benchmark("PUT",times,time_elapsed);
	}else{
		hashset_debug("Test failed.");
	}

	char has_test_failed=0;

	hashset_debug("Key contains test");
	start=clock();
	for(int i=0;i<times;i++){
		char* key=data+ i*key_size;
		int ret=hashset_contains(set,key);
		if(!ret){
			hashset_debug("Failed after %d keys",i);
			tests_failed+=1;
			has_test_failed=1;
		}
	}
	end=clock();
	if(!has_test_failed){
		double time_elapsed= (double)(end-start)/CLOCKS_PER_SEC;
		print_benchmark("CONTAINS",times,time_elapsed);
	}else{
		hashset_debug("Test failed.");
	}
	hashset_destroy(set);
	free(data);
	memory_leak();
	set=NULL;
}


int main(){
	benchmark();
	return 0;
}
