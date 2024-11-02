#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>
#include "png-generator.h"



//PNG image values
static const int PNG_BIT_DEPTH = 8;      
static const int PNG_COLOR_TYPE = 2;     //RGB
static const int PNG_COMPRESSION = 0;    //zlib
static const int PNG_FILTER = 0;         //no filter
static const int PNG_INTERLACE = 0;      //no interlace
static char *file_name = NULL;

//converts a 32 bit variable to an 8bit array
uint8_t *convert_32bit_to_8bit(uint32_t val){
    static uint8_t bit8_array[4];
    bit8_array[0] = (val >> 24) & 0xFF;
    bit8_array[1] = (val >> 16) & 0xFF;
    bit8_array[2] = (val >> 8) & 0xFF;
    bit8_array[3] = val & 0xFF;

    return bit8_array;
}

//appends an array to another
uint8_t *append_8bit_array(uint8_t *arr1, size_t size1, uint8_t *arr2, size_t size2){
    uint8_t *new_array = (uint8_t *)malloc(size1 + size2);

    for(int i = 0; i < size1 + size2; i++){
        if(i < size1){
            new_array[i] = arr1[i];
        }else{
            new_array[i] = arr2[i - size1];
        }
    }
    return new_array;
}

uint8_t *add_filter(uint8_t *data, size_t width, size_t height) {
    size_t filtered_size = height * (width * 3 + 1);
    uint8_t *filtered_data = (uint8_t *)malloc(filtered_size);

    //fill new array with data + filtertype at beginning of every row
    for(size_t y = 0; y < height; y++){

        filtered_data[y*(width*3+1)] = 0;
        for(size_t x = 0; x < width * 3; x++){
            filtered_data[y*(width*3+1)+x+1] = data[x + y*width*3];
        }
        
    }

    return filtered_data;
}

uint8_t *compress_data(uint8_t *data, size_t size, uLong *compressed_size){

    *compressed_size = compressBound(size);

    uint8_t *output = (uint8_t *)malloc(*compressed_size);                               //compressed data array

    int result = compress(output, compressed_size, (const Bytef *)data, size);

    if (result == Z_OK) {
        printf("png-generator: Compression successful.\n");
        return output;
    } else {
        printf("png-generator: Compression failed: %d\n", result);
        return NULL;
    }
}

