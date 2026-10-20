# Contributing to dtypes

Thank you for your intrest in dtypes.

Let's keep this simple, small, human and a little honest.

You can call this a piece of hobby code if you wish to.

This repository was meant to provide a collection  of common data structures used in day to day programing 

so that C programmers don't have to roam repository to repository in search of a new data structure.

And I admit that this code base has some serious issues and was never meant for the production . 

## Real talk

I am maintaing this repository for a long time. and as the repository is getting bigger

it is getting difficult for me to find all the bugs and fix them one by one. 

And I seriously need  contribution to maintain this well.

This was never meant for the production usage... but can be if you wish to contribute. this can mean for the production usage.


This repository is simple as fuck.

It doesn't needs a `Makefile` or a `build.sh` if you know how to create a shared or static library.

Although they just exist for convinience.

And as we are talking about simplicity.

The API is consistent too.

For example:

if there exists a file `list.c` or `hashmap.c` or `blah.c` it will depend upon `list.h` or `hashmap.h` or `blah.h`

respectively (exceptions exists if it is an utility of the repository e.g. [hash\_function.c](hash_function.c "Source file of hash_function.c in this repo") ).

and if the file is `list.c` it will expose `list_t` ,  `list_custom` , `list_new` and `list_destroy` as if it were a contract to God;

and more `list_*` functions as operations in it.


### What you should do and what I expect from you.

I expect that you will help me fixing the existing bugs in this repo.

or add a new data structure to the repo which is common and this repository 

lacks.

### If you are adding a new data structure to the repository.

I expect that you will give them in two files.

For example:

Let's assume that you want to submit `wheel` data structure 

It shall come in two files `wheel.h` and `wheel.c` and must expose

`wheel_custom`, `wheel_new` and `wheel_destroy` important functions

Plus it should expose other `wheel_*` functions as a operations on it.

like `wheel_rotate` as an hypothetical example.

### If you are fixing any code that you consider is bug.


Before you fix the bug. please check that `Is someone already fixing it?`

And if not

Do the following steps.

#### 1.
Clone the repository into your machine.


Compile the binary

<!--via 
```bash

# The library is simple and doesn't need a 4 million dollar compilation setup
gcc -Wall -Wextra -I..  -c *.c 

ar rcs libdtypes.a *.o
```
or

```bash
gcc  -Wall -Wextra -shared *.c -o "<your sensible and convinent library name>"

```

which ever is most suitable to you.
-->
#### 2.

Write a test condition which can reproduce the bug.

Does it happen? if yes follow the last step.

#### 3.

Open a pull request explaining what is the bug with the code 

that reproduces that specific bug found;

along with the solution i.e which additions or removal of code will fix it.

Please note: If it is a very serious vulnerablity rather than a bug

For example, buffer overflow or remote code execution by some how.

Open a pull request , Say it is a vulnerablity rather than a bug.

Give us your e-mail address, keep a hot patch ready, we will contact you back

for the hot patch and we will talk how can we fix that vulnerablity.

to how to fix it.
<!--
I expect you know what are the consequenses of removing or adding code to it

and why those lines of code exist or do not exist.

Before you fix the bug. please check that `Is someone already fixing it?`

And if not please consider a pull request of `what is the bug?` , `what issue it can cause?` along with the code

you consider will fix it.
-->

#### Why this repository provides two constructors ?

Last but not the least.

This repository provides two constructors 

`*_custom() ` and `*_new()` .

and the neeed of `_custom` is because I didn't wanted to 

add `data_structure_new()` , `data_structure_new_with_custom_allocator()`, `data_structure_with_custom_allocator_and_seed()`

`data_structure_new_with_custom_allocator_and_preserved_space_and_alien_communication_support()` and blah blah blah.

Instead of that it all gets compressed into `*_custom`, wether we are talking about custom allocators and dellocators,

preserved space, hash functions callbacks and everything customizable.


In case user calls `*_new()` the code selects default things passes it to `*_custom` function and returns

what `*_custom` returned. 

And in case the user calls `*_custom`

user is basically switching to `sudo -s` in this repository's language.

If user said it, the user means it. 

The `*_custom` function doesn't gives a fuck 

if someone passed `NULL` to the parameter `xmalloc`, now only C compiler and God knows

how will a `NULL` pointer allocate memory.

To sum up this section: The `*_custom` function gives total control of the data structure to the caller so 

that they can trigger a segfault in 10 way(s) :joy:. 

And if you wish to add such data structures which are extremely customizable;

instead of creating 10 diffrent functions for 10 diffrent support; 

Please compress them to one `*_custom` function.

#### Thank you for reading this lecture. I wish that you will contribute to this repository someday.
#### or use it in your personal projects (please don't use it in production without fixing the existing bugs,
#### otherwise, your boss or senior may hate you).
