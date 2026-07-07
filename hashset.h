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


void* memdup(void* buff,uint32_t size){
	char* memptr=malloc(size);
	if(memptr==NULL)
		return NULL;
	memcpy(memptr,buff,size);
	return memptr;
}


typedef struct{
	uint32_t hash;
	char* record;
}srecord_t;


typedef struct{
	uint32_t used;
	uint32_t capacity;
	srecord_t* records;
}spage_t;

int spage_init(spage_t* page,uint32_t capacity){
	srecord_t* records=malloc(sizeof(srecord_t)*capacity);
	if(records==NULL)
		return 1;
	page->used=0;
	page->capacity=capacity;
	page->records=records;
	return 0;
}


int spage_push(spage_t* page,srecord_t record){
       if(page->used>=page->capacity){
	       hashset_debug("Failed to push record with hash %u",record.hash);
	       return 1;
       }
       page->records[page->used++]=record;
       return 0;
}

void spage_destroy(spage_t* page){
	free(page->records);
	page->records=NULL;
}

typedef struct{
	uint32_t table_size;
	uint32_t page_size;
	uint32_t element_size;
	uint32_t (*hash)(void*, uint32_t);
	spage_t* pages;
}hashset_t;


hashset_t* hashset_new(uint32_t page_size,uint32_t element_size,uint32_t (*hash_function)(void*,uint32_t)){
	hashset_t* set=NULL;
	spage_t* pages=NULL;
	
	if(hash_function==NULL)
		hash_function=djb_hash;

	set=malloc(sizeof(hashset_t));
	if(set==NULL)
		goto hashset_new_failed;
	set->page_size=page_size;
	set->element_size=element_size;
	set->hash=hash_function;
	set->table_size=4096;
	pages=malloc(sizeof(spage_t)*set->table_size);
	if(pages==NULL)

		goto hashset_new_failed;
	for(uint32_t idx=0;idx<set->table_size;idx++){
		if(spage_init(pages+idx,page_size)==0)
			continue;
		while(idx--){
			spage_destroy(pages+idx);
		}
		goto hashset_new_failed;
	}
	set->pages=pages;
	return set;
hashset_new_failed:
	if(set!=NULL)
		free(set);
	if(pages!=NULL)
		free(pages);
	return NULL;

}
int hashset_has_hot_bucket(hashset_t* set){
        for(uint32_t i=0;i<set->table_size;i++){
                spage_t* page=set->pages+i;
                double load_factor= (double) page->used / (double) page->capacity;
                if(load_factor>=0.9)
                        return 1;
        }
        return 0;
}

int hashset_grow(hashset_t* set){
        uint32_t old_capacity=set->table_size;
        uint32_t new_capacity=old_capacity*2;
	hashset_debug("Hashset is growing...\nOld size: %u New size: %u",old_capacity,new_capacity);
        spage_t* old_pages=set->pages;

        spage_t* new_pages=malloc(sizeof(spage_t)*new_capacity);
        if(new_pages==NULL)
                goto hashset_grow_failed;
        for(uint32_t idx=0;idx<new_capacity;idx++){
                if(spage_init(new_pages+idx,set->page_size)==0)
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
		spage_destroy(old_pages+old_capacity);
	}
        free(old_pages);
        return 0;
hashset_grow_failed:
        if(new_pages!=NULL){
		while(new_capacity){
			new_capacity--;
			spage_destroy(new_pages+new_capacity);
		}
                free(new_pages);
	}
        return 1;
}

int hashset_add(hashset_t* set,void* obj){
        if (hashset_has_hot_bucket(set))
                if(hashset_grow(set)!=0)
                        return 1;
	void* objdup=memdup(obj,set->element_size);
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
		free(objdup);
	}
	return ret;
}
int hashset_put(hashset_t* set,void* obj){
	return hashset_add(set,obj);
}
int hashset_contains(hashset_t* set,void* obj){
        uint32_t hash=set->hash(obj,set->element_size);
        uint32_t index=hash % set->table_size;

        spage_t* page=set->pages+index;

        for(uint32_t idx=0;idx<page->used;idx++){
		srecord_t* rec=page->records+idx;
		if(rec->hash==hash && memcmp(obj,rec->record,set->element_size)==0)
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
			free(rec->record);
		}
		spage_destroy(page);
	}
	free(set->pages);
	free(set);
}


#endif



