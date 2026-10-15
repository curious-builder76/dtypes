#include<string.h>
#include<stdlib.h>
#include<stdint.h>

#include "dtypes/list.h"
#define NULL_NODE ((size_t)-1)

typedef struct{
	size_t next;
	char buff[];
}node_t;

typedef struct __list{
	size_t obj_size;
	size_t used;
	size_t cap;
	size_t tail_physical;
	node_t* nodes;
}list_t;


list_t* list_new(size_t obj_size){
	size_t capacity=1024;
	size_t mem_required=(sizeof(node_t)+obj_size)*
		capacity;
	list_t* l=malloc(sizeof *l);
	if(!l) return l;
	memset(l,0,sizeof *l);
	node_t* nodes=malloc(mem_required);
	if(!nodes){
		free(l); 
		return NULL;
	}
	memset(nodes,255,mem_required);
	l->nodes=nodes;
	l->cap=capacity;
	l->obj_size=obj_size;
	return l;
}
void list_destroy(list_t* l){
	if(!l) return ;
	free(l->nodes);
	free(l);
}

static node_t* get_node_at(list_t* list,size_t idx){
	if (idx>=list->cap) return NULL;
	node_t* curr=list->nodes;
	size_t padding=sizeof(node_t)+list->obj_size;
	while(idx--){
		if(curr->next==NULL_NODE){ return NULL;}
		curr= (node_t*)((char*)list->nodes + padding*curr->next);
	}
	return curr;
}


int list_grow(list_t* list){
	size_t new_cap=list->cap*2;
	if(new_cap < list->cap){
		return 1;
	}

	size_t padding= sizeof(node)+ obj_size;
	if(SIZE_MAX/padding <= new_cap) {
		return 1;
	}

	size_t mem_required=padding*new_cap;

	node_t* new_nodes=realloc(list->nodes,new_cap);
	if(!new_nodes){
		return 1;
	}
	list->cap=new_cap;
	list->nodes=new_nodes;
	return 0;
}



// This function will return a free node from the list or create new ones.
static node_t* get_node(list_t* list){

	size_t padding=sizeof(node_t)+list->obj_size;
	node_t* tail=(node_t*)((char*)list->nodes + padding*list->tail_physical);
	char* mem= (char*)list->nodes;
	for(size_t idx=0;idx<list->used;idx++){
		node_t* node=(node_t*)(mem+padding*idx);
		if(node->next==NULL_NODE && node!=tail){ return node;}
	}
	// Trying to be less RAM hungry
	//
	if(list_grow(list)!=0){
		return NULL;
	}
	return (node_t*)(mem+padding*(list->used++));
}
void* list_get(list_t* list,size_t idx){
	node_t* n=get_node_at(list,idx);
	return n ? n->buff : NULL; // return null if not found.
}
int list_append(list_t* list,void* obj){
	size_t padding=sizeof(node_t)+list->obj_size;

	node_t* tail= (node_t*)((char*)list->nodes+ (padding*list->tail_physical));

	node_t* fresh_node=get_node(list);
	if(!fresh_node){
		return 1;
	}
	size_t distance= (size_t) (list->nodes - fresh_node);

	tail->next=distance;

	memcpy(fresh_node->buff,obj,list->obj_size);

	return 0;
}


