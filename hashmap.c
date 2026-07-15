#ifndef __HASHMAP_IMPL

#define __HASHMAP_IMPL

#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#ifdef __HASHMAP_DEBUG

#include<stdio.h>

#define debug(...) do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)
#else
#define debug(...)
#endif


#include "dtypes/hashmap.h"

#include "dtypes/hash_function.h"


/*
 * A generic page based hashmap which is friendlier to CPU cache
 * for general purpose.
 */


/*
 * mrecord_t and mpage_t are bound structures of 
 * hashmap_t and are internally managed by 
 * hashmap_* functions.
 */

typedef struct{
	uint32_t hash; // cached hash.
	char* key;
	char* value;
}mrecord_t;

typedef struct{
	uint32_t used;
	uint32_t capacity;
	mrecord_t* records;
}mpage_t;

typedef struct __hashmap_struct{
	uint32_t table_size;
	uint32_t key_size;
	uint32_t value_size;
	uint32_t page_size;
	void* (*malloc)(size_t);
	void (*free)(void*);
	int (*compare)(const void*,const void*, size_t);
	uint32_t (*hash)(void* , uint32_t);
	mpage_t* pages;
}hashmap_t;

//
// Memory Ownership Note:
// Everything passed to hashmap gets
// copied in it and hashmap owns it 
// for it's life time.


void* hashmap_memdup(hashmap_t* map,void* src,uint32_t size){
	void* dst=map->malloc(size);
	if(dst!=NULL)
		memcpy(dst,src,size);
	return dst;
}

// Initialize a page.

int mpage_init(hashmap_t* map,mpage_t* page,uint32_t capacity){
	mrecord_t* records=map->malloc(sizeof(mrecord_t)*capacity);
	if(records==NULL)
		return 1;
	page->records=records;
	page->used=0;
	page->capacity=capacity;
	return 0;
}

// Push an object to the page.
// Returns a non zero number upon failure.
//

int mpage_push(mpage_t* page,mrecord_t rec){
	if(page->used>=page->capacity){
		debug("Failed to push record with hash %u",rec.hash);
		return -1;
	}
	page->records[page->used++]=rec;
	return 0;
}


// Free the resources acquired by page.

void mpage_destroy(hashmap_t* map,mpage_t* page){
	map->free(page->records);
	page->records=NULL;
}

// hashmap_custom allows an user to
// fine tune a hashmap as much possible by the API.

