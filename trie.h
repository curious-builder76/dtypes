#ifndef __TRIE_IMPL__
#define __TRIE_IMPL__


#include<stdio.h>

struct __trie;

typedef struct __trie trie_t;

trie_t* trie_custom(
		void* (*xmalloc)(size_t),
		void* (*xrealloc)(void*, size_t),
		void (*free)(void*)
		);
trie_t* trie_new();

int trie_insert(trie_t* , char*);

int trie_contains(trie_t*, char*);

void trie_destroy(trie_t*);

#endif


