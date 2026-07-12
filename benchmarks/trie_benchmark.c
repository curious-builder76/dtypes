
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "dtypes/trie.h"
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




int sequential_inserts(int times){
	int ret_code=0;
	trie_t* trie=trie_custom(xmalloc,xrealloc,xfree);
	if(trie==NULL){
		return 1;
	}

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
	trie_destroy(trie);
	return ret_code;
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
        trie_t* trie=trie_custom(
                        xmalloc,
                        xrealloc,
                        xfree
                        );
	if(!trie){
		perror("");
		return;
	}
	INSERT(trie,"apple");
	INSERT(trie,"aplication");
	INSERT(trie,"armour");
	INSERT(trie,"banana");
	CONTAINS(trie,"apple");
	debug("Nonexistent entry test");
	CONTAINS(trie,"panda");
	debug("Existent entry test.");
	CONTAINS(trie,"apple");

	debug("Long insert short get test.\nExpected: segfault.");
	INSERT(trie,"ppppppppppppppppppppppppppppppppppppp");
	CONTAINS(trie,"ppp");
	debug("If you are looking at this message. the test was passed.");
	debug("Short insert long get test.\nExpected: segfault.");
	INSERT(trie,"zzz");
	CONTAINS(trie,"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
	CONTAINS(trie,"zzz");
	debug("If you are looking at this message. the test was passed.");
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
	ret=sequential_inserts(times);
	end=clock();
	if(ret){
		debug("Test failed.");
		tests_failed++;
	}else{
		debug("Test passed");
		time_elapsed=(double)(end-start)/CLOCKS_PER_SEC;
		debug("trie_insert: %d operations in %.3f seconds.",times,time_elapsed);
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
		debug("trie_insert: %d operations in %.3f seconds.",times,time_elapsed);
	}
	if(!tests_failed){
		debug("All tests passed.");
	}else{
		debug("%d tests failed out of 2.",tests_failed);
	}
	memory_leak();

}
int main(){
	benchmark();
	return 0;
}
