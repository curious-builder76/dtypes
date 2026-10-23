

# Documentation of `dtypes`


## Table of Contents





## Short description


The following will explain what is that data structure,
which functions are present and what they do for the particular
data structure.



## Custom allocators

This repository uses the following parameter names for custom allocators,

here's a short brief.

### xmalloc
`xmalloc(size_t memory_size)` can be any custom allocator of your choice, i.e an arena allocator, a mmap based allocator or anything you wish to
given it cannot be NULL, and shall return a readable and writable pointer (which can return NULL indicating failures).

### xrealloc
`xrealloc(void* old_ptr, size_t new_size)` should resize the pointer/memory/region/chunk to the new provided size, this parameter cannot be NULL,
but can return NULL indicating failures

### xfree
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

`array` is a non-`NULL` pointer returned by either `array_custom()` or `array_new()` 

```C
void* array_at(array_t* array, size_t pos)
```
Returns a direct pointer on success or NULL on failure.
Which is faster. However you shouldn't keep that pointer ownership.
Or you should gurantee that as long as you own it, the array won't be modified.
It may become a dangling pointer after modifications.


`array` is a non-`NULL` pointer returned by either `array_custom()` or `array_new()` 


```C
int array_oom(array_t* array)
```
Returns a non zero number if the last operation failed because
of OOM (Out of Memory) conditions.


`array` is a non-`NULL` pointer returned by either `array_custom()` or `array_new()` 

```C

int array_pop(array_t* array,void* obj)

``
Pops an element from the last of array and copies it to obj.

`array` is a non-`NULL` pointer returned by either `array_custom()` or `array_new()` 

```C

int array_destroy(array_t* array)

```

Destroys the array completely, and releases resources aqquired by it.
You should'nt use that array after
this function is called on it.


`array` is a  pointer returned by either `array_custom()` or `array_new()`  which can be `NULL`

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

## Deque

The double-ended queue is most often presented as a generalized queue, which gives its name. 
It is a queue that allows insertions and deletions at both ends.


```C
deque_t* deque_custom(size_t element_size,void* (*xmalloc)(size_t), void (*xfree)(void*))
```

Return a custom deque or `NULL` upon failure

`element_size` is the size of element


`xmalloc` and `xfree` are the custom allocator and deallocators passed  by user.

if `xmalloc()` allocates the resources then `xfree()` must release it.

See [xmalloc](#xmalloc) and [xfree](#xfree)
 


```C
deque_t* deque_new(size_t element_size)
```

Thin wrapper around `deque_custom()` same as calling `deque_custom(element_size, malloc, free)`


`element_size` is the size of element;  just pass `sizeof(element)` to this.


```C
int deque_append(deque_t* deque,void* obj)
```

Append an item to the right side of deque;
Returns zero upon success or a non zero number indicating failure.

`deque` is a non-`NULL`  pointer returned by `deque_custom()` or `deque_new()`

`obj` is the item which will be added to the deque, it cannot be `NULL` and it's size should be equal to the 
parameter `element_size` passed to `deque_custom()` or `deque_new()`


```C
void* deque_pop(deque_t* deque)
```

Pop an item from the **right** side of the deque and
return a pointer pointing to the popped item.
In case of failure, this function returns `NULL`


`deque` is the deque from which the item is to be popped. 
This parameter needs to be returned by `deque_custom()` or `deque_new()` and cannot be `NULL` .



```C
int deque_appendleft(deque_t* deque,void* obj)
```
Pop an item from the **left** side of deque
And return a pointer pointing to the item 
or return `NULL` indicating failure.


`deque` is the deque from which the item is to be popped. 
This parameter needs to be returned by `deque_custom()` or `deque_new()` and cannot be `NULL` .

```C
int deque_oom(deque_t* deque)
```

Returns a non zero number if the last operation on deque failed due to
Out of Memory condition.

`deque` is a pointer returned by `deque_custom()` or `deque_new()` and cannot be `NULL`

```C
void deque_destroy(deque_t* deque)
```

Release the resources held by the deque. the deque becomes a dangling pointer after this

`deque` is an pointer returned by `deque_custom()` or `deque_new()` and can be `NULL`.


## Hash functions

These are used internally by the library and are 
exposed to public via `dtypes/hash_function.h`

The data structure constructors accept these in place of 
`hash_function` parametwr.

Useful for customizing a hash based data structure.
These functions have a common pattern in general

```C
uint64_t <name>_hash(const void* data, size_t length, void* hash_context_aka_hash_context)
```
Currently provides

| Hash name | Function name |
| :---  | :--- |
| Murmur hash (64-bit) | `murmur_hash` |
| Sip hash (64-bit) | `sip_hash` |
| XX3 hash (64-bit) | `xxh3_hash` |



### Murmur Hash

```C
uint64_t murmur_hash(const void* data, size_t len, void* seed)
```

Returns Murmur Hash of the data supplied.

`data` is a non `NULL` pointer which should meet the condition `sizeof(*data)==len`

`len` is the size of the `data` supplied.

`seed` is the pointer pointing to a 32-bit integer or in other words to an `uint32_t`
which wil as as the seed of Murmur Hash


Example:

```C
#include<string.h>
#include "dtypes/hash_function.h"


