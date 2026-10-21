#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include "dtypes/list.h"
#define NULL_NODE ((size_t)-1)


// This implementation of list is barely conventional linked list.
// 


typedef struct{
	size_t next;
	size_t prev;
	char buff[];
}node_t;
typedef struct __list{
	void* (*malloc)(size_t);
	void* (*realloc)(void*, size_t);
	void (*free)(void*);
	size_t obj_size;
	size_t used;
	size_t cap;
	size_t tail_physical;
	int is_full;
	size_t length;
	size_t last_access_logical;
	size_t last_access_physical;
	node_t* nodes;
}list_t;

// Create a customized list .
list_t* list_custom(size_t obj_size, void* (*xmalloc)(size_t), void* (*xrealloc)(void*, size_t), void (*xfree)(void*) ){
	size_t capacity=8;
	size_t mem_required=(sizeof(node_t)+obj_size)*capacity;
	list_t* l=xmalloc(sizeof *l);
	if(!l) return l;
	memset(l,0,sizeof *l);
	node_t* nodes=xmalloc(mem_required);
	if(!nodes){
		xfree(l); 
		return NULL;
	}
	memset(nodes,255,mem_required);
	l->nodes=nodes;
	l->cap=capacity;
	l->obj_size=obj_size;
	l->malloc=xmalloc;
	l->realloc=xrealloc;
	l->free=xfree;
	return l;
}

// Delegates to list_custom with default parameters
list_t* list_new(size_t obj_size){
	return list_custom(obj_size,malloc,realloc,free);
}
// Destroy the list.
void list_destroy(list_t* l){
	if(!l) return ;
	l->free(l->nodes);
	l->free(l);
}

// Well. this won't hack your computer.
//
// this just takes advantage of recorded last access.
//
// as we have a contigious allocation and we are iterating over
// a linked list. this will calculate the  distance and traverse
// them only... rather than traversing the whole linked list which
// is slower.
// Note: this isn't idiot proof. and isn't meant for the public API
static node_t* exploit_last_access(list_t* list,size_t idx){
	size_t distance= idx - list->last_access_logical;

	size_t padding=sizeof(node_t)+ list->obj_size;

	size_t physical_address=list->last_access_physical;
	size_t logical_address=list->last_access_logical;

	node_t* curr= (node_t*)((char*)list->nodes + padding*physical_address);
	while(distance--){
		if(curr->next==NULL_NODE){
			return NULL;
		}
		physical_address=curr->next;
		logical_address++;
		curr= (node_t*)((char*)list->nodes + padding*physical_address);
	}
	list->last_access_logical=logical_address;
	list->last_access_physical=physical_address;
	return curr;
}

// Logically traverse the list.
static node_t* get_node_at(list_t* list,size_t idx){
	if (idx>=list->length) return NULL;
	node_t* curr=list->nodes;

	size_t padding=sizeof(node_t)+list->obj_size;
	if(idx>=list->last_access_logical){
		return exploit_last_access(list,idx);

	}
	size_t physical_address=0;
	size_t logical_address=0;
	while(idx--){
		if(curr->next==NULL_NODE){ 
			return NULL;
		}
		physical_address=curr->next;
		logical_address++;
		curr= (node_t*)((char*)list->nodes + padding*physical_address);
	}
	list->last_access_logical=logical_address;
	list->last_access_physical=physical_address;
	return curr;
}


// Grow the list by 4 times of it's original size
// (usually when full).

int list_grow(list_t* list){
	size_t new_cap= list->cap*4;
	if(new_cap < list->cap){
		return 1;
	}

	size_t padding= sizeof(node_t)+ list->obj_size;
	if(SIZE_MAX/padding < new_cap) {
		return 1;
	}

	size_t mem_required=padding*new_cap;

	node_t* new_nodes=list->realloc(list->nodes,mem_required);
	if(!new_nodes){
		return 1;
	}

	memset((char*)new_nodes+padding*list->cap, 255, padding*(new_cap-list->cap));
	list->cap=new_cap;
	list->nodes=new_nodes;
	return 0;
}



