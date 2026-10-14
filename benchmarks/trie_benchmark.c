
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "dtypes/trie.h"

#include "print.h"
#include "test_alloc.h"

#define debug(...)do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)


#define INSERT(trie,what)do{\
	debug("INSERT: %s",what);\
	if(trie_insert(trie,what)!=0){\
		perror("");\
	};\
}while(0)

#define CONTAINS(trie,what)do{\
	debug("CONTAINS: %s  -->  %s",what, trie_contains(trie,what) ? "true":"false");\
}while(0)




trie_t* sequential_inserts(int times){
	int ret_code=0;
	trie_t* trie;
	do{
		trie=trie_custom(xmalloc,xrealloc,xfree);
	}
	while(trie==NULL);

	for(int index=0;index<times;index++){
		char ptr[32];
		memset(ptr,0,sizeof(ptr));
		sprintf(ptr,"value_%d",index);
		if(trie_insert(trie,ptr)!=0){
			debug("Failed after %d inserts.",index);
			ret_code=1;
			break;
		}
	}
	if(ret_code){
		trie_destroy(trie);
		return NULL;
	}
	return trie;
}

int random_lookups(trie_t* trie,int times){
	if(!trie){
		debug("Skipping random insertion test as sequential insertions failed.");
		return 1;
	}
	for(int i=0;i<times;i++){
		char ptr[32];
		int value=random()%times;
		sprintf(ptr,"value_%d",value);
		if(!trie_contains(trie,ptr)){
			debug(

					"Failed after %d operations: "
					"the value %d shall exist. But it doesn't",i,value
			     );
			return 1;
		}
	}
	return 0;
}
int random_inserts(int times){
	int ret_code=0;
	trie_t* trie=trie_custom(xmalloc,xrealloc,xfree);
	if(!trie){
		return 1;
	}
	srand(time(NULL));
	for(int index=0;index<times;index++){
		char ptr[32];
		memset(ptr,0,sizeof(ptr));
		int val=rand()%times;
		sprintf(ptr,"%d",val);
		if(trie_insert(trie,ptr)!=0){
			debug("Failed adter %d inserts.",index);
			ret_code=1;
			break;
		}
	}
	trie_destroy(trie);
	return ret_code;
}

void benchmark(){
	debug("==== benchmark of trie.====");
	trie_t* trie;
	do{
		trie=trie_custom(
			xmalloc,
			xrealloc,
			xfree
			);
	}
	while(!trie);
	INSERT(trie,"apple");
	INSERT(trie,"aplication");
	INSERT(trie,"armour");
	INSERT(trie,"banana");
	CONTAINS(trie,"apple");
	debug("Nonexistent entry test");
	CONTAINS(trie,"panda");
	debug("Existent entry test.");
	CONTAINS(trie,"apple");
	trie_destroy(trie);

	trie=NULL;

	debug("Sequential inserts.");

	int tests_failed=0;
	int times=1000000;
	clock_t start;
	clock_t end;
	double time_elapsed;
	int ret;
	start=clock();
	trie_t* trie2=sequential_inserts(times);
	end=clock();
	if(!trie2){
		debug("Test failed.");
		tests_failed++;
	}else{
		debug("Test passed");
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		print_benchmark("INSERT (sequential)",times,time_elapsed);
	}
	debug("Random inserts.");
	start=clock();
	ret=random_inserts(times);
	end=clock();
	if(ret){
		debug("Test failed.");
		tests_failed++;
	}
	else{
		debug("Test passed.");
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;	
		print_benchmark("INSERT (random)",times,time_elapsed);
	}
	start=clock();
	debug("Random lookups.");
	if(random_lookups(trie2,times)!=0){
		tests_failed++;
	}else{
		end=clock();
		debug("Test passed.");
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		print_benchmark("CONTAINS (random)",times,time_elapsed);
	}
	if(trie2){
		trie_destroy(trie2);
	}
	if(!tests_failed){
		debug("All tests passed.");
	}else{
		debug("%d tests failed out of 3.",tests_failed);
	}
	memory_leak();

}
int main(){
	benchmark();
	return 0;
}
