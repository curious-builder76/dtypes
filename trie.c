

#include "dtypes/trie.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>


#define MAX_NODES 255


/*
 * Adapted from
 * https://github.com/curious-builder76/trie/
 *
 */


// Item and string are used inter-changably in this file

typedef struct memory_allocator_context{
	int oom;
	void* (*malloc)(size_t);
	void* (*realloc)(void* , size_t);
	void (*free)(void*);
}ctx_t;


typedef struct __trie{
	uint8_t seen_null;
	uint8_t capacity;
	struct __trie* nodes;
}trie_t;

typedef unsigned char uchar;

typedef trie_t node_t;




/* extract the allocator context */

static  ctx_t* get_ctx(node_t* node){
	return (ctx_t*)((char*)node + sizeof(node_t));
}

// Return a custom trie.
//
trie_t* trie_custom(void* (*xmalloc)(size_t), void* (*xrealloc)(void*, size_t), void (*xfree)(void* )){
	size_t mem_required= sizeof(trie_t)+sizeof(ctx_t);

	trie_t* root;
	if((root=xmalloc(mem_required))==NULL){
		return root;
	}
	memset(root, 0, mem_required);
	ctx_t* ctx=get_ctx(root);
	ctx->malloc=xmalloc;
	ctx->realloc=xrealloc;
	ctx->free=xfree;
	return root;
}

// Lookup table used for computing the nearest power of twos
static const uint16_t table[]={
	1, 1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16,
16, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256
};

// Grow a node of the trie using the given context
static inline int trie_grow(trie_t* trie, uchar ch, ctx_t* ctx){
	if(trie->capacity >=MAX_NODES){
		/* At maximum capacity*/
		return 0;
	}


	if(trie->capacity > ch){
		/* already sorted */
		return 0;
	}
	uint16_t cap= trie->capacity ? trie->capacity : 8;

	cap= table[ch+1];
	if(cap>=MAX_NODES)
		cap=MAX_NODES;
	size_t mem_required=sizeof(node_t)*cap;

	node_t* nodes;
	if(trie->nodes==NULL){
		nodes=ctx->malloc(mem_required);
	}else{
		nodes=ctx->realloc(trie->nodes,mem_required);
	}
	if(!nodes){
		return 1;
	}

	memset(nodes+ trie->capacity, 0, (cap-trie->capacity)*sizeof(node_t));
	trie->nodes=nodes;
	trie->capacity=cap;
	return 0;
}

// Insert an item to the trie, Return zero upon success
// or a non zero number indicating failure.
int trie_insert(trie_t* trie, char* input){
	uchar* data= (uchar*)input;


	ctx_t* ctx=get_ctx(trie);
	while(1){
		uchar ch=*data++;
		if(ch==0){
			trie->seen_null=1;
			return 0;
		}

		if(trie_grow(trie,ch,ctx)!=0){
			ctx->oom=1;
			return 1;
		}
		trie= trie->nodes+ (ch-1);
	}
}

// Return a non zero number if the item is present in the trie.
// Or zero indicating absence.
//
int trie_contains(trie_t* trie,char* input){
	uchar* data=(uchar*)input;

	while(1){
		uchar ch=*data++;
		if(ch==0){
			return trie->seen_null;
		}
		if( ch >= trie->capacity){
			return 0;
		}
		trie= trie->nodes + (ch-1);
	}
}

// Returns a non zero number if the last insert operation
// failed because of out of memory condition.
int trie_oom(trie_t* trie){
	ctx_t* ctx=get_ctx(trie);
	return ctx->oom;
}

// Helper for trie_destroy().
// Destroys all the branches of the trie.
static void trie_destroy0(trie_t* trie, ctx_t* ctx){
	if(!trie || !trie->nodes){
		return;
	}

	for(uint16_t idx=0;idx<trie->capacity;idx++){
		trie_t* child=trie->nodes+idx;
		trie_destroy0(child,ctx);
	}
	ctx->free(trie->nodes);
}
// Destroy the trie completely.
void trie_destroy(trie_t* trie){
	if(!trie) return;
	ctx_t* ctx=get_ctx(trie);
	trie_destroy0(trie,ctx);
	ctx->free(trie);
}
