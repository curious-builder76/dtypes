
#include<stdint.h>

#include "hash_function.h"

uint64_t djb_hash(void* obj,size_t size){
        unsigned char* buff=obj;
        uint64_t hash=5381;
        for(uint64_t i=0;i<size;i++){
                uint64_t c=buff[i];
                hash=(hash<<5)+hash+c;
        }
        return hash;
}

