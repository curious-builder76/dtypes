#ifndef DTYPES_LIST_H

#define DTYPES_LIST_H
#include<stdint.h>
struct __list;

typedef struct __list list_t;

list_t* list_custom(size_t obj_size, void* (*xmalloc)(size_t), void* (*xrealloc)(void*, size_t), void (*xfree)(void*));

list_t* list_new(size_t obj_size);
void list_destroy(list_t* l);
void* list_get(list_t* list,size_t idx);
int list_append(list_t* list,void* obj);
int list_put(list_t* list,size_t idx,void* obj);
int list_update(list_t* list,size_t idx,void* obj);
#endif
