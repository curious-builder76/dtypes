#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<va_args.h>

typedef struct{
	void (*free)(void*);
	size_t ndims;
	size_t obj_size;
}ndarray_t;


// For sake of "my" convinience.

typedef ndarray_t array_t;

size_t* get_dims(array_t* a){
	return (size_t*)((char*)a + sizeof(array_t));
}

// Check out of bound dims
int check_dims(array_t* a,...){
	char error=0;

	va_list indices;
	
	va_start(indices,a->ndims);

	size_t* dims=get_dims(a);

	for(size_t idx=0;idx<array->ndims;idx++){
		size_t dim=va_arg(indices,idx);

		if( dim >= dims[idx]){
			error=1;
			break;
		}
	}

	va_end(indices);
	return error;
}

int copy_dims(array_t* a,size_t ndims,...){
	va_list dims;
	va_start(dims,ndims);

	size_t* array_dims=get_dims(a);

	for(size_t idx=0;idx<ndims;idx++){
		size_t dim=va_arg(dims,size_t);

		array_dims[idx]=dim;
	}
	va_end(dims);
	return 0;
}

array_t* ndarray_custom(void* (*xmalloc)(size_t), void (*xfree)(void*),size_t obj_size,size_t ndims, ...){
	va_list dims;

	va_start(dims,ndims);

	size_t mem_required=obj_size;

	for(size_t n=0;n<ndims;n++){
		mem_required*=va_arg(dims,size_t);
	}

	va_end(dims);

	mem_required= sizeof(array_t)+ sizeof(size_t)*ndims + mem_required;

	array_t* array=xmalloc(mem_required);
	if(!array){
		return NULL;
	}

	memset(array,0,mem_required);
	array->free=xfree;
	array->ndims=ndims;
	array->obj_size=obj_size;

	copy_dims(array,ndims,...);

	return array;
}



array_t* ndarray_new(size_t obj_size,size_t ndims,...){
	return ndarray_custom(
			malloc,
			free,
			obj_size,
			ndims,
			...
			);
}


void* ndarray_get(ndarray_t* array,...){
	if(check_dims(array,...)){
		return NULL;
	}
	size_t location=0;

	size_t* array_dims=get_dims(array);
	va_list dims;
	va_start(dims,array->ndims);

	for(size_t idx=0;idx<array->ndims;idx++){
		location = location + va_arg(dims,size_t)*array_dims[idx];
	}

	location*=array->obj_size;

	return ((char*)array + array->ndims*sizeof(size_t) +  location);
}

int ndarray_put(array_t* a,void* obj,...){
	void* dst=ndarray_get(a,obj);
	if(!dst) return 1;
	memcpy(dst,obj,a->obj_size);
	return 0;
}
void ndarray_destroy(ndarray_t* a){
	if(!a) return;

	a->free(a);
}
