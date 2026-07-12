#ifndef __LOOKUP_IMPL__
#define __LOOKUP_IMPL__

/*
 * Page based probablistic lookup table.
 */

/*
 * If the hash function guarantees that  no hash will collide.
 * then this can also guarantees that there will be neither false positives nor negatives.
 */

#include<string.h>
#include<stdlib.h>
#include<stdint.h>


#include "dtypes/lookup.h"
#include "dtypes/hash_function.h"

// lpage_t is bound object of lookup_t and are internally managed by functions
// lookup_* and lpage_* functions.

typedef struct{
	uint32_t used;
	uint32_t capacity;
	uint32_t* hashes;
}lpage_t;



// lookup is a table of hashes (integers) containing the
// fingerprint returned by the hash function.
//
// This supports custom allocators and deallocators.

typedef struct __lookup_struct{
	uint32_t (*hash)(void*, uint32_t);
	void* (*malloc)(size_t);
	void (*free)(void*);
	uint32_t page_size;
	uint32_t element_size;
	uint32_t table_size;
	lpage_t* pages;
}lookup_t;



// Initialize a page

int lpage_init(lookup_t* look,lpage_t* lpage,uint32_t capacity){
	uint32_t* hashes=NULL;
	hashes=look->malloc(sizeof(uint32_t)*capacity);
	if(hashes==NULL)
		goto lpage_new_failed;
	lpage->used=0;
	lpage->capacity=capacity;
	lpage->hashes=hashes;
	return 0;
lpage_new_failed:
	if(hashes!=NULL)
		look->free(hashes);
	return 1;
}


// Check the membership of a hash in the page.
//
// return zero upon failure or else a non zero
// on sucess.

int lpage_has(lpage_t* page,uint32_t hash){
	for(uint32_t idx=0;idx<page->used;idx++){
		if(page->hashes[idx]==hash){
			return 1;
		}
	}
	return 0;
}


// Push an hash to the page.
// return zero upon success or
// a non zero integer indicating 
// failure.

int lpage_push(lpage_t* page,uint32_t hash){
	if(page->used>=page->capacity)
		return 1;
	if(lpage_has(page,hash)){
		return 0; // Supress error.
	}
	page->hashes[page->used++]=hash;
	return 0;
}

// Release the resources aquired by page.
void lpage_destroy(lookup_t* look,lpage_t* page){
	look->free(page->hashes);
	page->hashes=NULL;
}

// Allow the users to fine tune the data structure
// as much allowed by the API.

lookup_t*  lookup_custom(
		uint32_t element_size,
		uint32_t page_size,
		uint32_t (*hash_function)(void*, uint32_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		)
{
	if(hash_function==NULL)
		hash_function=djb_hash;
	lookup_t* look=NULL;
	lpage_t* pages=NULL;
	look=xmalloc(sizeof(lookup_t));
	if(look==NULL)
		goto lookup_new_failed;
	look->malloc=xmalloc;
	look->free=xfree;
	look->hash=hash_function;
	look->page_size=page_size;
	look->element_size=element_size;
	look->table_size=4;
	pages=xmalloc(sizeof(lpage_t)*look->table_size);
	if(pages==NULL)
		goto lookup_new_failed;
	look->pages=pages;
	uint32_t i=0;
	for(;i<look->table_size;i++){
		if(lpage_init(look,look->pages+i,page_size)==0)
			continue;
		while(i--){
			lpage_destroy(look,look->pages+i);
		}
		goto lookup_new_failed;
	}
	return look;
lookup_new_failed:
	if(look!=NULL)
		xfree(look);
	if(pages!=NULL){
		xfree(pages);
	}
	return NULL;
}


// Wrapper around lookup_custom()
// Uses default functions 

lookup_t* lookup_new(
		uint32_t element_size,
		uint32_t page_size,
		uint32_t (*hash_function)(void*, uint32_t)
		)
{
	return lookup_custom(
			element_size,
			page_size,
			hash_function,
			malloc,
			free);
}


// Return a non zero number if any page in the given 
// "lookup" is 90% full or zero otherwise.
//
int lookup_has_hot_bucket(lookup_t* look){
	for(uint32_t i=0;i<look->table_size;i++){
		lpage_t* page=look->pages+i;
		double load_factor= (double) page->used / (double) page->capacity;
		if(load_factor>=0.9)
			return 1;
	}
	return 0;
}

// Grow the size of lookup table by doubling it's size
// and re-distributing it's hashes.
int lookup_grow(lookup_t* look){
	uint32_t old_capacity=look->table_size;
	uint32_t new_capacity=old_capacity*2;
	lpage_t* old_pages=look->pages;
	
	lpage_t* new_pages=look->malloc(sizeof(lpage_t)*new_capacity);
	if(new_pages==NULL)
		goto lookup_grow_failed;
	for(uint32_t idx=0;idx<new_capacity;idx++){
		if(lpage_init(look,new_pages+idx,look->page_size)==0)
			continue;
		while(idx--){
			lpage_destroy(look,new_pages+idx);
		}
		goto lookup_grow_failed;
	}
	for(uint32_t i=0;i<old_capacity;i++){
		lpage_t* old_page=old_pages+i;
		for(uint32_t j=0;j<old_page->used;j++){
			uint32_t hash=old_page->hashes[j];
			uint32_t index=hash%new_capacity;
			lpage_t* new_page=new_pages+index;
			lpage_push(new_page,hash);
		}
	}
	look->table_size=new_capacity;
	look->pages=new_pages;
	while(old_capacity){
		old_capacity--;
		lpage_destroy(look,old_pages+old_capacity);
	}
	look->free(old_pages);
	return 0;
lookup_grow_failed:
	if(new_pages!=NULL)
		look->free(new_pages);
	return 1;
}

// Add an item to the lookup object
// Return zero upon success
// or a non zero number indicating failure

int lookup_add(lookup_t* look,void* obj){
	if (lookup_has_hot_bucket(look))
		if(lookup_grow(look)!=0)
			return 1;

	uint32_t hash=look->hash(obj,look->element_size);
	uint32_t index=hash % look->table_size;
	lpage_t* page=look->pages+index;
	return lpage_push(page,hash);
}

// Returns a non zero number indicating that 
// the lookup contains given object as per
// provided hash function or zero otherwise.

int lookup_contains(lookup_t* look,void* obj){
	uint32_t hash=look->hash(obj,look->element_size);
	uint32_t index=hash % look->table_size;

	lpage_t* page=look->pages+index;

	return lpage_has(page,hash);
}


// Release the resources held by 
// the lookup table.

void lookup_destroy(lookup_t* look){
	while(look->table_size){
		look->table_size--;
		lpage_destroy(look,look->pages+look->table_size);
	};
	look->free(look->pages);
	look->free(look);
}
#endif