char* name="Murmur Hash";

uit32_t seed= /* <murmur hash seed> */

// Include the nul terminator
uint64_t hashed= murmur_hash(name, strlen(name)+1, &seed);

```

### Sip Hash

```C
uint64_t sip_hash(const void* data, size_t len, void* user_data)
```

Returns the Sip Hash of the data supplied


`data` is a non `NULL` pointer which should meet the condition `sizeof(*data)==len`

`len` is the size of the `data` supplied.


`user_data` is the hash context  (i.e. a `sip64_t`) of this hash function.

 

Example:

```C
#include<string.h>
#include "dtypes/hash_function.h"


char* name="Sip Hash";

sip64_t hash_context={ .seed0= /* first seed */ ,.seed1= /* second seed */ };

// Include the nul terminator
uint64_t hashed= sip_hash(name, strlen(name)+1, &hash_ctx);

```

### XX Hash

```C
uint64_t xxh3_hash(const void* data, size_t len, void* user_data)
```

Returns the XXHash of the data supplied


`data` is a non `NULL` pointer which should meet the condition `sizeof(*data)==len`

`len` is the size of the `data` supplied.


`user_data` is the hash context  (i.e. a `xxh3_t`) of this hash function.

 

Example:

```C
#include<string.h>
#include "dtypes/hash_function.h"


char* name="XX Hash";

xx3h_t hash_context={ /* < XX hash seed > */ };

// Include the nul terminator
uint64_t hashed= xxh3_hash(name, strlen(name)+1, &hash_ctx);
```


## Hash map

A data structure that stores data in form of key value pairs

or in other terms assigns value to key (i.e. `key = value` )


```C
hashmap_t* hashmap_custom(size_t table_size,size_t bucket_capacity,size_t key_size, size_t value_size,
                uint64_t (*hash_function)(const void*, size_t, void* ),
                int (*compare)(const void*,const void* ,size_t),
                void* (*xmalloc)(size_t),
                void (*xfree)(void*),
                void* user_data)
```

Returns a fine tuned hash map as per user specification.


`table_size` is the inital table size you wish it to be but not zero
(useful for preallocating space, loves power of twos)

`bucket_size` is the bucket size which will remain constant through out it's life time (loves power of twos, favours integers >= 64).

`key_size` is the size of key object in bytes and cannot be zero
(simply pass `sizeof(your_desired_key_size_and_it_can_be_anything)` to it.

`value_size` is the size of the value object and cannot be zero

`hash_function(object, object_len, hash_context)` is the hash function used by the 
hashmap to hash structures (any of the functions provided by `"dtypes/hash_function.h"`
will work; use the suitable hash function as per your use case
if you feel creative, invent your own hash function and promise it won't be a terrible hash.)


`compare(a,b)` is a call back used by the data structure to 
compare objects, (similar of `memcmp()` and `strncmp()`) 
which should be a non null pointer (use `memcmp()` instead 
if you don't have any custom compare function)


See [xmalloc](#xmalloc) and [xfree](#xfree)


`user_data` is the context that will be passed to the hash function.

```C

hashmap_t* hashmap_new(size_t key_size,size_t value_size)
```

Wrapper over `hashmap_custom()`
Uses the default configuration and returns a hashmap pointer.

[!] Warning: uses DJB Hash by default. hashmaps returned by this are vulnerable 
to hash flooding (as of now, may change in future)


`key_size` is the size of key object in bytes and cannot be zero

`value_size` is the size of the value object and cannot be zero

```C

void* hashmap_get(hashmap_t* map,void* key)
```

Get an item assigned to `key` from the hashmap. returns `NULL` upon failure.

`map` is a non-`NULL` pointer returned by `hashmap_custom()` or `hashmap_new()`

`key` is the pointer to an object whose assigned value is to be fetched. which cannot be `NULL`


```C

int hashmap_put(hashmap_t* map,void* key,void* value)

```

Assign `key` to `value` incase the `key` doesn't exists.
Or else over write the old assigned `value`

`map` is a non-`NULL` pointer either returned by `hashmap_custom()` 
or `hashmap_new()` 

`key` is a non-`NULL` item for mapping `key` to `value`

`value` is a non-`NULL` item being mapped by `key`


```C

