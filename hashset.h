#ifndef __HASHSET_IMPL
#define __HASHSET_IMPL

#include<stdint.h>
#include<string.h>
#include<stdlib.h>

#ifdef __HASHSET_DEBUG
#include<stdio.h>
#define hashset_debug(...) do {\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)
#else
#define hashset_debug(...)
#endif

#include "hash_function.h"

// spage_t and srecord_t are bound structures of
// hashset_t and are internally used and managed by 
// hashset_* functions.


// srecord_t is a record containing a cached hash of
// it's record.
typedef struct{
	uint32_t hash;
	char* record;
}srecord_t;

// spage_t is an array of srecord_t having constant
// capacity of "capacity" throughout it's lifetime.

typedef struct{
	uint32_t used;
	uint32_t capacity;
	srecord_t* records;
}spage_t;

// hashset is an hashset with page based memory .
// which is friendier to CPU cache.
//
typedef struct{
	uint32_t table_size;
	uint32_t page_size;
	uint32_t element_size;

	void* (*malloc)(size_t);
	void (*free)(void*);
	int (*cmp)(const void* , const void*, size_t );
	uint32_t (*hash)(void*, uint32_t);
	
	spage_t* pages;
}hashset_t;


// Memory Ownership Note:
// Everything the caller passes to hashset gets copied in it and now the hashset
// contains it for its lifetime.



void* hashset_memdup(hashset_t* set,void* buff,uint32_t size){
        char* memptr=set->malloc(size);
        if(memptr==NULL)
                return NULL;
        memcpy(memptr,buff,size);
        return memptr;
}


// Initialze a page.

int spage_init(hashset_t* set,spage_t* page,uint32_t capacity){
	srecord_t* records=set->malloc(sizeof(srecord_t)*capacity);
	if(records==NULL)
		return 1;
	page->used=0;
	page->capacity=capacity;
	page->records=records;
	return 0;
}


// Push an item to page.

int spage_push(spage_t* page,srecord_t record){
       if(page->used>=page->capacity){
	       hashset_debug("Failed to push record with hash %u",record.hash);
	       return 1;
       }
       page->records[page->used++]=record;
       return 0;
}

// Destroy the page.

void spage_destroy(hashset_t* set,spage_t* page){
	set->free(page->records);
	page->records=NULL;
}


// hashmap_custom allows an user to provide a
// custom hash function, allocator and deallocator.
// and much more fine tuning

hashset_t* hashset_custom(
		uint32_t page_size,
		uint32_t element_size,
		uint32_t (*hash_function)(void*,uint32_t),
		int (*compare)(const void* , const void*, size_t ),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		)
{
	hashset_t* set=NULL;
	spage_t* pages=NULL;

	if(compare==NULL)
		compare=memcmp;
	if(hash_function==NULL)
		hash_function=djb_hash;
	set=xmalloc(sizeof(hashset_t));
	if(set==NULL)
		goto hashset_custom_failed;
	set->page_size=page_size;
	set->element_size=element_size;
	set->hash=hash_function;
	set->table_size=512;
	set->malloc=xmalloc;
	set->free=xfree;
	set->cmp=compare;
	pages=xmalloc(sizeof(spage_t)*set->table_size);
	if(pages==NULL)

		goto hashset_custom_failed;
	for(uint32_t idx=0;idx<set->table_size;idx++){
		if(spage_init(set,pages+idx,page_size)==0)
			continue;
		while(idx--){
			spage_destroy(set,pages+idx);
		}
		goto hashset_custom_failed;
	}
	set->pages=pages;
	return set;
hashset_custom_failed:
	if(set!=NULL)
		xfree(set);
	if(pages!=NULL)
		xfree(pages);
	return NULL;

}

// hash_new is made for convinience and ease of use
// on the top of hashset_custom.

hashset_t* hashset_new(
		uint32_t page_size,
		uint32_t element_size,
		uint32_t (*hash_function)(void*, uint32_t),
		int (*compare)(const void* ,const void*, size_t)
		)
{
	compare= compare!=NULL ? compare : memcmp;
	return hashset_custom(
			page_size,
			element_size,
			hash_function,
			compare,
			malloc,
			free
			);
}


// Returns a non zero number if  a page is 90% full in
// the given hashset

int hashset_has_busy_page(hashset_t* set){
        for(uint32_t i=0;i<set->table_size;i++){
                spage_t* page=set->pages+i;
                double load_factor= (double) page->used / (double) page->capacity;
                if(load_factor>=0.9)
                        return 1;
        }
        return 0;
}

// Increase the capacity of given set by doubling it's table size.
// Returns a non zero number upon failure.

int hashset_grow(hashset_t* set){
        uint32_t old_capacity=set->table_size;
        uint32_t new_capacity=old_capacity*2;
        spage_t* old_pages=set->pages;

        spage_t* new_pages=set->malloc(sizeof(spage_t)*new_capacity);
        if(new_pages==NULL)
                goto hashset_grow_failed;
        for(uint32_t idx=0;idx<new_capacity;idx++){
                if(spage_init(set,new_pages+idx,set->page_size)==0)
                        continue;
                goto hashset_grow_failed;
        }
        for(uint32_t i=0;i<old_capacity;i++){
                spage_t* old_page=old_pages+i;
                for(uint32_t j=0;j<old_page->used;j++){
			srecord_t rec=old_page->records[j];
                        uint32_t hash=rec.hash;
                        uint32_t index=hash%new_capacity;
                        spage_t* new_page=new_pages+index;
                        if(spage_push(new_page,rec)!=0){
				goto hashset_grow_failed;
			}
                }
        }
        set->table_size=new_capacity;
        set->pages=new_pages;
	while(old_capacity){
		old_capacity--;
		spage_destroy(set,old_pages+old_capacity);
	}
        set->free(old_pages);
        return 0;
hashset_grow_failed:
        if(new_pages!=NULL){
		while(new_capacity){
			new_capacity--;
			spage_destroy(set,new_pages+new_capacity);
		}
                set->free(new_pages);
	}
        return 1;
}

// Puts an item to hashset.
// Returns a non zero number upon failure,

int hashset_put(hashset_t* set,void* obj){
        if (hashset_has_busy_page(set))
                if(hashset_grow(set)!=0)
                        return 1;
	void* objdup=hashset_memdup(set,obj,set->element_size);
	if(objdup==NULL)
		return 1;

        uint32_t hash=set->hash(obj,set->element_size);
        uint32_t index=hash % set->table_size;
        spage_t* page=set->pages+index;

	srecord_t rec={
		hash,
		objdup
	};
        int ret=spage_push(page,rec);
	if(ret!=0){
		set->free(objdup);
	}
	return ret;
}

//  Returns a non zero number if the given hashset
//  contains "obj" passed to hashset as per provided hash function
//  and compare operator.

int hashset_contains(hashset_t* set,void* obj){
        uint32_t hash=set->hash(obj,set->element_size);
        uint32_t index=hash % set->table_size;

        spage_t* page=set->pages+index;

        for(uint32_t idx=0;idx<page->used;idx++){
		srecord_t* rec=page->records+idx;
		if(rec->hash==hash && set->cmp(obj,rec->record,set->element_size)==0)
			return 1;
	}
	return 0;
}

void hashset_destroy(hashset_t* set){
	while(set->table_size){
		set->table_size--;
		spage_t* page=set->pages+set->table_size;
		for(uint32_t idx=0;idx<page->used;idx++){
			srecord_t* rec=page->records+idx;
			set->free(rec->record);
		}
		spage_destroy(set,page);
	}
	set->free(set->pages);
	set->free(set);
}


#endif



