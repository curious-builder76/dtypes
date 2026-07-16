#ifndef __BITSET_IMPL_H__
#define __BITSET_IMPL_H__
#include<stdint.h>

struct __bitset;

typedef struct __bitset bitset_t;

bitset_t* bitset_custom(size_t capacity, void* (*)(size_t), void (*)(void*));
bitset_t* bitset_new(size_t capacity);

int bitset_checkbit(bitset_t*, size_t );
int bitset_setbit(bitset_t*, size_t );
int bitset_clearbit(bitset_t*, size_t );
int bitset_togglebit(bitset_t*, size_t );
int bitset_readbit(bitset_t*, size_t );

size_t bitset_population(bitset_t*, size_t);
/*
size_t bitset_find_first(bitset_t*);
size_t bitset_find_last(bitset_t*);
*/
void bitset_destroy(bitset_t* );


#endif

