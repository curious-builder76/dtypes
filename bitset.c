#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "dtypes/bitset.h"


// bitset is an array of "capacity"/8 sized bytes 
// with some extra meta-data containing exactly "capacity"
// bits. 
// where "capacity" is a non zero number through out the lifetime of
// bitset.

typedef struct __bitset{
	void (*free)(void*);
	size_t capacity;
	uint8_t* bits;
}bitset_t;



// The following code is a bit messy and unreadable (apologies for that.);
//
// So it is documented here instead of below.
//
// The code decleares a few functions as follws.
//
// bitset_custom() : Allows you to create a "custom" bitset as much as allowed by the API.
//
// bitset_new()    : This is a thin wrapper of bitset_custom()
//                   which defaults to the  malloc(), and free() provided by libc.
//                   equivalent of calling bitset_custom(capacity, malloc, free)
//
// bitset_checkbit() : Returns 1 if a bit is set, 0 if not. and -1 if out of bounds
//                     operation was performed.
//
// bitset_readbit()  :  this is an alias bitset_checkbit()
//
// bitset_setbit()   : Sets a bit in given bitset (irresepective of the bit was set or not).
//                     Returns zero upon success or a non zero number indicating 
//                     failure (Out of Bounds operation.)
//
// bitset_clearbit() : Clears a bit in given bitset (irresepective of the bit was set or not).
//                     Returns zero upon success or a non zero number indicating 
//                     failure (Out of Bounds operation.)
//
// bitset_togglebit(): Toggles a bit in given bitset (sets to on (1) if off (0); and off (0) if on (1) ).
//                     Returns zero upon success or a non zero number indicating 
//                     failure (Out of Bounds operation.)
// 
// bitset_population(): Returns how many bits are set in the given bitset.
// 
// bitset_destroy() :  Releases the resources held by bitset. (the bitset becomes a dangling pointer
//                     after that)
//


// Not implemented yet. (Commented out.)
// bitset_find_first(): Returns the index where the first bit is set. (zero either indicates a failure or the zero-th bit set
//
// bitset_find_last(): Returns the index where the last bit is set.



bitset_t* bitset_custom(size_t capacity, void* (*xmalloc)(size_t), void (*xfree)(void*)){
	size_t size_actual=(capacity+7)>>3;
	if(!size_actual) return NULL;
	bitset_t* set=xmalloc(sizeof(bitset_t));
	if(!set)return NULL;
	// size_actual = capacity / pow(2,3)
	set->bits=xmalloc(size_actual);
	if(!set->bits){
		xfree(set);
		return NULL;
	}
	set->capacity=capacity;
	memset(set->bits,0,size_actual);
	set->free=xfree;
	return set;
}


bitset_t* bitset_new(size_t capacity){
	return bitset_custom(capacity,malloc,free);
}

int bitset_checkbit(bitset_t* set,size_t index){
	if(index>=set->capacity) return -1;
	if((set->bits[index>>3] & (1<<(index & 7)))!=0) return 1;
	
	return 0;
}

int bitset_readbit(bitset_t* set, size_t index){
	return bitset_checkbit(set,index);
}

size_t bitset_population(bitset_t* set){
	size_t count=0;
	for(size_t idx=0;idx<set->capacity;idx++){
		count+= (size_t) bitset_checkbit(set,idx);
	}
	return count;
}
/*
size_t bitset_find_first(bitset_t* set){

	size_t index=0;
	int ret;
	for(;;index++){
		ret=bitset_checkbit(set,index);
		if(ret==-1)
			return 0;
		if(ret==1)
			break;
	}
	return index;
}

size_t bitset_find_last(bitset_t* set){

	size_t index=set->capacity;
	int ret;
	while(index){
		index--;
		ret=bitset_checkbit(set,index);
		if(ret==1)
			break;
	}
	return index;
}

*/

void bitset_destroy(bitset_t* set){
	set->free(set->bits);
	set->free(set);
}


#define DECL_BITFUNCTION(name,op)\
int bitset_##name(bitset_t* set,size_t index){\
	if(index>=set->capacity)return 1; \
	set->bits[index>>3] op (1<<(index & 7 ));\
	return 0;\
}


DECL_BITFUNCTION(setbit,|=)
DECL_BITFUNCTION(togglebit,^=)
DECL_BITFUNCTION(clearbit,&= ~)


