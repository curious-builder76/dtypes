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

/*
 * A generic page based hashmap which is friendlier to CPU cache
 * for general purpose.
 */


// Classic djb hash.

uint32_t djb_hash(void* obj,uint32_t size){
	unsigned char* buff=obj;
	uint32_t hash=5381;
	for(uint32_t i=0;i<size;i++){
		uint32_t c=buff[i];
		hash=(hash<<5)+hash+c;
	}
	return hash;
}

typedef struct{
	uint32_t hash;
	char* key;
	char* value;
}record_t;

typedef struct{
	uint32_t used;
	uint32_t capacity;
	record_t* records;
}mpage_t;

typedef struct{
	uint32_t table_size;
	uint32_t key_size;
	uint32_t value_size;
	uint32_t page_size;
	uint32_t (*hash)(void* , uint32_t);
	int (*compare)(const void*,const void*, size_t);
	mpage_t* pages;
}hashmap_t;

/*
 * An simple utility to duplicate memory.
 */

void* memdup(void* src,uint32_t size){
	void* dst=malloc(size);
	if(dst!=NULL)
		memcpy(dst,src,size);
	return dst;
}

int mpage_init(mpage_t* page,uint32_t capacity){
	record_t* records=malloc(sizeof(record_t)*capacity);
	if(records==NULL)
		return 1;
	page->records=records;
	page->used=0;
	page->capacity=capacity;
	return 0;
}

int mpage_push(mpage_t* page,record_t rec){
	if(page->used>=page->capacity){
		debug("Failed to push record with hash %u",rec.hash);
		return -1;
	}
	page->records[page->used++]=rec;
	return 0;
}

void mpage_destroy(mpage_t* page){
	free(page->records);
}

hashmap_t* hashmap_new(
		uint32_t page_size,
		uint32_t key_size,
		uint32_t value_size,
		uint32_t (*hash_function)(void* ,uint32_t),
		int (*compare)(const void*, const void*,size_t)
		){
	hashmap_t* map=NULL;
	mpage_t* pages=NULL;
	
	map=malloc(sizeof(hashmap_t));
	if(map==NULL)
		goto hashmap_new_failed;
	map->page_size=page_size;
	map->key_size=key_size;
	map->value_size=value_size;

	if(hash_function==NULL)
		hash_function=djb_hash;
	map->hash=hash_function;
	map->compare=compare;
	
	map->table_size=8;

	pages=malloc(sizeof(mpage_t)*map->table_size);
	if(pages==NULL)
		goto hashmap_new_failed;

	map->pages=pages;

	for(uint32_t idx=0;idx<map->table_size;idx++){
		if(mpage_init(pages+idx,page_size)==0)
			continue;
		while(idx>0){
			idx--;
			mpage_destroy(pages+idx);
		}
		goto hashmap_new_failed;
	}
	return map;
hashmap_new_failed:
	if(pages!=NULL)
		free(pages);
	if(map!=NULL)
		free(map);
	return NULL;
}

int hashmap_grow(hashmap_t* map){
	uint32_t old_tablesize=map->table_size;

	uint32_t new_tablesize=old_tablesize*2;
	mpage_t* pages=malloc(sizeof(mpage_t)*new_tablesize);
	if(pages==NULL){
		return 1;
	}
	for(uint32_t idx=0;idx<new_tablesize;idx++){
		if(mpage_init(pages+idx,map->page_size)==0)
			continue;
		while(idx>0){
			idx--;
			mpage_destroy(pages+idx);
		}
		goto hashmap_grow_failed;
	}
	for(uint32_t idx=0;idx<old_tablesize;idx++){
		mpage_t* old_page=map->pages+idx;
		for(uint32_t j=0;j<old_page->used;j++){
			record_t rec=old_page->records[j];
			uint32_t hash=rec.hash;
			uint32_t index=hash% new_tablesize;
			mpage_push(pages+index,rec);
		}
	}
	map->pages=pages;
	map->table_size=new_tablesize;
	return 0;
hashmap_grow_failed:
	if(pages!=NULL)
		free(pages);
	return 1;
}
int hashmap_has_hot_page(hashmap_t* map){
	for(uint32_t idx=0;idx<map->table_size;idx++){
		mpage_t* page=map->pages+idx;
		double load_factor= (double) page->used / (double) page->capacity;
		if (load_factor>=0.9)
			return 1;
	}
	return 0;
}


void* hashmap_get(hashmap_t* map,void* key){
	uint32_t hash=map->hash(key,map->key_size);
	uint32_t index= hash % map->table_size;
	mpage_t* page=map->pages+index;
	for(uint32_t idx=0;idx<page->used;idx++){
		record_t* rec=page->records+idx;
		if(rec->hash==hash && map->compare(key,rec->key,map->key_size)==0){
			return rec->value;
		}
	}
	return NULL;
}


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
	key=memdup(key,map->key_size);
	value=memdup(value,map->value_size);
	if(key==NULL || value==NULL)
		goto hashmap_put_failed;
	uint32_t hash=map->hash(key,map->key_size);
	record_t record={
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
		free(key);
	if(value!=NULL)
		free(value);
	return 1;
}

int hashmap_delete(hashmap_t* map,void* key){
	uint32_t hash= map->hash(key,map->key_size);
	uint32_t index= hash % map->table_size;
	mpage_t* page=map->pages+index;
	char found=0;
	uint32_t idx=0;
	for(;idx<page->used;idx++){
		record_t* rec=page->records+idx;
		if(rec->hash==hash && memcmp(rec->key,key,map->key_size)){
			found=1;
			break;
		}
	}
	if(!found)
		return 1;
	record_t* tmp=page->records+idx;
	free(tmp->key);
	free(tmp->value);
	page->records[idx]=page->records[page->used-1];
	page->used--;
	return 0;
}

void hashmap_destroy(hashmap_t* map){
	while(map->table_size){
		map->table_size--;
		mpage_t* page=map->pages+map->table_size;
		for(uint32_t idx=0;idx<page->used;idx++){
			record_t* rec=page->records+idx;
			free(rec->key);
			free(rec->value);
		}
		mpage_destroy(page);
	}
	free(map);
}


#endif
