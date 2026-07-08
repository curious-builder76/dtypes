#ifndef __ARRAY_IMPL__
#define __ARRAY_IMPL__

#include<stdlib.h>
#include<stdint.h>
#include<string.h>


typedef unsigned char uchar;

typedef struct{
	char oom;
	uint32_t element_size;
	uint32_t capacity;
	uint32_t used;
	void* (*malloc)(size_t);
	void* (*realloc)(void* , size_t);
	void (*free)(void* );
	uchar* elements;
}array_t;


// Creates an new "custom" array.
// Returns null upon failure.
array_t* array_custom(
		uint32_t element_size,
		void* (*xmalloc)(size_t),
		void* (*xrealloc)(void* , size_t),
		void (*xfree)(void*)
		)
{
	uint32_t capacity=512;
	uint32_t mem_required=capacity*element_size;
	
	uchar* elements=NULL;
	array_t* array=NULL;

	elements=xmalloc(mem_required);
	if(elements==NULL) goto array_new_failed;

	array=xmalloc(sizeof(array_t));

	if(array==NULL) goto array_new_failed;

	array->elements=elements;
	array->used=0;
	array->element_size=element_size;
	array->capacity=capacity;
	array->oom=0;
	array->malloc=xmalloc;
	array->realloc=xrealloc;
	array->free=xfree;
	return array;
array_new_failed:
	if(array!=NULL) xfree(array);
	if(elements!=NULL) xfree(elements);
	return NULL;
}

// Wrapper of array_custom for ease of use.
//
array_t* array_new(uint32_t element_size){
	return array_custom( 
			element_size,
			malloc,
			realloc,
			free
			);
}

// Grows the size of array.

int array_grow(array_t* array){
	uint32_t old_cap=array->capacity;
	uint32_t new_capacity=old_cap*2;

	uint32_t mem_required=new_capacity*array->element_size;

	uchar* new_mem=array->realloc(array->elements,mem_required);
	if(new_mem==NULL){
		array->oom=1;
		return 1;
	}
	array->elements=new_mem;
	array->capacity=new_capacity;
	return 0;
}

// Returns the siize of array.

uint32_t array_getsize(array_t* array){
	return array->used;
}

// Appends an item at the end of array.

int array_put(array_t* array,void* obj){
	if (array->used>=array->capacity){
		if(array_grow(array)!=0) 
			return 1;
	}
	uint32_t ret_point=array->used*array->element_size;
	uchar* mem_pos=array->elements+ret_point;
	memcpy(mem_pos,obj,array->element_size);
	array->used++;
	return 0;
}


// Copies "element_size" bytes to array 
// and returns 0 on success and a non zero number on error.

int array_get(array_t* array,uint32_t pos,void* obj){
	if(pos>=array->used){
		return 1;
	}
	uint32_t cur_pos=pos*array->element_size;
	uchar* cursor=array->elements+cur_pos;
	memcpy(obj,cursor,array->element_size);
	return 0;
}


// Returns a direct pointer on success or NULL on failure.
// Which is faster. However you shouldn't keep that pointer ownership.
// Or you should gurantee that as long as you own it, the array won't be modified.
// It may become a dangling pointer after modifications.
//
// Useful for faster iterations

void* array_at(array_t* array, uint32_t pos){
	if(pos>=array->used)
		return NULL;
	uint32_t cur_pos=pos*array->element_size;
	return array->elements + cur_pos;
}


// Clears the array within no time.

int array_clear(array_t* array){
	array->used=0;
	return 0;
}


// Pops an element from the last of array and copies it to obj.

int array_pop(array_t* array,void* obj){
	if(array->used==0)
		return 1;
	array_get(array,array->used-1,obj);
	array->used--;
	return 0;
}

// Returns a non zero number if the last operation failed because
// of OOM (Out of Memory) conditions. 

int array_oom(array_t* array){
	return array->oom;
}

// Destroys the array completely, and releases resources aqquired by it.
// You should'nt use that array after 
// this function is called on it.
int array_destroy(array_t* array){
	array->free(array->elements);
	array->free(array);
	return 0;
}


#endif
