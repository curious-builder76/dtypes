

// valgrind feels a bloatware to me.
// and now I am using this custom allocator to detect memory leaks.


#ifndef __TEST_ALLOC_IMPL__
#define __TEST_ALLOC_IMPL__

#include<inttypes.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>

uint64_t total_mem_allocated=0;
uint64_t total_mem_freed=0;


char hostile_enviroment=1;

int null_frequency=2;

uint64_t malloc_called=0;
uint64_t realloc_called=0;
uint64_t free_called=0;

void* xmalloc(size_t size){
	errno=0;
	if( hostile_enviroment && rand()%null_frequency==0){
		fprintf(stderr,"I evil xmalloc() is returning NULL."
				"Stop your program from segfault if you can.\n"
		       );
		fflush(stderr);
		errno=ENOMEM;
		return NULL;
	}
	size_t* p=malloc(sizeof(size_t)+size);
	if(!p){
		return NULL;
	}
	malloc_called++;
	*p++=size;
	total_mem_allocated+= (uint64_t) size;

	return p;
}

void xfree(void* ptr){
	if(!ptr)
		return;
	free_called++;
	size_t* orig_ptr=ptr;
	orig_ptr--;

	total_mem_freed+=(uint64_t)(*orig_ptr);

	free(orig_ptr);
}

void* xrealloc(void* ptr, size_t new_size){
	errno=0;
	if( hostile_enviroment && rand()%null_frequency==0){
		fprintf(stderr,"I evil xrealloc() is returning NULL."
				"Stop your program from segfault if you can.\n"
		       );
		errno=ENOMEM;
		return NULL;
	}
	// If null return fresh memory.
	if(!ptr){
		return xmalloc(new_size);
	}

	realloc_called++;
	size_t* old_ptr=ptr;

	// Restore the pointer to original position.
	old_ptr--;


	// Store old size.
	size_t old_size=*old_ptr;

	// Create new pointer.
	size_t* new_ptr=realloc(old_ptr,new_size+sizeof(size_t));

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

void memory_leak(){
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


