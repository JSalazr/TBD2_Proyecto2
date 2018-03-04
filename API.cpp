#include "API.h"
#include <stdlib.h>
#include <cstring>
#include <math.h>

#define BLOCK_SIZE 4096
#define SIZE_DIR 0
#define TABLES_TABLE_DIR 4
#define NUMBER_OF_TABLES 8
#define NUMBER_OF_BLOCKS 12

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
    fs.seekp(BLOCK_SIZE, ios_base::beg);
    fs.read(buffer, BLOCK_SIZE);
    int pos_block = pos / 8;
    int bit_in_char = pos % 8;
    char byte_to_edit = buffer[pos_block];
    byte_to_edit ^= 1UL << bit_in_char;
    memcpy(&buffer[pos_block], &byte_to_edit, 1);
    fs.seekp(BLOCK_SIZE, ios_base::beg);
    fs.write(buffer, BLOCK_SIZE);
    fs.close();
}

int find_next_empty_block(char* name){
    std::fstream fs (name, std::ios::in | std::ios::out);
    clean_buffer();
    fs.read(buffer, BLOCK_SIZE);
    int number_of_blocks;
    memcpy(&number_of_blocks, &buffer[NUMBER_OF_BLOCKS], 4);
    clean_buffer();
    fs.seekp(BLOCK_SIZE, ios_base::beg);
    fs.read(buffer, BLOCK_SIZE);
    for(int c = 0; c < number_of_blocks / 8; c++){
        char byte_to_check = buffer[c];
        for(int a = 0; a < 8; a++){
            int check = 1 << a;
            if((byte_to_check & check) == 0){
                fs.close();
                return c * 8 + a;
            }
        }
    }
    return 0;
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
    ofs.seekp(size - BLOCK_SIZE, ios_base::beg);
    ofs.write("", 1);
    ofs.close();
}

