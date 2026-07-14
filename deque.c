#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdio.h>

typedef unsigned char uchar;


typedef struct __deque{
	void* (*malloc)(size_t);
	void (*free)(void*);
	char oom;
	size_t capacity;
	size_t element_size;
	size_t size;
	size_t front;
	size_t rear;
	uchar* buff;
}deque_t;




// Note to C developer and friends:
// 
// This note is about regarding the current memory ownership policy.
//
// This implementation of deque has slow appends
// compared to pops. 
//
// And comes with a caveat.
// which is;
//
// while you own a pointer returned by deque_pop* functions
// you shall not mutate the deque as it may lead to mutation on the pointer 
// you are holding. as the pointer is owned by deque it is subject to change.
//  



// Return a deque  fine tuned as much as 
// allowed by the API. Or NULL upon 
// failure.
//
deque_t* deque_custom(size_t element_size,void* (*xmalloc)(size_t), void (*xfree)(void*)){
	deque_t* deque=xmalloc(sizeof(deque_t));
	if(deque==NULL)
		return NULL;
	deque->capacity=512;
	size_t memory_required=element_size*deque->capacity;
	uchar* buff=malloc(memory_required);
	if(!buff){
		xfree(deque);
		return NULL;
	}
	deque->oom=0;
	deque->malloc=xmalloc;
	deque->free=xfree;
	deque->front=0;
	deque->rear=0;
	deque->size=0;
	deque->element_size=element_size;
	deque->buff=buff;
	return deque;
}


// Made for convinence purpose and demands just the neccesary arguments only.

deque_t* deque_new(size_t element_size){
	return deque_custom(
			element_size,
			malloc,
			free
			);
}

// Grow the deque by doubling it's size and 
// resetting it's iternal markers.

int deque_grow(deque_t* deque){
	size_t old_size=deque->capacity*deque->element_size;
	size_t new_size=old_size*2;
	deque->oom=0;
	if(new_size<old_size){
		return 1;
	}
	
	uchar* new_buff=deque->malloc(new_size);
	if(!new_buff){
		deque->oom=1;
		return 1;
	}
	size_t idx=0;
	for(;;idx++){
		uchar* element=deque_popleft(deque);
		if(element==NULL){
			break;
		}
		uchar* new_location=new_buff+(idx*deque->element_size);
		memcpy(new_location,element,deque->element_size);
	}
	deque->front=idx;
	deque->rear=0;
	deque->free(deque->buff);
	deque->buff=new_buff;
	deque->capacity=new_size/deque->element_size;
	return 0;
}

// Append an item from the right side of deque
// and return zero upon succes or a non zero number indicating failure.

int deque_append(deque_t* deque,void* obj){
	if(deque->size>=deque->capacity){
		return 1;
	}
	
	uchar* location= deque->buff + (deque->element_size*deque->front);
	memcpy(location,obj,deque->element_size);
	deque->front= (deque->front+1) % deque->capacity;
	deque->size++;
	return 0;
}


// Pop an item from the right side of deque
// and return a pointer OWNED BY DEQUE.
// Or null upon failure.

void* deque_pop(deque_t* deque){
	if(!deque->size){
		return NULL;
	}
	size_t index= deque->front ?  (deque->front-1) : (deque->capacity-2);
	uchar* ret_point=deque->buff+ (index * deque->element_size);
	deque->front=index;
	deque->size--;
	return ret_point;
}



// Append an item from the left side of the deque.
// And return zero upon success or a non zero number 
// indicating failure.

int deque_appendleft(deque_t* deque,void* obj){
	if(deque->size>=deque->capacity){
		return 1;
	}
	
	size_t new_rear = deque->rear ? (deque->rear-1) : (deque->capacity-2);
	uchar* location= deque->buff + (deque->rear*deque->element_size);
	memcpy(location,obj,deque->element_size);
	deque->rear=new_rear;
	deque->size++;
	return 0;
}



// Pop an item from the left side of deque.
// And return a pointer OWNED BY DEQUE.
// or NULL upon failure.

void* deque_popleft(deque_t* deque){
	if(!deque->size){
		return NULL;
	}
	uchar* location= deque->buff + (deque->element_size*deque->rear);
	deque->rear= (deque->rear+1)%deque->capacity;
	deque->size--;
	return location;
}

// Free the resources held by deque.

void deque_destroy(deque_t* deque){
	if(!deque) return;

	deque->free(deque->buff);
	deque->free(deque);
}



