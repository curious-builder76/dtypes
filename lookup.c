
/*
 * Experimental data structure, similar to a bloom filter
 * but stores  hashses.
 */

/*
 * If the hash function guarantees that  no hash will collide.
 * then this can also guarantees that there will be neither false positives nor negatives.
 */

#include<string.h>
#include<stdlib.h>
#include<stdint.h>


#include "dtypes/lookup.h"
#include "dtypes/hash_function.h"

// This data structure is an adapted version of 
// hashtable that only stores hashes
// discarding objects


typedef struct{
	size_t used;
	size_t min_hash_seen;
	size_t max_hash_seen;
	uint64_t hashes[];
}bucket_t;

typedef struct __lookup_struct{
	size_t obj_size;
	size_t table_size;
	size_t bucket_capacity;
	uint64_t (*hash_function)(const void* , size_t, void*);
	void* (*malloc)(size_t);
	void (*free)(void*);
	void* user_data;
	int oom_flag;
	bucket_t* table;
}lookup_t;


// Returns a fine tuned lookup structure

lookup_t* lookup_custom(
		size_t table_size, size_t bucket_capacity, size_t obj_size, 
		uint64_t (*hash)(const void* , size_t, void*),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*),
		void* user_data)
{
	lookup_t* look= xmalloc(sizeof(lookup_t));
	if(!look) return look;

	size_t bucket_size= sizeof(bucket_t)+ obj_size*bucket_capacity;

	size_t mem_required= bucket_size*table_size;

	bucket_t* table=xmalloc(mem_required);
	if(!table){
		xfree(look);
		return NULL;
	}

	memset(look, 0, sizeof(lookup_t));

	memset(table, 0, mem_required);

	look->table_size=table_size;
	look->bucket_capacity=bucket_capacity;
	look->obj_size=obj_size;
	look->hash_function=hash;
	look->malloc=xmalloc;
	look->free=xfree;
	look->user_data=user_data;
	look->table=table;
	return look;
}

// Delegates to lookup_custom()
// with default parameters

lookup_t* lookup_new(size_t obj_size){
	return lookup_custom(
			8, 64, obj_size, 
			default_hash_function, malloc, free,
			NULL
			);
			
}


// Return a bucket from the given lookup structure at given idx

static bucket_t* get_bucket0(lookup_t* look, size_t idx){
	size_t bucket_size= sizeof(bucket_t) + look->obj_size*look->bucket_capacity;

	return (bucket_t*)((char*)look->table + bucket_size*idx);
}


// Internal macro signals used by lookup_put0

#define SEEN 2
#define FAILED 1
#define SUCCESS 0

// Helper function for lookup_put
static int lookup_put0(lookup_t* look, uint64_t obj_hash){
	size_t casted_hash= (size_t)obj_hash;

	size_t bucket_idx= casted_hash % look->table_size;

	bucket_t* bucket=get_bucket0(look, bucket_idx);
	if(bucket->used>=look->bucket_capacity){
		return FAILED;
	}

	if(bucket->min_hash_seen > casted_hash){
		bucket->min_hash_seen=casted_hash;
		goto insert_new;
	}
	if(bucket->max_hash_seen < casted_hash){
		bucket->max_hash_seen=casted_hash;
		goto insert_new;
	}

	for(size_t idx=0; idx<bucket->used;idx++){
		uint64_t hash=bucket->hashes[idx];
		if(hash==obj_hash)
			return SEEN;
	}
insert_new:
	size_t insert_idx=bucket->used++;

	uint64_t* hash_ptr=bucket->hashes + insert_idx;

	*hash_ptr=obj_hash;

	return SUCCESS;
}

// Grow (or resize) the lookup structure
// setting new_size as it's new table size

int lookup_grow(lookup_t* look, size_t new_size){
	lookup_t* look2= lookup_custom(
			new_size, look->bucket_capacity,
			look->obj_size, look->hash_function, 
			look->malloc, look->free, look->user_data
			);
	if(!look2) 
		return 1;

	for(size_t bucket_idx=0;bucket_idx<look->table_size; bucket_idx++){
		bucket_t* bucket=get_bucket0(look, bucket_idx);

		for(size_t idx=0;idx<bucket->used; idx++){
			uint64_t hash= bucket->hashes[idx];
			if(lookup_put0(look2, hash)==FAILED){
				lookup_destroy(look2);
				return 1;
			}
		}
	}
	look->free(look->table);
	memcpy(look, look2, sizeof(lookup_t)); 
	look2->free(look2);
	return 0;
}

// Adds the hash (fingerprint) to the lookup structure
//
// Returns zero indicating success and a non zero number
// indicating failure

int lookup_add(lookup_t* look, void* obj){
	look->oom_flag=0;
	uint64_t hash=look->hash_function(obj, look->obj_size, look->user_data);
	for(size_t idx=0;idx<5;idx++){
		if(lookup_put0(look,hash)!=FAILED){
			return 0;
		}
		if(lookup_grow(look,look->table_size*2)!=0){
			return 1;
		}
	}
	look->oom_flag=1;
	return 1;
}

// Returns a non zero number indicating that the last add operation was failed 
// due to Out of Memory conditions
int lookup_oom(lookup_t* look){
	return look->oom_flag;
}

// Returns a non zero number indicating that the structuure is confident 
// enough to say "this object is present" returns zero otherwise
int lookup_contains(lookup_t* look, void* obj){
	uint64_t hash=look->hash_function(obj, look->obj_size, look->user_data);

	size_t hash_casted= (size_t) hash;

	size_t bucket_idx= hash % look->table_size;

	bucket_t* bucket=get_bucket0(look, bucket_idx);

	if(bucket->min_hash_seen > hash_casted) return 0;

	if(bucket->max_hash_seen < hash_casted) return 0;

	for(size_t idx=0;idx<bucket->used; idx++){
		if(bucket->hashes[idx]==hash){
			return 1;
		}
	}
	return 0;
}

// Release the resources acquired by the lookup structure
void lookup_destroy(lookup_t* look){
	if(!look) return;

	look->free(look->table);
	look->free(look);
}