hashmap_t* hashmap_custom(
		uint32_t page_size,
		uint32_t key_size,
		uint32_t value_size,
		uint32_t (*hash_function)(void* ,uint32_t),
		int (*compare)(const void*, const void*,size_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		)
{
	hashmap_t* map=NULL;
	mpage_t* pages=NULL;
	
	map=xmalloc(sizeof(hashmap_t));
	if(map==NULL)
		goto hashmap_custom_failed;
	map->page_size=page_size;
	map->key_size=key_size;
	map->value_size=value_size;
	map->malloc=xmalloc;
	map->free=xfree;
	if(hash_function==NULL)
		hash_function=djb_hash;
	map->hash=hash_function;
	map->compare=compare;
	
	map->table_size=512;

	pages=xmalloc(sizeof(mpage_t)*map->table_size);
	if(pages==NULL)
		goto hashmap_custom_failed;

	map->pages=pages;

	for(uint32_t idx=0;idx<map->table_size;idx++){
		if(mpage_init(map,pages+idx,page_size)==0)
			continue;
		while(idx>0){
			idx--;
			mpage_destroy(map,pages+idx);
		}
		goto hashmap_custom_failed;
	}
	return map;
hashmap_custom_failed:
	if(pages!=NULL)
		xfree(pages);
	if(map!=NULL)
		xfree(map);
	return NULL;
}
//
// hashmap_new is built in the top of 
// hashmap_custom which is made for ease of use.
hashmap_t* hashmap_new(
		uint32_t page_size,
		uint32_t key_size,
		uint32_t value_size,
		uint32_t (*hash_function)(void* , uint32_t),
		int (*compare)(const void*, const void* ,size_t )
		)
{
	compare=compare ? compare : memcmp;
	return hashmap_custom(
			page_size,
			key_size,
			value_size,
			hash_function,
			memcmp,
			malloc,
			free
			);
}

// Increase the capacity of hashmap by doubling it's key size.
// and also redistribute it's records based on the new size.

int hashmap_grow(hashmap_t* map){
	uint32_t old_tablesize=map->table_size;
	uint32_t new_tablesize=old_tablesize*2;
	mpage_t* pages=map->malloc(sizeof(mpage_t)*new_tablesize);
	mpage_t* old_pages=map->pages;
	if(pages==NULL){
		return 1;
	}
	for(uint32_t idx=0;idx<new_tablesize;idx++){
		if(mpage_init(map,pages+idx,map->page_size)==0)
			continue;
		while(idx>0){
			idx--;
			mpage_destroy(map,pages+idx);
		}
		goto hashmap_grow_failed;
	}
	for(uint32_t idx=0;idx<old_tablesize;idx++){
		mpage_t* old_page=map->pages+idx;
		for(uint32_t j=0;j<old_page->used;j++){
			mrecord_t rec=old_page->records[j];
			uint32_t hash=rec.hash;
			uint32_t index=hash% new_tablesize;
			mpage_push(pages+index,rec);
			
		}
	}
	while(old_tablesize){
		old_tablesize--;
		mpage_destroy(map,old_pages+old_tablesize);
	}
	map->free(old_pages);
	map->pages=pages;
	map->table_size=new_tablesize;
	return 0;
hashmap_grow_failed:
	if(pages!=NULL)
		map->free(pages);
	return 1;
}

// Returns a non zero number if the given hashmap
// has a page 90% full.

int hashmap_has_hot_page(hashmap_t* map){
	for(uint32_t idx=0;idx<map->table_size;idx++){
		mpage_t* page=map->pages+idx;
		double load_factor= (double) page->used / (double) page->capacity;
		if (load_factor>=0.9)
			return 1;
	}
	return 0;
}

// Returns the corresponding value assigned to key if found.
// Or returns null. 
void* hashmap_get(hashmap_t* map,void* key){
	uint32_t hash=map->hash(key,map->key_size);
	uint32_t index= hash % map->table_size;
	mpage_t* page=map->pages+index;
	for(uint32_t idx=0;idx<page->used;idx++){
		mrecord_t* rec=page->records+idx;
		if(rec->hash==hash && map->compare(key,rec->key,map->key_size)==0){
			return rec->value;
		}
	}
	return NULL;
}

// Assigns value to key inside the given hashmap.
// Or update it's value if a key already exists.

int hashmap_put(hashmap_t* map,void* key,void* value){
	if(hashmap_has_hot_page(map)){
		if(hashmap_grow(map)!=0)
			return 1;
	}
	char* ret_value=hashmap_get(map,key);
	if(ret_value!=NULL){
		memcpy(ret_value,value,map->value_size);
		return 0;
	}
	key=hashmap_memdup(map,key,map->key_size);
	value=hashmap_memdup(map,value,map->value_size);
	if(key==NULL || value==NULL)
		goto hashmap_put_failed;
	uint32_t hash=map->hash(key,map->key_size);
	mrecord_t record={
		hash,
		key,
		value
	};
	uint32_t index= hash % map->table_size;

	mpage_t* page=map->pages+index;
	if(mpage_push(page,record)!=0)
		goto hashmap_put_failed;
	return 0;
hashmap_put_failed:
	if(key!=NULL)
		map->free(key);
	if(value!=NULL)
		map->free(value);
	return 1;
}

// Delete a key from hashmap.

int hashmap_delete(hashmap_t* map,void* key){
	uint32_t hash= map->hash(key,map->key_size);
	uint32_t index= hash % map->table_size;
	mpage_t* page=map->pages+index;
	char found=0;
	uint32_t idx=0;
	for(;idx<page->used;idx++){
		mrecord_t* rec=page->records+idx;
		if(rec->hash==hash && map->compare(rec->key,key,map->key_size)==0){
			found=1;
			break;
		}
	}
	if(!found)
		return 1;
	mrecord_t* tmp=page->records+idx;
	map->free(tmp->key);
	map->free(tmp->value);
	page->records[idx]=page->records[page->used-1];
	page->used--;
	return 0;
}

// Release the resourses aquired by hashmap.

void hashmap_destroy(hashmap_t* map){
	while(map->table_size){
		map->table_size--;
		mpage_t* page=map->pages+map->table_size;
		for(uint32_t idx=0;idx<page->used;idx++){
			mrecord_t* rec=page->records+idx;
			map->free(rec->key);
			map->free(rec->value);
		}
		mpage_destroy(map,page);
	}
	map->free(map->pages);
	map->free(map);
}


#endif
