#include "API.h"
#include <stdlib.h>
#include <cstring>
#include <math.h>

#define BLOCK_SIZE 4096
#define SIZE_DIR 0
#define TABLES_TABLE_DIR 4
#define NUMBER_OF_TABLES 8

using namespace std;

char buffer[BLOCK_SIZE];

void clean_buffer(){
    for(int c = 0; c < BLOCK_SIZE; c++){
        buffer[c] = 0;
    }
}

void toggle_bit(int pos, char* name){
    std::fstream fs (name, std::ios::in | std::ios::out);
    clean_buffer();
    fs.seekp(BLOCK_SIZE);
    fs.read(buffer, BLOCK_SIZE);
    int pos_block = pos / 8;
    int bit_in_char = pos % 8;
    char byte_to_edit = buffer[pos_block];
    byte_to_edit ^= 1UL << bit_in_char;
    memcpy(&buffer[pos_block], &char_to_edit, 1);
    fs.seekp(BLOCK_SIZE);
    fs.write(buffer, BLOCK_SIZE);
    fs.close();
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

int create_database(int size, char* name){
    std::ofstream ofs(name, std::ios::binary | std::ios::out);
    ofs.seekp(size - BLOCK_SIZE);
    ofs.write("", 1);
}

void format_database(int size, char* name){
    int number_of_blocks = size / BLOCK_SIZE;
    float blocks_for_bitmap = ceil(float(number_of_blocks) / float(BLOCK_SIZE));
    blocks_for_bitmap += 1;
    clean_buffer();
    memcpy(&buffer[SIZE_DIR], &size, 4);
    memcpy(&buffer[TABLES_TABLE_DIR], &blocks_for_bitmap, 4);
    std::fstream fs(name, std::ios::in | std::ios::out);
    fs.write(buffer, BLOCK_SIZE);
    fs.close();
    blocks_for_bitmap -= 1;
    toggle_bit(0, name);
    toggle_bit(1, name);
    toggle_bit(2, name);
}

void delete_database(char* name){
    remove(name);
}