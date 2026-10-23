#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include "dtypes/hashset.h"
#include "dtypes/hash_function.h"


// This hashset is an adapted version of hashmap
//
// from this repository


// Contains a cached hash and stored object

typedef struct{
	uint64_t hash;
	unsigned char data[];
}record_t;



// Stores bucket metadata 

typedef struct{
	size_t used;
	size_t min_hash_seen;
	size_t max_hash_seen;
}bucket_t;


// Hashset is based upon open addressing
//
//


typedef struct __hashset_struct{
	size_t bucket_capacity;
	size_t table_size;
	size_t key_size;
	int oom_flag;
	uint64_t (*hash_function)(const void* ,  size_t, void* ); 
	void* (*malloc)(size_t);
	void (*free)(void*);
	int (*compare)(const void*, const void* , size_t  );
	void* user_data;
	bucket_t* table;
}hashset_t;

// Helper function;
//
// Returns a bucket from the given set at given idx.
//
// Not idiot proof.
//

static bucket_t* get_bucket0(hashset_t* set,size_t idx){
	size_t block_size=set->key_size;

	size_t record_size= sizeof(record_t) + block_size;

	size_t bucket_size= sizeof(bucket_t) + record_size*set->bucket_capacity;

	return (bucket_t*)((char*)set->table + bucket_size*idx);
}



// Allows fine tuning of a hash set.
// 

hashset_t* hashset_custom(
                size_t table_size,size_t bucket_capacity,
                size_t key_size,
                uint64_t (*hash_function)(const void* , size_t, void* ),
                int (*compare)(const void* , const void* , size_t),
                void* (*xmalloc)(size_t),
                void (*xfree)(void*),
                void* user_data)

{
	hashset_t* set=xmalloc(sizeof(hashset_t));
	if(!set)
		return NULL;

	memset(set, 0, sizeof *set);
	size_t block_size= key_size;

	size_t record_size= sizeof(record_t)+ block_size;

	size_t bucket_size= sizeof(bucket_t) + record_size * bucket_capacity;

	size_t total_memory=bucket_size*table_size;

	bucket_t* table= xmalloc(total_memory);

	if(!table){
		xfree(set);
		return NULL;
	}
	memset(table,0, total_memory);

	set->bucket_capacity=bucket_capacity;
	set->table_size=table_size;
	set->key_size=key_size;
	set->malloc=xmalloc;
	set->free=xfree;
	set->compare=compare;
	set->user_data=user_data;
	set->hash_function=hash_function;
	set->table=table;
	return set;

}


// Delegates to hashset_custom with default parameters
hashset_t* hashset_new(size_t key_size){
	return hashset_custom( 8, 64, 
			key_size, 
			default_hash_function, 
			memcmp, malloc, free,
			NULL);

}


// Destroy an hashset and release it's resources

void hashset_destroy(hashset_t* set){
	if(!set) return;

	set->free(set->table);
	set->free(set);
}

// Helper function for hashset_custom
void* hashset_get_data(hashset_t* set,void* key){
	uint64_t hash=set->hash_function(key,set->key_size,set->user_data);

	size_t bucket_idx= (size_t)hash % set->table_size;
	// printf("hashset_get(): I am looking for: %zu, (as per hash) %lu\n",bucket_idx,hash); 
	size_t record_size= sizeof(record_t)+ set->key_size;

	bucket_t* bucket=get_bucket0(set,bucket_idx);

	for(size_t idx=0; idx< bucket->used; idx++){
		size_t distance= sizeof(bucket_t) + record_size*idx;
		record_t* record= (record_t*)((char*)bucket + distance);
		// printf("hash=%lu key=%s value=%s" 
		// "set->compare(my_key, requested_key, set->key_size)=%d\n",
		// record->hash,key,value,set->compare(record->data,key,set->key_size));
		if(record->hash==hash && set->compare(record->data, key, set->key_size)==0){
			return record->data;
		}
	}
	return NULL;
}

// Returns 1 if the provided object is present in given
// hashset else returns 0
int hashset_contains(hashset_t* set, void* obj){
	char* data=hashset_get_data(set, obj);
	if(data!=NULL) return 1;
	return 0;

}