//generates the crc-value
uint32_t calculate_crc(uint8_t *data, size_t size){
    
    //initialization
    uint32_t gen = 0xEDB88320;          //inverted generator polynomial
    uint32_t crc = 0xFFFFFFFF;          //initial value and crc-value holder
    uint32_t end = 0xFFFFFFFF;          //final xor value
    
    //calculate crc
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


int write_data_to_file(uint8_t *data, size_t size){

    FILE *file = fopen(file_name, "ab");

    if(file == NULL){
        perror("ERROR: failed to open file\n");
        return 1;
    }

    size_t written = fwrite(data, 1, size, file);

    if(written != size){
        perror("ERROR: failed to write file\n");
        fclose(file);
        return 1;
    } 

    if(fclose(file) != 0){
        perror("ERROR: failed to close file\n");
        return 1;
    }

    return 0;
}





//PNG fragments
int signature(){

    int error_check = 0;

    uint8_t png_signature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    error_check = error_check + write_data_to_file(png_signature, sizeof(png_signature));

    if(error_check != 0){
        return 1;
    }else{
        return 0;
    }
}

int ihdr_chunk(int width, int height){

    int error_check = 0;                                                                 //error value. If != 0 means error

    uint32_t data_length =  0x0000000D;
    uint8_t chunk_type[4] = {0x49, 0x48, 0x44, 0x52};                                    //IHDR
    uint8_t raw_data[13];

    raw_data[0] = (width >> 24) & 0xFF;
    raw_data[1] = (width >> 16) & 0xFF;
    raw_data[2] = (width >> 8) & 0xFF;
    raw_data[3] = width & 0xFF;

    raw_data[4] = (height >> 24) & 0xFF;
    raw_data[5] = (height >> 16) & 0xFF;
    raw_data[6] = (height >> 8) & 0xFF;
    raw_data[7] = height & 0xFF;

    raw_data[8] = PNG_BIT_DEPTH;
    raw_data[9] = PNG_COLOR_TYPE;
    raw_data[10] = PNG_COMPRESSION;
    raw_data[11] = PNG_FILTER;
    raw_data[12] = PNG_INTERLACE;

    uint8_t *crc_raw_data = append_8bit_array(chunk_type, 4, raw_data, 13);
    uint32_t crc = calculate_crc(crc_raw_data, sizeof(chunk_type) + sizeof(raw_data));
    free(crc_raw_data);

    error_check = error_check + write_data_to_file(convert_32bit_to_8bit(data_length), 4); //data length
    error_check = error_check + write_data_to_file(chunk_type, 4);                         //chunk type
    error_check = error_check + write_data_to_file(raw_data, 13);                          //data
    error_check = error_check + write_data_to_file(convert_32bit_to_8bit(crc), 4);         //crc

    if(error_check != 0){
        return 1;
    }else{
        return 0;
    }
}

int idat_chunk(uint8_t *raw_data, size_t size, int width, int height){

    int error_check = 0;

    uint32_t data_length;
    uint8_t chunk_type[] = {0x49, 0x44, 0x41, 0x54};                                     //IDAT

    //Filter data
    uLong filtered_size = (width * 3 + 1) * height;                                      //size after appending filter type
    uint8_t *filt_data = add_filter(raw_data, width, height);                            //filtered data

    //Compress data
    uLong compressed_size;                                                               //size after compression
    uint8_t *comp_data = compress_data(filt_data, filtered_size, &compressed_size);
    if(comp_data == NULL){
        return 1;
    }
    free(filt_data);


    int whole_chunks = compressed_size / MAX_IDAT_SIZE;                                  //calculate number of Chunks
    if(compressed_size % MAX_IDAT_SIZE != 0){
        whole_chunks = whole_chunks + 1;                                                 //add one chunk for rest
    }
    
   
    for(int i = 0; i < whole_chunks; i++){
        
        if(i != whole_chunks - 1){                                                       //if not the last chunk
            
            uint8_t *chunk_array = (uint8_t *)malloc(MAX_IDAT_SIZE);
            data_length = MAX_IDAT_SIZE;
            uint32_t crc;

            for(int j = i * MAX_IDAT_SIZE; j < (i + 1) * MAX_IDAT_SIZE; j++){            //fill array to write in chunk
                chunk_array[j - i * MAX_IDAT_SIZE] = comp_data[j];
            }
            
            uint8_t *crc_raw_data = append_8bit_array(chunk_type, 4, chunk_array, data_length);
            crc = calculate_crc(crc_raw_data, sizeof(chunk_type) + MAX_IDAT_SIZE);
            free(crc_raw_data);

            error_check = error_check + write_data_to_file(convert_32bit_to_8bit(data_length), 4);
            error_check = error_check + write_data_to_file(chunk_type, 4);
            error_check = error_check + write_data_to_file(chunk_array, MAX_IDAT_SIZE);
            error_check = error_check + write_data_to_file(convert_32bit_to_8bit(crc), 4);
            free(chunk_array);

            if(error_check != 0){
                return 1;
            }
            error_check = 0;
        }else{                                                                           //if the last chunk
            
            data_length = compressed_size - i * MAX_IDAT_SIZE;
            uint8_t *chunk_array = (uint8_t *)malloc(data_length);
            uint32_t crc;

            for(int j = i * MAX_IDAT_SIZE; j < compressed_size; j++){                    //fill array to write in chunk
                chunk_array[j - i * MAX_IDAT_SIZE] = comp_data[j];
            }

            uint8_t *crc_raw_data = append_8bit_array(chunk_type, 4, chunk_array, data_length);
            crc = calculate_crc(crc_raw_data, sizeof(chunk_type) + data_length);
            free(crc_raw_data);

            error_check = error_check + write_data_to_file(convert_32bit_to_8bit(data_length), 4);
            error_check = error_check + write_data_to_file(chunk_type, 4);
            error_check = error_check + write_data_to_file(chunk_array, data_length);     
            error_check = error_check + write_data_to_file(convert_32bit_to_8bit(crc), 4);
            free(chunk_array);

            if(error_check != 0){
                return 1;
            }
            error_check = 0;
        }
    }
    free(comp_data);
    return 0;
}

int iend_chunk(){

    int error_check = 0;

    uint32_t data_length = 0;
    uint8_t chunk_type[] = {0x49, 0x45, 0x4E, 0x44};                                     //IHDR
    uint32_t crc = 0xAE426082;

    error_check = error_check + write_data_to_file(convert_32bit_to_8bit(data_length), 4);
    error_check = error_check + write_data_to_file(chunk_type, 4);
    error_check = error_check + write_data_to_file(convert_32bit_to_8bit(crc), 4);

    if(error_check != 0){
        return 1;
    }else{
        return 0;
    }

}

char *find_av_name(){

    uint8_t number = 1;                               //number extension for name

    while(number < 10){

        int size = 0;
        while(file_name[size] != '\0'){               //determine length of string - \0 char 
            size = size + 1;
        }

        char *temp_name = (char *)malloc(size+1+1);   //array for possibly available name
        for(int i = 0; i < size - 4; i++){
            temp_name[i] = file_name[i];
        }

        temp_name[size - 4] = '0' + number;           //extend name with number as char
        temp_name[size - 3] = '.';
        temp_name[size - 2] = 'p';
        temp_name[size - 1] = 'n';
        temp_name[size + 0] = 'g';
        temp_name[size + 1] = '\0';

        number = number + 1;

        //check for existing file with new name
        FILE *file_check = fopen(temp_name, "r");
        if(file_check){
            fclose(file_check);                       //if existing -> next round in loop
        }else{
            return temp_name;                         //if non-existing -> return new name
        }
    }
    printf("png-generator: %s was overwritten.\n", file_name);
    remove(file_name);
    return file_name;                                 
}

int remove_file(){
    if(remove(file_name) == 0) {
        printf("png-generator: File removed.\n");
        return 0;
    }else{
        perror("png-generator: ERROR: Failed to remove file.\n");
        return 1;
    }
}


int generate_png(uint8_t *color_data, size_t color_data_size, int width, int height, char *name){

    file_name = name;                     //set file name

    //check for existing file and adjust name when needed
    FILE *file_check = fopen(file_name, "r");
    if(file_check){
        fclose(file_check);
        printf("png-generator: WARNING: File already exists.\n");
        file_name = find_av_name();
        printf("png-generator: New filename: %s\n", file_name);
    }

    if(signature()){
        printf("png-generator: ERROR: Failed to write signature.\n");
        remove_file();
        return 1;
    }
    if(ihdr_chunk(width, height)){
        printf("png-generator: ERROR: Failed to write IHDR Chunk.\n");
        remove_file();
        return 2;
    }
    if(idat_chunk(color_data, color_data_size, width, height)){
        printf("png-generator: ERROR: Failed to write IDAT Chunk.\n");
        remove_file();
        return 3;
    }
    if(iend_chunk()){
        printf("png-generator: ERROR: Failed to write IEND Chunk.\n");
        remove_file();
        return 4;
    }
    
    return 0;
}