void format_database(int size, char* name){
    int number_of_blocks = size / BLOCK_SIZE;
    float blocks_for_bitmap = ceil(float(number_of_blocks) / float(BLOCK_SIZE));
    blocks_for_bitmap += 1;
    int number_of_tables = 0;
    clean_buffer();
    memcpy(&buffer[SIZE_DIR], &size, 4);
    memcpy(&buffer[TABLES_TABLE_DIR], &blocks_for_bitmap, 4);
    memcpy(&buffer[NUMBER_OF_TABLES], &number_of_tables, 4);
    memcpy(&buffer[NUMBER_OF_BLOCKS], &number_of_blocks, 4);
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

void show_tables(char* name){
    std::fstream fs (name, std::ios::in | std::ios::out);
    clean_buffer();
    fs.seekp(BLOCK_SIZE * 2, ios_base::beg);
    fs.read(buffer, BLOCK_SIZE);

    struct TableInfo *tables = (struct TableInfo*)malloc(sizeof(struct TableInfo));
    int pos = 0;
    while(pos * sizeof(struct TableInfo) < BLOCK_SIZE){
        memcpy(tables, &buffer[pos*sizeof(struct TableInfo)], sizeof(struct TableInfo));
        if(tables->used){
            cout << tables->table_name << endl;
        }
        pos++;
    }
    fs.close();
}

void create_table(char* name, char* table_name, char* columns, char* types, char* key){
    char* column_names;
    column_names = strtok(columns, "=");
    column_names = strtok(NULL, "=");
    char* column_types;
    column_types = strtok(types, "=");
    column_types = strtok(NULL, "=");
    char* prim_key;
    prim_key = strtok(key, "=");
    prim_key = strtok(NULL, "=");

    struct Column *columns_info[15];
    char *column, *type;
    column = strtok(column_names, ",");

    int block_for_table_metadata = find_next_empty_block(name);
    toggle_bit(block_for_table_metadata, name);

    int start_of_data = find_next_empty_block(name);
    toggle_bit(start_of_data, name);
    
    int c = 0;
    while(column != NULL){
        columns_info[c] = (struct Column*)malloc(sizeof(struct Column));
        strcpy(&columns_info[c]->column_name[0], column);
        column = strtok(NULL, ",");
        c++;
    }

    clean_buffer();
    int size_of_register = 0;
    type = strtok(column_types, ",");
    for(int a=0; a < c; a++){
        if(!strcmp(type, "int")){
            columns_info[a]->type = 'i';
            int column_size = 4;
            memcpy(&columns_info[a]->pos_in_register, &size_of_register, 4);
            size_of_register += 4;
            memcpy(&columns_info[a]->size, &column_size, 4);
        }else{
            columns_info[a]->type = 'v';
            char* temp;
            temp = strtok(type, "(");
            temp = strtok(NULL, ")");
            int column_size = atoi(temp);
            memcpy(&columns_info[a]->pos_in_register, &size_of_register, 4);
            size_of_register += atoi(temp);
            memcpy(&columns_info[a]->size, &column_size, 4);
        }
        if(!strcmp(columns_info[a]->column_name, prim_key)){
            columns_info[a]->primary_key = 1;
        }else{
            columns_info[a]->primary_key = 0;
        }
        type = strtok(NULL, ",");
        columns_info[a]->used = 1;
        memcpy(&buffer[8 + sizeof(struct Column) * a], columns_info[a], sizeof(struct Column));
    }

    memcpy(&buffer[0], &size_of_register, 4);
    size_of_register = 0;
    memcpy(&buffer[4], &size_of_register, 4);

    memcpy(&buffer[4090], &start_of_data, 4);

    std::fstream fs (name, std::ios::in | std::ios::out);
    fs.seekp(BLOCK_SIZE * block_for_table_metadata, ios::beg);
    fs.write(buffer, BLOCK_SIZE);

    clean_buffer();
    fs.seekp(BLOCK_SIZE * 2,ios_base::beg);
    fs.read(buffer, BLOCK_SIZE);
    struct TableInfo *tables = (struct TableInfo*)malloc(sizeof(struct TableInfo));
    int pos = 0;
    memcpy(tables, &buffer, sizeof(struct TableInfo));
    pos++;
    while(tables->used){
        memcpy(tables, &buffer[pos*sizeof(struct TableInfo)], sizeof(struct TableInfo));
        pos++;
    }
    pos--;
    tables = (struct TableInfo*)malloc(sizeof(struct TableInfo));
    tables->used = 1;
    strcpy(tables->table_name, table_name);
    tables->metadata_block = block_for_table_metadata;
    memcpy(&buffer[pos*sizeof(struct TableInfo)], tables, sizeof(struct TableInfo));

    fs.seekp(BLOCK_SIZE * 2, ios::beg);
    fs.write(buffer, BLOCK_SIZE);
    fs.close();    
}

void insert_register(char* name, char* table_name, char* columns, char* values){
    char* column_names;
    column_names = strtok(columns, "=");
    column_names = strtok(NULL, "=");
    char* column_values;
    column_values = strtok(values, "=");
    column_values = strtok(NULL, "=");

    clean_buffer();
    std::fstream fs (name, std::ios::in | std::ios::out);
    fs.seekp(BLOCK_SIZE * 2, ios::beg);
    fs.read(buffer, BLOCK_SIZE);
    struct TableInfo *tables = (struct TableInfo*)malloc(sizeof(struct TableInfo));
    int c = 0;
    int metadata_block;
    memcpy(tables, &buffer[c * sizeof(struct TableInfo)], sizeof(struct TableInfo));
    while(tables->used){
        if(!strcmp(tables->table_name, table_name)){
            metadata_block = tables->metadata_block;
            break;
        }
        c++;
        memcpy(tables, &buffer[c * sizeof(struct TableInfo)], sizeof(struct TableInfo));
    }

    clean_buffer();
    fs.seekp(BLOCK_SIZE * metadata_block, ios::beg);
    fs.read(buffer, BLOCK_SIZE);

    int register_size;
    int number_of_registers;

    memcpy(&register_size, &buffer[0], 4);
    memcpy(&number_of_registers, &buffer[4], 4);

    char register_to_add[register_size];

    char* column;
    column = strtok(column_names, ",");
    c = 0;
    struct Column columns1[15];
    while(column != NULL){
        memcpy(&columns1[c], &buffer[8 + c * sizeof(struct Column)], sizeof(struct Column));
        bool found = false;
        while(columns1[c].used){
            if(!strcmp(columns1[c].column_name, column)){
                found = true;
                break;
            }
            c++;
            memcpy(&columns1[c], &buffer[8 + c * sizeof(struct Column)], sizeof(struct Column));
        }
        if(!found){
            cout << "Columna " << column << " no existe en esta tabla." << endl;
            return;
        }
        column = strtok(NULL, ",");
    }

    char* value;
    value = strtok(column_values, ",");
    for(int a = 0; a <= c; a++){
        if(columns1[a].type == 'i'){
            int val = atoi(value);
            memcpy(&register_to_add[columns1[a].pos_in_register], &val, 4);
        }else if(columns1[a].type == 'v'){
            string temp_str = value;
            if(temp_str.size() <= columns1[a].size){
                strcpy(&register_to_add[columns1[a].pos_in_register], value);
            }else{
                cout << "Varchar de tamano muy largo" << endl;
                return;
            }
        }else{
            cout << "No se puede ingresar a la tabla" << endl;
            return;
        }
        value = strtok(NULL, ",");
    }

    int pos = register_size * number_of_registers;
    int relative_block = pos / 4090;
    int pos_in_block = pos % 4090;
    cout << relative_block << endl;
    int next_block;
    memcpy(&next_block, &buffer[4090], 4);
    clean_buffer();
    fs.seekp(BLOCK_SIZE * next_block, ios::beg);
    fs.read(buffer, BLOCK_SIZE);
    for(int x = 0; x < relative_block; x++){
        memcpy(&next_block, &buffer[4090], 4);
        fs.seekp(BLOCK_SIZE * next_block, ios::beg);
        fs.read(buffer, BLOCK_SIZE);
    }
    memcpy(&buffer[pos_in_block], register_to_add, register_size);
    fs.seekp(BLOCK_SIZE * next_block, ios::beg);
    fs.write(buffer, BLOCK_SIZE);
    fs.close();
}