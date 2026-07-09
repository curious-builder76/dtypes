#ifndef __LOOKUP_IMPL__
#define __LOOKUP_IMPL__

/*
 * Page based probablistic lookup table.
 */

/*
 * Warning: This thing is not tested.
 * and is for educational purpose only. (unfortunately!).
 */

#include<string.h>
#include<stdlib.h>
#include<stdint.h>


struct __lookup_struct;
typedef struct __lookup_struct lookup_t;

lookup_t*  lookup_custom(
		uint32_t element_size,
		uint32_t page_size,
		uint32_t (*hash_function)(void*, uint32_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*)
		);

lookup_t* lookup_new(
		uint32_t element_size,
		uint32_t page_size,
		uint32_t (*hash_function)(void*, uint32_t)
		);

int lookup_has_hot_bucket(lookup_t* look);

int lookup_grow(lookup_t* look);

int lookup_add(lookup_t* look,void* obj);

int lookup_contains(lookup_t* look,void* obj);

void lookup_destroy(lookup_t* look);
#endif
