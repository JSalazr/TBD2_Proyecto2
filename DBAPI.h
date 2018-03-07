#include <stdio.h>
#include <iostream>
#include <fstream>
#include "DiskAPI.h"

using namespace std;

void create_table(char* name, char* table_name, char* columns, char* types, char* key);

void insert_register(char* name, char* table_name, char* columns, char* values);

void show_tables(char* name);

void select_show(char* name, char* table, char* columns, char* where);

void update_register(char* name, char* table, char* columns, char* values, char* where);