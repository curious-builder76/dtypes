#ifndef __HASHMAP_IMPL

#define __HASHMAP_IMPL

#ifdef __HASHMAP_DEBUG


#include<stdint.h>

#define debug(...) do{\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)
#else
#define debug(...)
#endif

struct __hashmap_struct;

typedef struct __hashmap_struct hashmap_t;
hashmap_t* hashmap_custom(size_t table_size,size_t bucket_capacity,size_t key_size, size_t value_size,
		uint64_t (*hash_function)(void*, size_t, void* ),
		int (*compare)(const void*,const void* ,size_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*),
		void* user_data);
hashmap_t* hashmap_new(
		size_t page_size,
		size_t key_size
		);

int hashmap_oom(hashmap_t* map);

void* hashmap_get(hashmap_t* map,void* key);

void hashmap_destroy(hashmap_t* map);

int hashmap_delete(hashmap_t* map,void* key);

// void* debug_hashmap(hashmap_t* map);

int hashmap_put(hashmap_t* map,void* key,void* value);
#endif
