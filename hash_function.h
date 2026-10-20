#ifndef __HASH_FUNCTIONS__
#define __HASH_FUNCTIONS__

#include<stdlib.h>

uint64_t djb_hash(void* obj,size_t size);

typedef struct{
	uint64_t seed0;
	uint64_t seed1;
}sip64_t;

typedef struct{
	uint64_t seed0;
}xxh3_t;

uint64_t murmur_hash(const void* data, size_t len, void* seed);

uint64_t sip_hash(const void* data, size_t len, void* user_data);
uint64_t xx3_hash(const void* data, size_t len,void* user_data);



#endif
