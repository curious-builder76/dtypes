#ifndef DTYPES_LIST_H

#define DTYPES_LIST_H

struct __list;

typedef struct __list list_t;

list_t* list_new(size_t obj_size);
void list_destroy(list_t* l);
void* list_get(list_t* list,size_t idx);
int list_put(list_t* list,size_t idx,void* obj);

#endif
