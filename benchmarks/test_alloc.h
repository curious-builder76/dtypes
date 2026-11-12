

#ifndef __TEST_ALLOC_IMPL__
#define __TEST_ALLOC_IMPL__

#include<inttypes.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<time.h>

static uint64_t total_mem_allocated=0;
static uint64_t total_mem_freed=0;


static char hostile_enviroment=0;

static int null_frequency=5;

static uint64_t malloc_called=0;
static uint64_t realloc_called=0;
static uint64_t free_called=0;



static inline void* xmalloc(size_t size){
	static char seeded=0;
	if(!seeded){
		seeded=1;
		srand(time(0));
	}
	errno=0;
	if( hostile_enviroment && rand()%null_frequency==0){
		errno=ENOMEM;
		return NULL;
	}
	size_t* p=(size_t*)malloc(sizeof(size_t)+size);
	if(!p){
		return NULL;
	}
	malloc_called++;
	*p++=size;
	total_mem_allocated+= (uint64_t) size;

	return p;
}

static inline void xfree(void* ptr){
	if(!ptr)
		return;
	free_called++;
	size_t* orig_ptr=(size_t *)ptr;
	orig_ptr--;

	total_mem_freed+=(uint64_t)(*orig_ptr);

	free(orig_ptr);
}

static inline void* xrealloc(void* ptr, size_t new_size){
	errno=0;
	if( hostile_enviroment && rand()%null_frequency==0){
		errno=ENOMEM;
		return NULL;
	}
	// If null return fresh memory.
	if(!ptr){
		return xmalloc(new_size);
	}

	realloc_called++;
	size_t* old_ptr=(size_t *)ptr;

	// Restore the pointer to original position.
	old_ptr--;


	// Store old size.
	size_t old_size=*old_ptr;

	// Create new pointer.
	size_t* new_ptr=(size_t *)realloc(old_ptr,new_size+sizeof(size_t));

	if(!new_ptr)
		return NULL;

	// Write new meta data and advance.
	*new_ptr++=new_size;

	// Calculate diffrence.
	if(old_size>new_size){
		total_mem_freed+= (uint64_t)(old_size-new_size);
	}else{
		total_mem_allocated+=(uint64_t)(new_size-old_size);
	}

	return new_ptr;


}

static inline void memory_leak(void)  {
	if(total_mem_allocated==total_mem_freed){
		fprintf(stderr,"No memory leaks.\n");
		return;

	}

	uint64_t diffrence=total_mem_allocated-total_mem_freed;
	fprintf(stderr,"%" PRIu64 " bytes leaked.\n",diffrence);
	fprintf(stderr,"total malloc() called: %" PRIu64 "\n", malloc_called);
	fprintf(stderr,"total realloc() called: %" PRIu64 "\n", realloc_called);
	fprintf(stderr,"total free() called: %" PRIu64 "\n", free_called);

	fprintf(stderr,"malloc+realloc-free: %" PRIu64 "\n", malloc_called+realloc_called-free_called);
	fprintf(stderr,"realloc-free: %" PRIu64 "\n", realloc_called-free_called);
	fprintf(stderr,"malloc-free: %" PRIu64 "\n", realloc_called-free_called);
}

#endif
