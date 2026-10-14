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
	size_t tail;
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
	if(!nodes){free(l); return NULL;}
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

// Under development: Dynamic growth not added
static node_t* get_node_at(list_t* list,size_t idx){
	if (idx>=list->used) return NULL;
	node_t* curr=list->nodes;
	size_t padding=sizeof(node_t)+list->obj_size;
	while(idx--){
		if(curr->next==NULL_NODE){ return NULL;}
		curr= (node_t*)((char*)list->nodes + padding*curr->next);
	}
	return curr;
}
static node_t* get_node(list_t* list){
	size_t padding=sizeof(node_t)+list->obj_size;
	node_t* tail=get_node_at(list,list->tail);
	char* mem= (char*)list->nodes;
	for(size_t idx=0;idx<list->used;idx++){
		node_t* node=(node_t*)(mem+padding*idx);
		if(node->next==NULL_NODE && node!=tail){ return node;}
	}
	list->used++;
	return (node_t*)(mem+padding*list->used);
}


void* list_get(list_t* list,size_t idx){
	node_t* n=get_node_at(list,idx);
	return n ? n->buff : NULL; // return null if not found.
}
int list_put(list_t* list,size_t idx,void* obj){
	char* mem=list_get(list,idx);
	if(!mem){
		return 1; // Under development.
	};
	memcpy(mem,obj,list->obj_size);
	return 0;
}
