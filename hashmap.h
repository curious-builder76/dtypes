#ifndef __HASHMAP_IMPL

#define __HASHMAP_IMPL

#ifdef __HASHMAP_DEBUG


#include<stdio.h>

#define debug(...) do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)
#else
#define debug(...)
#endif

typedef struct __hashmap_struct;

typedef struct __hashmap_struct hashmap_t;

hashmap_t* hashmap_custom(
		uint32_t page_size,
		uint32_t key_size,
		uint32_t value_size,
		uint32_t (*hash_function)(void* ,uint32_t),
		int (*compare)(const void*, const void*,size_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		);
hashmap_t* hashmap_new(
		uint32_t page_size,
		uint32_t key_size,
		uint32_t value_size,
		uint32_t (*hash_function)(void* , uint32_t),
		int (*compare)(const void*, const void* ,size_t )
		);

int hashmap_grow(hashmap_t* map);

void* hashmap_get(hashmap_t* map,void* key);

void hashmap_destroy(hashmap_t* map);

int hashmap_delete(hashmap_t* map,void* key);
int hashmap_put(hashmap_t* map,void* key,void* value);
#endif
