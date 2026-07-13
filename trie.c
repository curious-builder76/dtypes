#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>


// node_t is a tree of nodes bound to trie
// and are internally managed by 
// trie_* and node_* functions

typedef struct __node{
	char node_value;
	uint8_t used;
	uint8_t capacity;
	struct __node* nodes;
}node_t;


// A trie is a forest of nodes
// which allows custom allocators,
// destructors to integrate smoothly/
//
// Nodes are kept in contigious memory 
// and are pre-allocated for enhanced performance
// and better CPU friendliness.

typedef struct __trie{
	void* (*malloc)(size_t);
	void* (*realloc)(void* , size_t);
	void (*free)(void*);
	uint8_t used;
	uint8_t capacity;
	node_t* nodes;
}trie_t;

// Initialize a new node already allocated by trie.

int node_init(trie_t* trie,node_t* node,char ch){
	node->used=0;
	node->capacity=8;
	node->nodes=trie->malloc(sizeof(node_t)*node->capacity);
	if(node->nodes==NULL){
		return 1;
	}

	node->node_value=ch;
	return 0;
}

// Grow the size of node by doubling it's size.

int node_grow(trie_t* trie,node_t* node){
	uint8_t cap_max=(uint8_t)(-1);
	if(node->capacity==cap_max){
		return 0; // Supress error.
			  // Node at its maximum capacity;
	}
	uint8_t old_cap=node->capacity;
	uint8_t new_cap=old_cap*2;
	if(new_cap<old_cap){
		// Overflow.
		new_cap= cap_max;
	}
	node_t* subnodes= trie->realloc(node->nodes,sizeof(node_t)*new_cap);
	if(subnodes==NULL){
		return 1;
	}
	node->nodes=subnodes;
	node->capacity=new_cap;
	return 0;
}

// Insert a string into the tree.

int node_insert(trie_t* trie,node_t* node,char* buff){
	if(node->used>=node->capacity){
		if(node_grow(trie,node)!=0){
			return 1;
		}
	}
	if(*buff=='\0'){
		// node_null()
		node_t* child=node->nodes+node->used;
		node->used++;
		child->used=0;
		child->capacity=0;
		child->nodes=NULL;
		child->node_value=*buff;
		return 0;
	}

	for(uint8_t idx=0;idx<node->used;idx++){
		node_t* child=node->nodes+idx;
		if(child->node_value== *buff){
			return node_insert(trie,child,buff+1);
		}
	}
	// node_push()
	node_t* child=node->nodes+node->used;
	if(node_init(trie,child,*buff)!=0){
		return 1;
	}
	node->used++;
	return node_insert(trie,child,buff+1);
}


// Returns zero if the node contains given "buff"
// and  non zero if not present
int node_contains(node_t* node,char* buff){	
	for(uint8_t idx=0;idx<node->used;idx++){
		node_t* child=node->nodes+idx;
		if(child->node_value!=*buff){
			continue;
		}
		if(child->node_value=='\0'){
			return 1;
		}
		return node_contains(child,buff+1);
		
	}
	return 0;
}

// Release the resources aquired by node.
//
void node_destroy(trie_t* trie,node_t* node){
	for(uint8_t idx=0;idx<node->used;idx++){
		node_t* child=node->nodes+idx;
		node_destroy(trie,child);
	}
	trie->free(node->nodes);
}

// Create a new custom trie with custom allocators and 
// deallocators
trie_t* trie_custom(
		void* (*xmalloc)(size_t),
		void* (*xrealloc)(void*, size_t),
		void  (*xfree)(void*)
		){
	trie_t* root=xmalloc(sizeof(trie_t));
	if(root==NULL)
		return NULL;
	root->used=0;
	root->malloc=xmalloc;
	root->free=xfree;
	root->realloc=xrealloc;
	root->capacity=8;
	root->nodes=xmalloc(sizeof(node_t)*root->capacity);

	if(root->nodes==NULL){
		xfree(root);
		return NULL;
	}
	return root;
}

// Wrapper around trie_custom which uses
// malloc(), realloc(), and free()
// provided by libc.
trie_t* trie_new(){
	return trie_custom(
			malloc,
			realloc,
			free
			);
}


// Grow the trie forest by
// doubling it's size

int trie_grow(trie_t* trie){
	uint8_t max_cap= (uint8_t)(-1);
	if(trie->capacity==max_cap)
		return 0; // Supress error.

	uint8_t old_cap=trie->capacity;
	uint8_t new_cap=old_cap*2;
	if(new_cap<old_cap){
		// Over flow.
		new_cap=max_cap;
	}
	node_t* subnodes=trie->realloc(trie->nodes,sizeof(node_t)*new_cap);
	if(subnodes==NULL)
		return 1;
	trie->nodes=subnodes;
	trie->capacity=new_cap;
	return 0;
}

// Insert a string to the given trie.
int trie_insert(trie_t* root,char* buff){
	if(*buff=='\0'){
		return 0; // Supress error.
	}
	if(root->used>=root->capacity){
		if(trie_grow(root)!=0){
			return 1;
		}
	}
	for(uint8_t idx=0;idx<root->used; idx++){
		node_t* child=root->nodes+idx;
		if(child->node_value==*buff){
			return node_insert(root,child,buff+1);
		}
	}
	// trie_push()
	node_t* child=root->nodes+root->used;
	if(node_init(root,child,*buff)!=0){
		return 1;
	}
	root->used++;
	return node_insert(root,child,buff+1);
}



// Check for membership of string in trie.
// returns zero if the string is not present
// or returns a non zero number if present.

int trie_contains(trie_t* trie,char* buff){
	if(*buff==0){
		return 0;
	}
	for(uint8_t idx=0;idx<trie->used;idx++){
		node_t* child=trie->nodes+idx;
		if(child->node_value==*buff){
			return node_contains(child,buff+1);
		}
	}
	return 0;
}

// Release the resources aquired by trie,
// and it's sub nodes.
void trie_destroy(trie_t* trie){
	for(uint8_t idx=0;idx<trie->used;idx++){
		node_t* child=trie->nodes+idx;
		node_destroy(trie,child);
	}
	trie->free(trie->nodes);
	trie->free(trie);
}
