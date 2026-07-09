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


struct __spage_struct;
typedef struct __spage_struct spage_t;

#else
#define hashset_debug(...)
#endif


struct __hashset_struct;

typedef struct __hashset_struct hashset_t;

hashset_t* hashset_custom(
		uint32_t page_size,
		uint32_t element_size,
		uint32_t (*hash_function)(void*,uint32_t),
		int (*compare)(const void* , const void*, size_t ),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		);

hashset_t* hashset_new(
		uint32_t page_size,
		uint32_t element_size,
		uint32_t (*hash_function)(void*, uint32_t),
		int (*compare)(const void* ,const void*, size_t)
		);

int hashset_has_busy_page(hashset_t* set);

int hashset_grow(hashset_t* set);
int hashset_put(hashset_t* set,void* obj);
int hashset_contains(hashset_t* set,void* obj);

void hashset_destroy(hashset_t* set);


#endif



