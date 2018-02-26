#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "API.h"

using namespace std;


int main(int argc, char **argv){
    if(argc > 2){
        if(!strcmp(argv[1], "create") && !strcmp(argv[2], "database")){
            int size_in_bytes = get_size_in_bytes(atoi(argv[4]), argv[5]);
            create_file(size_in_bytes, argv[3]);
        }
    }
}