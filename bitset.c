#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "dtypes/bitset.h"

typedef struct __bitset{
	void (*free)(void*);
	size_t capacity;
	size_t items_stored;
	uint8_t* bits;
}bitset_t;


bitset_t* bitset_custom(size_t capacity, void* (*xmalloc)(size_t), void (*xfree)(void*)){
	bitset_t* set=xmalloc(sizeof(bitset_t));
	if(!set)return NULL;

	set->bits=xmalloc(capacity>>3);
	if(!set->bits){
		xfree(set);
		return NULL;
	}

	set->free=xfree;
	set->items_stored=0;
	return set;
}


bitset_t* bitset_new(size_t capacity){
	return bitset_custom(capacity,malloc,free);
}

int bitset_checkbit(bitset_t* set,size_t index){
	return set->bits[index>>3] & (1<<(index & 7));
}

int bitset_readbit(bitset_t* set, size_t index){
	return bitset_checkbit(set,index);
}

void bitset_destroy(bitset_t* set){
	set->free(set->bits);
	set->free(set);
}

#define DECL_BITFUNCTION(name,op)\
int bitset_##name(bitset_t* set,size_t index){\
	set->bits[index>>3] op (1<<(index & 7 ));\
	return 0;\
}


DECL_BITFUNCTION(setbit,|=)
DECL_BITFUNCTION(togglebit,^=)
DECL_BITFUNCTION(clearbit,&= ~)


