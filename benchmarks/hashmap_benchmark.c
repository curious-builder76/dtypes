
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdint.h>

#define __HASHMAP_DEBUG
#include "dtypes/hashmap.h"
#include "test_alloc.h"
#include "print.h"
#include "dtypes/hash_function.h"




void* prepare_data(size_t times,size_t input_size){
	size_t mem_required=input_size*times;
	size_t pos=0;
	char*  memory=malloc(mem_required);
	if(!memory) return memory;

	memset(memory, 0, mem_required);

	for(size_t idx=0;idx<times;idx++){
		char* ptr=memory+pos;
		sprintf(ptr,"key_%zu",idx);
		pos+=input_size;
	}
	return memory;
}
void tidwall_me(){
	size_t bucket_capacity=64;
	size_t initial_table_size=1024;
	size_t key_size=sizeof(int);
	size_t value_size=key_size;
	uint32_t murmur_seed=1792419563;
        hashmap_t* map;
	do{
		map=hashmap_custom(initial_table_size, bucket_capacity, 
				key_size, value_size,
				murmur_hash, memcmp, malloc, free, &murmur_seed
				);
	}while(map==NULL);
	debug("====benchmark of hashmap (comparable to tidwall)====");
        int times=5000000;
	printf(
			"Initial table size: %zu\n"
			"Key size: %zu\n"
			"Value size: %zu\n"
			"Murmur hash seed (constant seed): %u\n"
			"Bucket capacity (lifetime constant): %zu\n",
			initial_table_size,
			key_size, value_size,
			murmur_seed,
			bucket_capacity
			);
        int tests_failed=0;

        debug("Key put test");
	
	double time_elapsed;
	clock_t start;
	clock_t end;
	start=clock();
        for(int i=0;i<times;i++){
		int val=i+1;
                int ret=hashmap_put(map,&i,&val);
                if(ret!=0){
                        debug("Failed after %d keys,",i);
                        tests_failed+=1;
                        break;
                }
        }
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
	if(tests_failed!=1){
		print_benchmark("PUT",times,time_elapsed);
		debug("Test passed");
	}
        debug("Key get test.");
	start=clock();
        int key_not_found=0;
        for(int i=0;i<times;i++){
                int* value=hashmap_get(map,&i);
                if(value==NULL || *value != i+1){
                        debug("Key was not found : %d",i);
                        tests_failed+=1;
                        key_not_found=1;
			// debug_hashmap(map);
                        break;
                }

        }
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
        if(key_not_found){
                debug("Test failed.");
        }else{
		print_benchmark("GET",times,time_elapsed);
                debug("Test passed.");
        }

        hashmap_destroy(map);
        debug("All tests passed.");
	memory_leak();
        map=NULL;
}

void hashmap_benchmark(){
	size_t bucket_capacity=64;
	size_t initial_table_size=1024;
	size_t key_size=32;
	size_t value_size=key_size;
        hashmap_t* map;
	do{
		map=hashmap_custom(initial_table_size, bucket_capacity, 
				key_size, value_size,
				default_hash_function, memcmp, xmalloc, xfree, NULL
				);
	}while(map==NULL);
	debug("====benchmark of hashmap====");
        int times=5000000;
	printf(
			"Initial table size: %zu\n"
			"Key size: %zu\n"
			"Value size: %zu\n"
			"Bucket capacity (lifetime constant): %zu\n",
			initial_table_size,
			key_size, value_size,
			bucket_capacity
			);
	char* data=prepare_data(times,key_size);
	if(!data){
		printf("Failed to prepare data\n");
		hashmap_destroy(map);
		return ;
	}
        int tests_failed=0;

        debug("Key put test");
	
	double time_elapsed;
	clock_t start;
	clock_t end;
	start=clock();
        for(int i=0;i<times;i++){
		char* ptr=data + i*key_size;
                int ret=hashmap_put(map,ptr,ptr);
                if(ret!=0){
                        debug("Failed after %d keys,",i);
                        tests_failed+=1;
                        break;
                }
        }
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
	if(tests_failed!=1){
		print_benchmark("PUT",times,time_elapsed);
		debug("Test passed");
	}
        debug("Key get test.");
	start=clock();
        int key_not_found=0;
        for(int i=0;i<times;i++){
		char* ptr=data + i*key_size; 
                char* value=hashmap_get(map,ptr);
                if(value==NULL){
                        debug("Key was not found : %s",ptr);
                        tests_failed+=1;
                        key_not_found=1;
			// debug_hashmap(map);
                        break;
                }

        }
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
        if(key_not_found){
                debug("Test failed.");
        }else{
		print_benchmark("GET",times,time_elapsed);
                debug("Test passed.");
        }

	free(data);
        hashmap_destroy(map);
        debug("All tests passed.");
	memory_leak();
        map=NULL;
}

int main(){

	hashmap_benchmark();
	tidwall_me();
        return 0;
}

