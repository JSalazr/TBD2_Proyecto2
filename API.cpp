#include "API.h"
#include <stdlib.h>
#include <cstring>
#include <fstream>

#define BLOCK_SIZE 4096

using namespace std;

char buffer[BLOCK_SIZE];

void clean_buffer(){
    for(int c = 0; c < BLOCK_SIZE; c++){
        buffer[c] = 0;
    }
}

int get_size_in_bytes(int initial_size, char* multiplier){
    if(!strcmp(multiplier, "KB")){
        return initial_size * 1024;
    }else if(!strcmp(multiplier, "MB")){
        return initial_size * 1024 * 1024;
    }else if(!strcmp(multiplier, "GB")){
        return initial_size * 1024 * 1024 * 1024;
    }
    return initial_size;
}

int create_file(int size, char* name){
    std::ofstream ofs(name, std::ios::binary | std::ios::out);
    ofs.seekp(size - 1);
    ofs.write("", 1);
}