#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include "dtypes/hashmap.h"


typedef struct{
	uint64_t hash;
	unsigned char data[];
}record_t;

typedef struct{
	size_t used;
	size_t min_hash_seen;
	size_t max_hash_seen;
}bucket_t;

typedef struct __hashmap_struct{
	size_t bucket_capacity;
	size_t table_size;
	size_t key_size;
	size_t value_size;
	uint64_t (*hash_function)(void* ,  size_t, void* ); 
	void* (*malloc)(size_t);
	void (*free)(void*);
	int (*compare)(const void*, const void* , size_t  );
	void* user_data;
	bucket_t* table;
}hashmap_t;


static bucket_t* get_bucket0(hashmap_t* map,size_t idx){
	size_t block_size=map->key_size+ map->value_size;

	size_t record_size= sizeof(record_t) + block_size;

	size_t bucket_size= sizeof(bucket_t) + record_size*map->bucket_capacity;

	return (bucket_t*)((char*)map->table + bucket_size*idx);
}


static uint64_t default_hash_function(char* key, size_t size, void* user_data){
	(void)user_data;
	return djb_hash(key, size);
}

hashmap_t* hashmap_custom(size_t table_size,size_t bucket_capacity,size_t key_size, size_t value_size,
		uint64_t (*hash_function)(void*, size_t, void* ),
		int (*compare)(const void*, const void* ,size_t),
		void* (*xmalloc)(size_t),
		void (*xfree)(void*),
		void* user_data)


{
	hashmap_t* map=xmalloc(sizeof(hashmap_t));
	if(!map)
		return NULL;

	memset(map, 0, sizeof *map);
	size_t block_size= key_size+ value_size;

	size_t record_size= sizeof(record_t)+ block_size;

	size_t bucket_size= sizeof(bucket_t) + record_size * bucket_capacity;

	size_t total_memory=bucket_size*table_size;

	bucket_t* table= xmalloc(total_memory);

	if(!table){
		xfree(map);
		return NULL;
	}
	memset(table,0, total_memory);

	map->bucket_capacity=bucket_capacity;
	map->table_size=table_size;
	map->key_size=key_size;
	map->value_size=value_size;
	map->malloc=xmalloc;
	map->free=xfree;
	map->compare=compare;
	map->user_data=user_data;
	map->hash_function=hash_function;
	map->table=table;
	return map;

}

hashmap_t* hashmap_new(size_t key_size, size_t value_size){
	return hashmap_custom( 8, 64, 
			key_size, value_size, 
			default_hash_function, 
			memcmp, malloc, free,
			NULL);

}

void* hashmap_get_data(hashmap_t* map,void* key){
	uint64_t hash=map->hash_function(key,map->key_size,map->user_data);

	size_t bucket_idx= (size_t)hash % map->table_size;
	// printf("hashmap_get(): I am looking for: %zu, (as per hash) %lu\n",bucket_idx,hash); 
	size_t record_size= sizeof(record_t)+ map->key_size+ map->value_size;

	bucket_t* bucket=get_bucket0(map,bucket_idx);

	for(size_t idx=0; idx< bucket->used; idx++){
		size_t distance= sizeof(bucket_t) + record_size*idx;
		record_t* record= (record_t*)((char*)bucket + distance);
		// printf("hash=%lu key=%s value=%s" 
		// "map->compare(my_key, requested_key, map->key_size)=%d\n",
		// record->hash,key,value,map->compare(record->data,key,map->key_size));
		if(record->hash==hash && map->compare(record->data, key, map->key_size)==0){
			return record->data;
		}
	}
	return NULL;
}

void* hashmap_get(hashmap_t* map, void* key){
	char* data= hashmap_get_data(map,key);
	if(!data) return NULL;

	return data+map->key_size;
}



int hashmap_put0(hashmap_t* map, void* key,void* value){

	uint64_t hash=map->hash_function(key,map->key_size,map->user_data);

	size_t casted_hash= (size_t) hash;

	size_t bucket_idx= casted_hash % map->table_size;

	size_t record_size= sizeof(record_t) + map->key_size + map->value_size;

	bucket_t* bucket= get_bucket0(map, bucket_idx);

	if(bucket->used>=map->bucket_capacity){
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

		if(record->hash==hash && 0==map->compare(key, record->data, map->key_size)){
			// Overwrite.
			memcpy(record->data+map->key_size, value, map->value_size);
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
	memcpy(record->data, key, map->key_size);
	memcpy(record->data+map->key_size, value, map->value_size);
	return 0;
}

int hashmap_grow(hashmap_t* map,size_t new_size){

	hashmap_t* map2 = hashmap_custom(
			new_size, map->bucket_capacity, 
			map->key_size, map->value_size, map->hash_function,
			map->compare, map->malloc, map->free,
			map->user_data);
	if(!map2){
		return 1;
	}

	size_t record_size= sizeof(record_t) + map->key_size + map->value_size;

	for(size_t idx=0;idx<map->table_size; idx++){
		bucket_t* bucket=get_bucket0(map,idx);
		for(size_t bucket_idx=0;bucket_idx<bucket->used;bucket_idx++){
			record_t* record = (record_t*)((char*)bucket + sizeof(bucket_t) + record_size* bucket_idx);
			unsigned char* key= record->data;
			unsigned char* value= key + map->key_size;
			if(hashmap_put0(map2,key,value)!=0){
				hashmap_destroy(map2);
				return 1;
			}
		}
	}
	map->free(map->table);
	memcpy(map, map2, sizeof(hashmap_t));
	map2->free(map2);
	return 0;
}


int hashmap_put(hashmap_t* map, void* key, void* value){
	for(size_t idx=0;idx<5;idx++){
		if(hashmap_put0(map, key, value)==0){
			return 0;
		}
		if(hashmap_grow(map, map->table_size*2)!=0){
			return 1;
		}
	}
	return 1;
}
void hashmap_destroy(hashmap_t* map){
	if(!map) return;

	map->free(map->table);
	map->free(map);
}
/*
void* debug_hashmap(hashmap_t* map){
	printf("Hashmap metadata: \n"
			"Key size: %zu\n"
			"Value size: %zu\n"
			"Table size: %zu\n"
			"Bucket capacity(fixed till life time): %zu\n"
			"---------------------------------------\n",
			map->key_size,
			map->value_size,
			map->table_size,
			map->bucket_capacity
			);
	size_t record_size= sizeof(record_t)+ map->key_size+ map->value_size;
	for(size_t bucket_idx=0;bucket_idx<map->table_size; bucket_idx++){
		bucket_t* bucket=get_bucket0(map,bucket_idx);
		printf("bucket %zu: \n",bucket_idx);
		for(size_t idx=0; idx< bucket->used; idx++){
			size_t distance= sizeof(bucket_t) + record_size*idx;
			record_t* record= (record_t*)((char*)bucket + distance);
			unsigned char* key= record->data;
			unsigned char* value= key + map->key_size;
			printf(" key=%s | value=%s |",key,value);
		}
		puts("\n-----------------------------------------");


	}
	return NULL;
}
*/

