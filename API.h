#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int get_size_in_bytes(int initial_size, char* multiplier);

int create_database(int size, char* name);

void format_database(int size, char* name);

void delete_database(char* name);