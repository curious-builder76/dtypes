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



struct __hashset_struct;

typedef struct __hashset_struct hashset_t;

hashset_t* hashset_custom(
		size_t table_size,size_t bucket_capacity,
		size_t key_size,
		uint64_t (*hash_function)(const void* , size_t, void* ),
		int (*compare)(const void* , const void* , size_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*),
		void* user_data);

hashset_t* hashset_new(size_t obj_size);

int hashset_oom(hashset_t* set);
int hashset_put(hashset_t* set,void* obj);
int hashset_contains(hashset_t* set,void* obj);

int hashset_delete(hashset_t* set, void* key);


void hashset_destroy(hashset_t* set);



#endif



