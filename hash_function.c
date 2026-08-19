
#include<stdint.h>

#include "dtypes/hash_function.h"

uint32_t djb_hash(void* obj,uint32_t size){
        unsigned char* buff=obj;
        uint32_t hash=5381;
        for(uint32_t i=0;i<size;i++){
                uint32_t c=buff[i];
                hash=(hash<<5)+hash+c;
        }
        return hash;
}

