

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

## Bitset




```C
bitset_t* bitset_custom(size_t capacity, void* (*xmalloc)(size_t), void (*xfree)(void*))
```
Returns a custom bitset specified by the user or `NULL` upon failure.

`capacity` is the capacity in bits (not bytes).


Parameter `xmalloc` is the custom allocator provided by user, and `xfree` is the deallocator for the same.

See [xmalloc](#xmalloc) and [xfree](#xfree)

```C
bitset_t* bitset_new(size_t capacity)
```
This is a thin wrapper of `bitset_custom()`
which defaults to the  `malloc()`, and `free()` provided by libc.
equivalent of calling `bitset_custom(capacity, malloc, free)`


`capacity` is the capacity in bits (not bytes).

```C
int bitset_setbit(bitset_t* set,size_t index)
```

Sets a bit in given bitset (irresepective of the bit was set or not).
Returns zero upon success or a non zero number indicating failure 
(Out of Bounds operation.)

`set` is the bitset returned by either `bitset_new()` or `bitset_custom()` and cannot be `NULL`

`index` is the position where the operation will be performed

```C
int bitset_togglebit(bitset_t* set,size_t index)
```
Toggles a bit in given bitset (sets to on (1) if off (0); and off (0) if on (1)).
Returns zero upon success or a non zero number indicating 
failure (Out of Bounds operation.)


`set` is the bitset returned by either `bitset_new()` or `bitset_custom()` and cannot be `NULL`

`index` is the position where the operation will be performed

```C
int bitset_clearbit(bitset_t* set,size_t index)
```


Clears a bit in given bitset (irresepective of the bit was set or not).
Returns zero upon success or a non zero number indicating 
failure (Out of Bounds operation.)


`set` is the bitset returned by either `bitset_new()` or `bitset_custom()` and cannot be `NULL`

`index` is the position where the operation will be performed

```C 
void bitset_destroy(bitset_t* set)
``` 
Releases the resources held by bitset. (the bitset becomes a dangling pointer after that)

`set` is the bitset returned by either `bitset_new()` or `bitset_custom()` and can be `NULL`

## Bloom filters


Bloom filters are the storage efficient versions of lookup structures.

These structures are probabilistic cannot return a false negative but can return a false positive.

Below is a table

| X | Stored  | Not stored |
| :---     | :---    | :---     |
| Lookup returns true | True positive | False positve |
| Lookup returns false | False negative | True negative |


```C
bloom_t* bloom_custom(size_t capacity,size_t element_size,uint64_t (*hash1)(void*, size_t),
 uint64_t (*hash2)(void*,size_t), void* (*xmalloc)(size_t), void (*xfree)(void*))
```

Returns a custom bloom filter or `NULL` indicating failure

`capacity` is the capacity in bits (not bytes).

`element_size` is the size of object or simply  pass `sizeof(object)` to this parameter

`hash1(object, object_size)` is the 1st hash function of bloom filter

`hash2(object, object_size)` is the 2nd hash function of bloom filter

`xmalloc` and `xfree` are the custom allocator and deallocators passed  by user

See [xmalloc](#xmalloc) and [xfree](#xfree)

```C
bloom_t* bloom_new(size_t capacity,size_t element_size, uint64_t (*hash1)(void*, size_t), uint64_t (*hash2)(void* , size_t))
```
`capacity` is the capacity in bits (not bytes).

`element_size` is the size of object or simply  pass `sizeof(object)` to this parameter

`hash1(object, object_size)` is the 1st hash function of bloom filter 

`hash2(object, object_size)` is the 2nd hash function of bloom filter and should not be same as `hash1`

```C
bloom_contains(bloom_t* bloom,void* obj)
```
Check wether the item possibly present
Returns a non zero number if possibly present 
else returns 0

`bloom` is a bloom filter returned by either `bloom_custom()` or `bloom_new()` and cannot be `NULL`

`obj` is the pointer pointing to the object whose membership is to be tested.


```C
int bloom_add(bloom_t* bloom,void* obj)
```
Add an item to the bloom filter

`bloom` is a bloom filter returned by either `bloom_custom()` or `bloom_new()` and cannot be `NULL`

`obj` is the pointer pointing to the item whose membership is to be added.

```C
void bloom_destroy(bloom_t* bloom)
```

Release the resources acquired by the bloom filter;
the  bloom filter becomes a dangling pointer afterwards


`bloom` is a bloom filter returned by either `bloom_custom()` or `bloom_new()` and can be `NULL`




