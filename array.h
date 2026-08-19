#ifndef __ARRAY_IMPL__
#define __ARRAY_IMPL__

#include<stdint.h>

struct __array_struct;

typedef struct  __array_struct array_t;

array_t* array_custom(uint32_t element_size,void* (*xmalloc)(size_t),void* (*xrealloc)(void* , size_t),void (*xfree)(void*));

array_t* array_new(uint32_t element_size);
int array_grow(array_t* array);
uint32_t array_getsize(array_t* array);
int array_put(array_t* array,void* obj);
int array_get(array_t* array,uint32_t pos,void* obj);
void* array_at(array_t* array, uint32_t pos);
int array_clear(array_t* array);
int array_pop(array_t* array,void* obj);
int array_oom(array_t* array);
int array_destroy(array_t* array);

#endif

