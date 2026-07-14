#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdio.h>

typedef unsigned char uchar;


typedef struct __deque{
	void* (*malloc)(size_t);
	void (*free)(void*);
	size_t capacity;
	size_t element_size;
	size_t size;
	size_t front;
	size_t rear;
	uchar* buff;
}deque_t;


// Note to self:
//
// For the sake of complexity of algorithms.
// Don't add dynamic resizing (as of now).
//
// Add it at future. when you are ready for it.
//




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
// you shall not mutate the deque as it may lead to mutate the pointer 
// you are holding. as the pointer is owned by deque it is subject to change.
//  



// Return a deque  fine tuned as much as 
// allowed by the API. Or NULL upon 
// failure.
//
deque_t* deque_custom(size_t element_size,size_t max_capacity, void* (*xmalloc)(size_t), void (*xfree)(void*)){
	deque_t* deque=xmalloc(sizeof(deque_t));
	if(deque==NULL)
		return NULL;
	size_t memory_required=element_size*max_capacity;
	uchar* buff=malloc(memory_required);
	if(!buff){
		xfree(deque);
		return NULL;
	}

	deque->malloc=xmalloc;
	deque->free=xfree;
	deque->capacity=max_capacity;
	deque->front=0;
	deque->rear=0;
	deque->size=0;
	deque->element_size=element_size;
	deque->buff=buff;
	return deque;
}


// Made for convinence purpose and demands just the neccesary arguments only.

deque_t* deque_new(size_t element_size,size_t max_capacity){
	return deque_custom(
			element_size,
			max_capacity,
			malloc,
			free
			);
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



int main(){
	deque_t* deque=deque_new(sizeof(int),1<<5);
	if(!deque){
		return 1;
	}
	for(int i=0;i<32;i++){
		if(deque_append(deque,&i)!=0){
			printf("Pop returned non zero ");
			return 1;
		}
		int* k=deque_popleft(deque);
		if(!k){
			printf("K is NULL.");
			break;
		}
		printf("append: %d popleft: %d\n",i,*k);
	}
	deque_destroy(deque);
	deque=NULL;
	return 0;
}
