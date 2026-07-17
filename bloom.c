#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

#include "dtypes/hash_function.h"


typedef struct{
	void (*free)(void*);
	uint64_t (*hash_functions)[2](void*, size_t);
	size_t capacity;
	size_t element_size;
	uint8_t bits[];
}bloom_t;



bloom_t* bloom_custom(size_t capacity,size_t element_size,uint64_t (*hash1)(void*, size_t), uint64_t (*hash2)(void*,size_t), void* (*xmalloc)(size_t), void (*xfree)(void*)){
	size_t size_actual= (capacity+7)>>3;
	if(!size_actual)
		return NULL;
	size_t mem_required=sizeof(bloom_t)+size_actual;

	bloom_t* bloom=xmalloc(mem_required);
	if(!bloom)
		return NULL;

	bloom->element_size=element_size;
	bloom->free=xfree;
	bloom->hash_functions={ hash1, hash2} 
	boom->capacity=size_actual;
	return bloom;
}

bloom_t* bloom_new(size_t capacity,size_t element_size, uint64_t (*hash1)(void*, size_t), uint64_t (*hash2)(void* , size_t)){
	return bloom_custom(
			capacity,
			element_size,
			hash1,
			hash2,
			malloc,
			free
			);
}

void bloom_destroy(bloom_t* bloom){
	bloom->free(bloom);
}


int bloom_add(bloom_t* bloom,void* obj){ 
	for(int i=0;i<2;i++){
		uint64_t hash= bloom->hash_functions[i](obj,bloom->element_size);

		uint64_t index= hash % bloom->capacity;
		bloom->bits[index>>3]  |= (1u << (index & 7));
	}
	return 0;
}



int bloom_contains(bloom_t* bloom,void* obj){ 
	for(int i=0;i<2;i++){
		uint64_t hash= bloom->hash_functions[i](obj,bloom->element_size);
		uint64_t index= hash % bloom->capacity;
		if(!(bloom->bits[index>>3]  & (1u << (index & 7)))) return 0;
	}
	return 1;
}



