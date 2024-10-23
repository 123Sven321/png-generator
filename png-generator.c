#include <stdio.h>

//generates the crc-value
unsigned int crc32(unsigned char *data, size_t size){
    
    //initialization
    unsigned int gen = 0xEDB88320;          //Inverted generator polynomial
    unsigned int crc = 0xFFFFFFFF;          //Initial value and crc-value holder
    unsigned int end = 0xFFFFFFFF;          //End xor value
    
    //iterate over input Array
    for(int i = 0; i < size; i++){
        crc = crc ^ data[i];
        
        for(int j = 0; j < 8; j++){
            if(crc & 1){
                crc = (crc >> 1) ^ gen;
            }else{
                crc = crc >> 1;
            }
        }
        
    }
    return crc ^ end;
}

int main() {
    
    

    return 0;
}