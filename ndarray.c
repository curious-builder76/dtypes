#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdarg.h>

#include "dtypes/ndarray.h"
typedef struct __ndarray__{
	void (*free)(void*);
	size_t ndims;
	size_t obj_size;
}ndarray_t;


/*
 *   MEMORY  LAYOUT:
 *  +----------------+---------+--------------------+----------------+
 *  | array metadata | strides | dimensions (dims). | array elements | 
 *  +----------------+---------+--------------------+----------------+
*/
size_t* get_strides(ndarray_t* a){
	return (size_t*)((char*)a + sizeof(ndarray_t));
}


size_t* get_dims(ndarray_t* a){
	return (size_t*)((char*)a + sizeof(ndarray_t)+ sizeof(size_t)*a->ndims);
}


// Check out of bound dims
int check_dims(ndarray_t* a,va_list* indices){
	char error=0;


	size_t* dims=get_dims(a);

	for(size_t idx=0;idx<a->ndims;idx++){
		size_t dim=va_arg(*indices,size_t);

		if( dim >= dims[idx]){
			error=1;
			break;
		}
	}

	return error;
}

// COpy dimentions
int copy_dims(ndarray_t* a,size_t ndims, va_list* dims){
	size_t* ndarray_dims=get_dims(a);

	for(size_t idx=0;idx<ndims;idx++){
		size_t dim=va_arg(*dims,size_t);
		
		ndarray_dims[idx]=dim;
	}
	return 0;
}
//         __
// Returns ||n=0,n=n {n}  from an pointer of variadic list
size_t prod_v(size_t n,va_list* dims){
	size_t total=1;

	while(n--)
		total*=va_arg(*dims,size_t);
	
	return total;
}


ndarray_t* ndarray_custom0(void* (*xmalloc)(size_t), void (*xfree)(void*), size_t obj_size,size_t ndims, va_list* dims){

	size_t mem_required=prod_v(ndims,dims);

	mem_required= sizeof(ndarray_t) + 2*sizeof(size_t)*ndims + obj_size*mem_required;
	ndarray_t* array=xmalloc(mem_required);
	if(!array) return NULL;

	memset(array,0,mem_required);

	array->ndims=ndims;
	array->obj_size=obj_size;
	array->free=xfree;


	return array;

}

// Calculate strides and copy it 

void create_strides(ndarray_t* array){
	size_t* ndarray_dims=get_dims(array);
	size_t* ndarray_strides=get_strides(array);
	size_t n=array->ndims;
	size_t stride=1;
	while(n--){
		ndarray_strides[n]=stride;
		stride=ndarray_dims[n]*stride;
	}

	printf("Strides: ");
	for(size_t idx=0;idx<array->ndims;idx++){
		printf("%zu ",ndarray_strides[idx]);
	}
	puts("");

}

ndarray_t* ndarray_custom(void* (*xmalloc)(size_t), void (*xfree)(void*), size_t obj_size, size_t ndims,...){
	va_list dims;
	va_start(dims,ndims);
	ndarray_t* array=ndarray_custom0(xmalloc, xfree, obj_size, ndims, &dims);
	va_end(dims);
	if(!array) return array;

	va_start(dims,ndims);
	copy_dims(array,ndims,&dims);
	va_end(dims);

	create_strides(array);
	return array;
}

ndarray_t* ndarray_new(size_t obj_size,size_t ndims,...){
	va_list dims;

	va_start(dims,ndims);
	ndarray_t* array=ndarray_custom0(malloc,free,obj_size,ndims,&dims);
	va_end(dims);

	if(!array) return array;

	va_start(dims,ndims);
	copy_dims(array,ndims,&dims);
	va_end(dims);

	create_strides(array);

	return array;
}

void ndarray_destroy(ndarray_t* a){
	if(!a)return;
       	a->free(a);
}



void* ndarray_get0(ndarray_t* array,size_t n,va_list* dims){
	size_t* ndarray_strides=get_strides(array);

	size_t idx=0;
	size_t location=ndarray_strides[idx++]*n;

	for(;idx<array->ndims;idx++){
		size_t dim=va_arg(*dims,size_t);
		location=location + dim*ndarray_strides[idx];
	}
	return ((char*)array+ sizeof(ndarray_t) + 2*sizeof(size_t)*array->ndims + location*array->obj_size);
}

void* ndarray_get(ndarray_t* array,size_t n,...){
	va_list dims;

	va_start(dims,n);

	int ret=check_dims(array,&dims);
	
	va_end(dims);

	if(ret) return NULL;

	va_start(dims,n);
	
	void* mem=ndarray_get0(array,n,&dims);
	
	va_end(dims);

	return mem;

}



int ndarray_put(ndarray_t* array,void* src, size_t n,...){
	va_list dims;

	va_start(dims,n);

	int ret=check_dims(array,&dims);
	
	va_end(dims);

	if(ret) return 1;

	va_start(dims,n);
	
	void* mem=ndarray_get0(array,n,&dims);
	
	va_end(dims);

	memcpy(mem,src,array->obj_size);
	return 0;

}


