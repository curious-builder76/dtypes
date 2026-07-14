#ifndef __DEQUE_IMPL_H__
#define __DEQUE_IMPL_H__


// Note:
// this thing is not tested yet,
// and is still in development.
//
// So hidden bugs may exist.

#include<stdint.h>

struct __deque;
typedef struct __deque deque_t;

deque_t* deque_custom(size_t , void* (*)( size_t), void (*)(void*) );
deque_t* deque_new(size_t );

int deque_append(deque_t* , void*);
int deque_appendleft(deque_t* , void*);

void* deque_pop(deque_t* );
void* deque_popleft(deque_t* );

void deque_destroy(deque_t*);


#endif

