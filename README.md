# A collection of simple data structures.

A lightweight collection of generic data structures for C

## Features

- Provides data structures that aren't part of C standard library.
- Designed with minimal dependencies.
- Generic, reusable implementatiions.
- Additional data structures are planned and will be added over time.


## Installation

For Linux and friends:

Install it via

```bash
git clone https://github.com/curious-builder76/dtypes.git dtypes

cd dtypes

# if you simply want to build the library
# run

./build.sh

# if you want to run benchmarks along with building it.
# run

BENCH=1 PURGE=1 ./build.sh

```

The above commands will create a file `libdtypes.a`
on the directory `dtypes`.

and finally

```bash
cd ..
```

Once done; now you have to just include it into your projects and link against it.

Now your project directory shall look something like this.

```
[user@localhost][~/path/to/project] $ tree
.
├── dtypes
│   ├── array.c
│   ├── array.h
│   ├── benchmarks
│   │   ├── array_benchmark.c
│   │   ├── benchmark.txt
│   │   ├── hashmap_benchmark.c
│   │       ...
│   │
│   ├── build.sh
│   ├── hash_function.c
│   ├── hash_function.h
│   ├── hashmap.c
│   ├── ...
│   ├── libdtypes.a
│   ├── ...
│   └── trie.h
└── project.c

```

Example:

```C
// project.c
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "dtypes/hashmap.h"


int main(){
    uint32_t page_size=512; // Page size is the size allocated to one "page" of the map. 
                            // Even if you don't understand what a "page" is;
                            // Just understand that the smaller the page size. the faster the 
                            // lookups and the slower the "put" operations.

    uint32_t key_size=6;   // The hashmap was intended to be generic. so it allows
                           // diffrent sized key value pairs, 
                           // here "key_size" and "value_size" are used for demonstration.
                           // but you are supposed to use sizeof(key) and sizeof(value)
    uint32_t value_size=6;
    hashmap_t* map=hashmap_new( page_size, key_size, value_size, NULL, memcmp);
    if(!map){
        perror("");
        return 1;
    }
    if(hashmap_put(map,"hello","world")!=0){
        perror("");
        return 1;
    }
    char* value=hashmap_get(map,"hello");
    if(value!=NULL){
        printf("Found value: %s\n",value);
     }else{
         printf("Value not found in map.");
      }
      hashmap_destroy(map);
      map=NULL;
      return 0;
}
```
```bash
gcc project.c -o project -O2 -Wall -Wextra -I. -Ldtypes -ldtypes

./project
```

## API Design

The api design was tried to be kept as consistent as possible.

Once you are familiar with it;

I believe you can even write code even in Notepad.

Every header file of this library exposes

functions prefixed that way.

For example:

If you do `#include "dtypes/hashmap.h"`

the API now exposes
` hashmap_t`  and `hashmap_*` functions to your project 

If you do `#include "dtypes/array.h"`

the API now exposes
`array_t` and `array_*` functions to your project

and so on

and every header file consists
of 

```C
*_custom()   // which allows you to fine tune the data structure as much the api allows you to 
             // (including custom allocators and deallocators).
*_new()      // which is made for ease of use; and demands the required parameters only.
*_destroy()  // which releases the resources.
```

## Purpose

So why does this library exists?

This library exists to solve a common problem

every C developer faces once in a life time.

that is searching on web for implementations in C

and their web history looks like

```text

bloom filter implementation C github

trie  implementation in C

trie implementation in C industry grade

hashmap implementation in C 

hashmap implementation in C supporting custom allocators.

...
``` 

and so on.

This library contains all  those data types in one place with the support of custom allocators


## Use Case

Everything at last boils down to one question

`Shall I use this library?`

If you are going to use it for personal projects

Definetely a big YES.

If you are going to use it for real industry grade projects

Probably no. as it still lacks some of the features set by industries.

If you are going to implement it in lower lever hardwares  such as ESP-32 or Raspberry PI.

Definetely no. as the project itself is a little RAM hungry (intentionally).

and those systems may merely contain the ram required by this.

## Pros

- Allocates everything in contigious memory.
- Friendly to CPU cache.
- Easier to install (especially on `*NIX` systems)
- Consistent API.
- Supports custom allocators.

## Cons

- A little RAM hungry
- API is stll evolving.
- Requires dynamic memory allocation
- Trade-offs speed with space.
- Not thread safe.

