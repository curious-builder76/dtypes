#ifndef __LOOKUP_IMPL__
#define __LOOKUP_IMPL__

#include<string.h>
#include<stdlib.h>
#include<stdint.h>


struct __lookup_struct;
typedef struct __lookup_struct lookup_t;

lookup_t*  lookup_custom(
		size_t table_size, size_t bucket_capacity,size_t obj_size, 
		uint64_t (*hash)(const void* , size_t, void*),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*),
		void* user_data
		);

lookup_t* lookup_new(size_t obj_size);


int lookup_oom(lookup_t* look);

int lookup_add(lookup_t* look,void* obj);

int lookup_contains(lookup_t* look,void* obj);

void lookup_destroy(lookup_t* look);
#endif
