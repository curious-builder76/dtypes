
#include<stdint.h>
#include<string.h>
#include<stdlib.h>

#ifdef __HASHSET_DEBUG
#include<stdio.h>
#define hashset_debug(...) do {\
	fprintf(stderr,__VA_ARGS__);\
	fprintf(stderr,"\n");\
}while(0)
#else
#define hashset_debug(...)
#endif

#include "dtypes/hashset.h"

#include "dtypes/hash_function.h"