int hashmap_delete(hashmap_t* map,void* key)

```

Delete a key  from the hashmap, Returns zero upom sucess and a non zero number indicating failure.


`map` is a non-`NULL` pointer returned by `hashmap_custom()` or `hashmao_new()` 

`key` is a non `NULL` pointer whose assigned value is to be deleted along with the specified key

```C

int hashmap_oom(hashmap_t* map)

```

Returns a non zero number if the last `hashmap_put()`  failed due to 
Out of Memory condition

`map` is a non-`NULL` pointer returned by `hashmap_custom()` or `hashmao_new()` 

```C

void hashmap_destroy(hashmap_t* map)

```

Releases the resources held by the given hashmap, the hashmap becomes a dangling pointer afterwards

`map` is a pointer returned by `hashmap_custom()` or `hashmap_new()` which can be `NULL`

## Lists

The lists in this repository are implemented as a cursor based doubly linked list.
For source code; see [here](list.c) 

```C

list_t* list_custom(size_t obj_size, void* (*xmalloc)(size_t), 
                    void* (*xrealloc)(void*, size_t), void (*xfree)(void*))

```

Returns a custom linked list

`obj_size` is the size of object/item/element in bytes; basically `sizeof(object)`

See [xmalloc](#xmalloc) [xrealloc](#xrelloc) and [xfree](#xfree).


```C

list_t* list_new(size_t obj_size)

```

Thin wrapper around `list_custom()` ; similar of calling `list_custom(obj_size,malloc, realloc, free)`

`obj_size` is the size of object in bytes ; basically `sizeof(object)` 

```C

void* list_get(list_t* list,size_t idx)

```
Return an item from the list at specified index. returns `NULL` upon failure.

`list` is a non-`NULL` pointer returned by `list_custom()` or `list_new()`

`idx` is the index of requested item.

```C

int list_put(list_t* list,size_t idx,void* obj)

```
Update an item at specified index, returns zero indicating succes or
a non zero number indicating failure


`list` is a non-`NULL` pointer returned by `list_custom()` or `list_new()`

`idx` is the index that will be updated.


```C

int list_update(list_t* list,size_t idx,void* obj)

```
Similar of `list_put()`
```C

int list_delete(list_t* list,size_t idx)

```
Delete an item at specified index, returns zero indicating succes or
a non zero number indicating failure


`list` is a non-`NULL` pointer returned by `list_custom()` or `list_new()`

`idx` is the index that will be updated.

```C

void list_destroy(list_t* list)

```

Release the resources held by list

`list` is a pointer which is returned by `list_custom()` or `list_new()` which can be `NULL`

## ND-Arrays

Ever heard of `numpy` ? If not, no issues; and if yes, then I guess such a person shall know what a ND-Array is.
This data structure is basically C's `type_name name[a][b][c]...` which got abstracted.

Almost every data structure kept in this repository grows dynamically; but this one is an _exception_ (it doesn't).


```C

ndarray_t* ndarray_custom(void* (*xmalloc)(size_t), void (*xfree)(void*),size_t obj_size,size_t ndims,...)

```

Returns a custom nd-array whose internal memory is initalized to zero. 
Returns `NULL` upon failure.

`obj_size` is the size of object.

`ndims` is the number of dimensions this nd-array will have, 
this parameter gets stored internally in the nd-array's meta data and 
in future; calls to the specified nd-array,
 **the  `ndarray_` prefixed functions expect exactly `ndims` arguments
to access, or modify the nd-array; not doing so may lead to undefined behavior**

Now the expression `...` is a variadic argument.

Pass exactly `ndims` arguments after `ndims` parameter to avoid undefined behaviour.


See [xmalloc](#xmallox) and [xfree](#xfree)

```C

ndarray_t* ndarray_new(size_t obj_size,size_t ndims,...)

```

`obj_size` is the size of object.

`ndims` is the number of dimensions this nd-array will have, 
this parameter gets stored internally in the nd-array's meta data and 
in future; calls to the specified nd-array,
 **the  `ndarray_` prefixed functions expect exactly `ndims` arguments
to access, or modify the nd-array; not doing so may lead to undefined behavior**

Now the expression `...` is a variadic argument.

Pass exactly `ndims` arguments after `ndims` parameter to avoid undefined behaviour.



```C

void* ndarray_get(ndarray_t* array,...)

```
```C

int ndarray_put(ndarray_t* a,void* obj,...)

```


```C

void ndarray_destroy(ndarray_t* a)

```


## Work in Progress...


