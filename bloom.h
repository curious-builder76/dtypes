
#ifndef __BLOOM_IMPL__

#define __BLOOM_IMPL__

#include<stdint.h>


struct __bloom;
typedef struct __bloom bloom_t;

bloom_t* bloom_custom(size_t ,size_t ,uint64_t (*)(void*, size_t), uint64_t (*)(void*,size_t), void* (*)(size_t), void (*)(void*));
bloom_t* bloom_new(size_t ,size_t , uint64_t (*)(void*, size_t), uint64_t (*)(void* , size_t));
int bloom_add(bloom_t* ,void* ); 
int bloom_contains(bloom_t* ,void* ); 

#endif
