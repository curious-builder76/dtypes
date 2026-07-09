
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdint.h>

#define __HASHMAP_DEBUG
#include "dtypes/hashmap.c"
#include "test_alloc.h"
#include "dtypes/hash_function.h"
void hashmap_benchmark(hashmap_t* map){
        int times=1000000;
        int tests_failed=0;

	debug("====benchmark of hashmap====");
        debug("Key put test");
	
	double time_elapsed;
	clock_t start;
	clock_t end;
	char ptr[32];
	start=clock();
        for(int i=0;i<times;i++){
                memset(ptr,0,32);
                sprintf(ptr,"key_%d",i);
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
		debug("hashmap_put: %d operations in %.3f seconds",times,time_elapsed);
		debug("Test passed");
	}
        debug("Key get test.");

	start=clock();
        int key_not_found=0;
        for(int i=0;i<times;i++){
                memset(ptr,0,32);
                sprintf(ptr,"key_%d",i);
                char* value=hashmap_get(map,ptr);
                if(value==NULL){
                        debug("Key was not found : %s",ptr);
                        tests_failed+=1;
                        key_not_found=1;
                        break;
                }

        }
	end=clock();
	time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
        if(key_not_found){
                debug("Test failed.");
        }else{
		debug("hashmap_get: %d operations in %.3f seconds",times,time_elapsed);
                debug("Test passed.");
        }
        debug("Busy page test.");
        int busy_pages=0;
        for(uint32_t i=0;i<map->table_size;i++){
                mpage_t* page=map->pages+i;
                if(page->used>=page->capacity){
                        debug("Page: %d is busy page",i);
                        busy_pages++;
                }
        }
        if(busy_pages>0){
                tests_failed++;
                printf("Found %d busy pages out of %u",busy_pages,map->table_size);
        }else{
                debug("No busy pages were found");
        }
        if(tests_failed){
                debug("Tests failed %d out of 3",tests_failed);
                return ;
        }
        debug("All tests passed.");
}

int main(){
        hashmap_t* map=hashmap_custom(
			512,
			32,
			32,
			djb_hash,
			memcmp,
			xmalloc,
			xfree
			);
        hashmap_benchmark(map);
        hashmap_destroy(map);
	memory_leak();
        map=NULL;
        return 0;
}

