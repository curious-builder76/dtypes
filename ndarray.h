#ifndef NDARRAY_IMPL_H

#define NDARRAY_IMPL_H 

struct __ndarray__;

typedef struct __ndarray__ ndarray_t;


ndarray_t* ndarray_custom(void* (*xmalloc)(size_t), void (*xfree)(void*),size_t obj_size,size_t ndims,...);

ndarray_t* ndarray_new(size_t obj_size,size_t ndims,...);

void* ndarray_get(ndarray_t* array,...);

int ndarray_put(ndarray_t* a,void* obj,...);
void ndarray_destroy(ndarray_t* a);

#endif

