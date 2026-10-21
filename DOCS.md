

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

Explaination of the parameters,

`element_size` is the size of element in bytes , just simply pass `sizeof(element_size)` to this parameter. 

[xmalloc]("###xmalloc")
