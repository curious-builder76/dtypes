#include<stdio.h>
#include<string.h>
#include<time.h>

#define __HASHSET_DEBUG
#include "dtypes/hashset.c"
#include "test_alloc.h"


void benchmark(){

	hashset_debug("====benchmark of hashset====");
	char key[32];
	hashset_t* set=hashset_custom(
			512,
			sizeof(key),
			NULL,
			memcmp,
			xmalloc,
			xfree
			);
	if(set==NULL){
		perror("hashset_new()");
		return;
	}
	
	int times=1000000;

	int tests_failed=0;
	
	clock_t start;
	clock_t end;
	char put_test_failed=0;
	hashset_debug("Key put test");
	start=clock();
	for(int i=0;i<times;i++){
		memset(key,0,sizeof(key));
		sprintf(key,"key_%d",i);
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
		hashset_debug("hashset_put: %d operations in %.3f seconds",times,time_elapsed);
	}else{
		hashset_debug("Test failed.");
	}

	char has_test_failed=0;

	hashset_debug("Key contains test");
	start=clock();
	for(int i=0;i<times;i++){
		memset(key,0,sizeof(key));
		sprintf(key,"key_%d",i);
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
		hashset_debug("hashset_contains: %d operations in %.3f seconds",times,time_elapsed);
	}else{
		hashset_debug("Test failed.");
	}
	int busy_pages=0;
	hashset_debug("Busy page test.");
	for(uint32_t idx=0;idx<set->table_size;idx++){
		spage_t* page=set->pages+idx;
		if(page->used>=page->capacity){
			hashset_debug("Page: %u is busy page.",idx);
			busy_pages++;
		}
	}
	if(busy_pages){
		hashset_debug("Found %u busy pages out of %u pages",busy_pages,set->table_size);
		tests_failed++;
	}else{
		hashset_debug("Test passed.");
	}
	if(!tests_failed){
		hashset_debug("All tests passed.");
	}else
		hashset_debug("%d tests failed out of %d",tests_failed,3);

	hashset_destroy(set);

	memory_leak();
	set=NULL;
}


int main(){
	benchmark();
	return 0;
}
