
#include<stdio.h>
#include "dtypes/trie.h"
#include "test_alloc.h"

int main(){
	fprintf(stderr,"==== tiny test of trie.====\n");
        trie_t* trie=trie_custom(
                        xmalloc,
                        xrealloc,
                        xfree
                        );
	if(!trie){
		return 1;
	}

	trie_insert(trie,"apple");
	trie_insert(trie,"aplication");
	trie_insert(trie,"armour");
	trie_insert(trie,"banana");
        puts( trie_contains(trie,"apple") ? "true" : "false");
        trie_destroy(trie);
        memory_leak();
        return 0;


}

