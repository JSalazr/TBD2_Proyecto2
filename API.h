#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int get_size_in_bytes(int initial_size, char* multiplier);

int create_database(int size, char* name);

void format_database(int size, char* name);

void delete_database(char* name);

int find_next_empty_block(char* name);

void create_table(char* name, char* table_name, char* columns, char* types, char* key);

void insert_register(char* name, char* table_name, char* columns, char* values);

void show_tables(char* name);

struct Column{
    bool used;
    char column_name[50];
    char pos_in_register;
    int size;
    char type;
    char primary_key;
};

struct TableInfo{
    bool used;
    char table_name[50];
    int metadata_block;
};