// This function will return a free node from the list 
// Incase the list is full it will create new ones and return it.
static node_t* get_node(list_t* list){
	size_t padding=sizeof(node_t)+list->obj_size;
	if(list->is_full){
		goto create_node;
	}
	node_t* tail=(node_t*)((char*)list->nodes + padding*list->tail_physical);
	for(size_t idx=0;idx<list->used;idx++){
		node_t* node=(node_t*)((char*)list->nodes+padding*idx);
		if(node->next==NULL_NODE && node!=tail){
			return node;
		}
	}
	list->is_full=1;
create_node:
	if(list->used>=list->cap){
		if(list_grow(list)!=0){
			return NULL;
		}
	}
	node_t* free_node=(node_t*)((char*)list->nodes+padding*list->used);
	list->used++;
	return free_node;
}

// Return an internal pointer pointing to the object
//
// stored at idx, returns NULL upon failure
void* list_get(list_t* list,size_t idx){
	node_t* node=get_node_at(list,idx);
	if(!node) return NULL;
	return node->buff;
}
// Helper function 
//
// Calculate distance from b to a (usually root of the array).
//
static size_t calculate_distance(void* a,void *b, size_t padding){
	size_t distance= (size_t)((char*)b - (char*)a);

	return distance/padding;
}

// Append an item to list
int list_append(list_t* list,void* obj){
	size_t padding=sizeof(node_t)+list->obj_size;

	node_t* fresh_node=get_node(list);
	if(!fresh_node){
		return 1;
	}

	node_t* tail= (node_t*) ((char*)list->nodes + padding*list->tail_physical);

	size_t new_tail=calculate_distance(list->nodes,fresh_node,padding);

	fresh_node->prev=list->tail_physical;

	tail->next=new_tail;

	list->tail_physical=tail->next;
	memcpy(fresh_node->buff,obj,list->obj_size);
	list->length++;
	return 0;
}

// Update a item on list
int list_put(list_t* list,size_t idx,void* obj){
	node_t* node=get_node_at(list,idx);

	if(!node) return 1;
	memcpy(node->buff,obj,list->obj_size);
	return 0;
}

// Similar of list_put()
int list_update(list_t* list,size_t idx,void* obj){
	return list_put(list,idx,obj);
}

// Delete an item from list.
int list_delete(list_t* list,size_t idx){
	if(idx>=list->length){
		return 1;
	}
	size_t padding=sizeof(node_t)+list->obj_size;
	if(idx==0){
		node_t* next_node=get_node_at(list,1);
		if(!next_node){
			memset(list->nodes,255, padding);
			list->length--;
			return 0;
		}

		memmove(list->nodes,next_node,padding);
		memset(next_node,255,padding);
		list->is_full=0;
		list->last_access_logical=0;
		list->last_access_physical=0;
		list->length--;
		return 0;
	}
	node_t* this_node=get_node_at(list,idx);
	node_t* prev=(node_t*)((char*)list->nodes + this_node->prev*padding);
	node_t* tail=(node_t*)((char*)list->nodes + list->tail_physical*padding);

	prev->next=this_node->next;
	if(this_node->next!=NULL_NODE){
		node_t* next_node=(node_t*)((char*)list->nodes + padding * this_node->next);
		next_node->prev=this_node->prev;
	}
	if(tail==this_node){

		list->tail_physical=this_node->prev;
	}

	list->is_full=0;
	list->last_access_logical=idx-1;
	list->last_access_physical=this_node->prev;	
	list->length--;
	memset(this_node, 255, padding);
	return 0;
}

 /*
int debug_list(list_t* list){
        node_t* curr=list->nodes;

        printf("Object size: %zu\n",list->obj_size);

        size_t logical_address=0;
        size_t physical_address=0;
        size_t padding=sizeof(node_t)+list->obj_size;
        for(size_t i=0;i<list->used;i++){

                size_t* got=(size_t*)curr->buff;
                printf("-----------\nLogical address: %zu\n"
                                "Physical address: %zu\n"
                                "Object stored: %zu\n",
                                logical_address,
                                physical_address,
                                *got
                      );
		logical_address++;
		physical_address=curr->next;
		if(physical_address==NULL_NODE)
			break;
                curr= (node_t*)((char*)list->nodes +padding*physical_address);
        }
        return 0;
}*/
