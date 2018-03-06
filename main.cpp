#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "DBAPI.h"

using namespace std;


int main(int argc, char **argv){
    if(argc > 2){
        if(!strcmp(argv[1], "create") && !strcmp(argv[2], "database")){
            if(atoi(argv[4]) > 128){
                cout << "Tamano maximo de DB es de 128 MB" << endl;
            }else{
                int size_in_bytes = get_size_in_bytes(atoi(argv[4]), argv[5]);
                create_database(size_in_bytes, argv[3]);
                format_database(size_in_bytes, argv[3]);
            }
        }else if(!strcmp(argv[1], "drop") && !strcmp(argv[2], "database")){
            delete_database(argv[3]);
        }else if(!strcmp(argv[1], "use") && !strcmp(argv[2], "database")){
            std::fstream fs("database_to_use", std::ios::out);
            int c = 0;
            while(argv[3][c] != 0){
                fs.put(argv[3][c]);
                c++;
            }
            fs.close();
        }else if(!strcmp(argv[1], "create") && !strcmp(argv[2], "table")){
            create_table(get_database_to_use(), argv[3], get_values(argv[4]), get_values(argv[5]), get_values(argv[6]));
        }else if(!strcmp(argv[1], "show") && !strcmp(argv[2], "tables")){
            show_tables(get_database_to_use());
        }else if(!strcmp(argv[1], "insert")){
            insert_register(get_database_to_use(), argv[2], get_values(argv[3]), get_values(argv[4]));
        }else if(!strcmp(argv[1], "select")){
            select_show(get_database_to_use(), get_values(argv[2]), get_values(argv[3]), get_values(argv[4]));
        }
    }
}