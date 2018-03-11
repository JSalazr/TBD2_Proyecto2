#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "DBAPI.h"

using namespace std;

char database_to_use[50];

int main(int argc, char **argv){
    char *option = new char[500];
    string opt;
    do{
        cout << "DBCLI > ";
        cin.getline(option, 500, '\n');
        char* op1 = strtok(option, " ");
        char* op2 = strtok(NULL, " ");
        char* op3 = strtok(NULL, " ");
        char* op4 = strtok(NULL, " ");
        char* op5 = strtok(NULL, " ");
        char* op6 = strtok(NULL, " ");
        if(!strcmp(op1, "create") && !strcmp(op2, "database")){
            if(atoi(op4) > 128){
                cout << "Tamano maximo de DB es de 128 MB" << endl;
            }else{
                int size_in_bytes = get_size_in_bytes(atoi(op4), op5);
                create_database(size_in_bytes, op3);
                format_database(size_in_bytes, op3);
            }
        }else if(!strcmp(op1, "drop") && !strcmp(op2, "database")){
            delete_database(op3);
        }else if(!strcmp(op1, "use") && !strcmp(op2, "database")){
            strcpy(database_to_use, op3);
        }else if(!strcmp(op1, "create") && !strcmp(op2, "table")){
            create_table(database_to_use, op3, get_values(op4), get_values(op5), get_values(op6));
        }else if(!strcmp(op1, "drop") && !strcmp(op2, "table")){
            drop_table(database_to_use, op3);
        }else if(!strcmp(op1, "show") && !strcmp(op2, "tables")){
            show_tables(database_to_use);
        }else if(!strcmp(op1, "insert")){
            insert_register(database_to_use, op2, get_values(op3), get_values(op4));
        }else if(!strcmp(op1, "select")){
            select_show(database_to_use, get_values(op2), get_values(op3), get_values(op4));
        }else if(!strcmp(op1, "update")){
            update_register(database_to_use, op2, get_values(op3), get_values(op4), get_values(op5));
        }else if(!strcmp(op1, "delete")){
            delete_register(database_to_use, op2, get_values(op3));
        }else if(!strcmp(op1, "exit")){
            break;
        }
        strcpy(option, " ");
    }while(true);
}