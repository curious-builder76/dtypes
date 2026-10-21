

# Documentation of `dtypes`


## Table of Contents





## Short description


The following will explain what is that data structure,
which functions are present and what they do for the particular
data structure.



## Custom allocators

This repository uses the following names for custom allocators,

here's a short brief.

### xmalloc
`xmalloc(size_t memory_size)` can be any custom allocator of your choice, i.e an arena allocator, a mmap based allocator or anything you wish to
given it cannot be NULL, and shall return a readable and writable pointer (which can return NULL indicating failures).

`xrealloc(void* old_ptr, size_t new_size)` should resize the pointer/memory/region/chunk to the new provided size, this parameter cannot be NULL,
but can return NULL indicating failures

`xfree(void* allocated_by_malloc_or_realloc)` should release the resources aquired by `xmalloc` and `xrealloc`,
this function can be a dummy function but cannot be NULL.


## Dynamic Arrays

The dynamic array,

The simple data structure we know in C i.e. `item_type name[size]`

which is just abstracted.

### array\_custom

```C
array_t* array_custom(size_t element_size,void* (*xmalloc)(size_t),void* (*xrealloc)(void* , size_t),void (*xfree)(void*))
 
```

It creates an customized array , passing invalid arguments to this/these function(s) is not recomended.


`element_size` is the size of element in bytes , just simply pass `sizeof(element_size)` to this parameter. 

[xmalloc](#xmalloc) is a custom allocator provided by user.
[xrealloc](#xrealloc) is a custom re-allocator provided by user.
[xfree](#xfree) is a custom deallocator provided by user


```C
array_t* array_new(size_t element_size)
``` 

It delegates to `array_custom` with default parameters.

`element_size` is the size of element in bytes , just simply pass `sizeof(element_size)` to this parameter. 

```C
size_t array_getsize(array_t* array)
```
Returns the size (equivalent of length) of an array


```C
void* array_at(array_t* array, size_t pos)
```
Returns a direct pointer on success or NULL on failure.
Which is faster. However you shouldn't keep that pointer ownership.
Or you should gurantee that as long as you own it, the array won't be modified.
It may become a dangling pointer after modifications.

## Bitset

```C
int array_oom(array_t* array)
```
Returns a non zero number if the last operation failed because
of OOM (Out of Memory) conditions.

```C
int array_pop(array_t* array,void* obj)
``
Pops an element from the last of array and copies it to obj.
```C
int array_destroy(array_t* array)
```
Destroys the array completely, and releases resources aqquired by it.
You should'nt use that array after
this function is called on it.

```C
bitset_t* bitset_custom(size_t capacity, void* (*xmalloc)(size_t), void (*xfree)(void*))
```