// Helper for hashset_put.
//
// Attempts to insert an item, 
// returns 0 upon sucess 
// and a non zero number indicating failure
//
int hashset_put0(hashset_t* set, uint64_t hash, void* key){


	size_t casted_hash= (size_t) hash;

	size_t bucket_idx= casted_hash % set->table_size;

	size_t record_size= sizeof(record_t) + set->key_size;

	bucket_t* bucket= get_bucket0(set, bucket_idx);

	if(bucket->used>=set->bucket_capacity){
		return 1;
	}

	size_t min_hash=bucket->min_hash_seen;
	size_t max_hash=bucket->max_hash_seen;

	// This fellow techinque works on the belief that 
	// if a hash function  say H(x) returns 10x 
	// then in future it will also return 10X for the object x
	//
	// If 10x is less than min or more than max, means we have never seen that
	//
	// this comes handy when the table is too big and empty.
	//
	//
	// So it skips the hard work and takes a short cut. that's it. :-)
	//
	//
	//
	if( min_hash > casted_hash){
		// printf("Yes, this dumb optimization works. (min_hash hit)!\n");
		min_hash=casted_hash;
		goto insert_new;
	}
	if( max_hash < casted_hash){
		// printf("Yes, this dumb optimization works.(max_hash hit)!\n");
		max_hash=casted_hash;
		goto insert_new;
	}

	for(size_t idx=0;idx<bucket->used;idx++){
		size_t distance=sizeof(bucket_t) + record_size*idx;

		record_t* record= (record_t*)((char*)bucket + distance);

		if(record->hash==hash && 0==set->compare(key, record->data, set->key_size)){
			// Early return
			return 0;
		}
	}

insert_new:
	// Insert a new one.
	bucket->max_hash_seen=max_hash;
	bucket->min_hash_seen=min_hash;
	size_t distance= sizeof(bucket_t) + record_size*bucket->used;
	bucket->used++;
	record_t* record = (record_t*)((char*)bucket + distance);
	record->hash=hash;
	memcpy(record->data, key, set->key_size);
	return 0;
}


// Delete an item from a given set

int hashset_delete(hashset_t* set, void* key){
	uint64_t hash=set->hash_function(key, set->key_size, set->user_data);

	size_t hash_casted=(size_t)hash;

	size_t bucket_idx= hash % set->table_size;

	size_t record_size=sizeof(record_t) + set->key_size;

	bucket_t* bucket=get_bucket0(set,bucket_idx);

	if(!bucket->used) return 1;
	
	if(bucket->min_hash_seen > hash_casted || bucket->max_hash_seen < hash_casted)
		return 1;
	size_t idx=0;
	char found=0;
	record_t* record;
	for(;idx<bucket->used;idx++){
		record= (record_t*)((char*)bucket + sizeof(bucket_t) + record_size*idx);
		if(record->hash==hash && set->compare(record->data, key, set->key_size)==0){
			found=1;
			break;
		}
	}
	if(!found) return 1;
	bucket->used--;
	record_t* last_record=(record_t*)((char*)bucket + bucket->used*record_size);
	memmove(record, last_record, record_size);

	return 0;

}

// Grow (or resize) the given hashset to new_size
// as it's new table size

int hashset_grow(hashset_t* set,size_t new_size){

	hashset_t* set2 = hashset_custom(
			new_size, set->bucket_capacity, 
			set->key_size, set->hash_function,
			set->compare, set->malloc, set->free,
			set->user_data);
	if(!set2){
		return 1;
	}

	size_t record_size= sizeof(record_t) + set->key_size;

	for(size_t idx=0;idx<set->table_size; idx++){
		bucket_t* bucket=get_bucket0(set,idx);
		for(size_t bucket_idx=0;bucket_idx<bucket->used;bucket_idx++){
			record_t* record = (record_t*)((char*)bucket + sizeof(bucket_t) + record_size* bucket_idx);
			unsigned char* key= record->data;
			if(hashset_put0(set2,record->hash,key)!=0){
				hashset_destroy(set2);
				return 1;
			}
		}
	}
	set->free(set->table);
	memcpy(set, set2, sizeof(hashset_t));
	set2->free(set2);
	return 0;
}

// Returns a non zero number if the last
//
// put operation failed due to 
// Out of memory conditions
int hashset_oom(hashset_t* set){
	return set->oom_flag;
}

// Insert an item to hashset, grow if required.
//
// Returns 0 indicating sucess,
// and a non zero number indicating failure
//

int hashset_put(hashset_t* set, void* key){
	set->oom_flag=0;

	uint64_t hash=set->hash_function(key,set->key_size,set->user_data);
	for(size_t idx=0;idx<5;idx++){
		if(hashset_put0(set, hash,key)==0){
			return 0;
		}
		if(hashset_grow(set, set->table_size*2)!=0){
			return 1;
		}
	}
	set->oom_flag=1;
	return 1;
}

/*
void* debug_hashset(hashset_t* set){
	printf("Hashset metadata: \n"
			"Key size: %zu\n"
			"Value size: %zu\n"
			"Table size: %zu\n"
			"Bucket capacity(fixed till life time): %zu\n"
			"---------------------------------------\n",
			set->key_size,
			set->value_size,
			set->table_size,
			set->bucket_capacity
			);
	size_t record_size= sizeof(record_t)+ set->key_size+ set->value_size;
	for(size_t bucket_idx=0;bucket_idx<set->table_size; bucket_idx++){
		bucket_t* bucket=get_bucket0(set,bucket_idx);
		printf("bucket %zu: \n",bucket_idx);
		for(size_t idx=0; idx< bucket->used; idx++){
			size_t distance= sizeof(bucket_t) + record_size*idx;
			record_t* record= (record_t*)((char*)bucket + distance);
			unsigned char* key= record->data;
			unsigned char* value= key + set->key_size;
			printf(" key=%s | value=%s |",key,value);
		}
		puts("\n-----------------------------------------");


	}
	return NULL;
}
*/

