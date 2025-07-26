#include "record.h"
#include <stdio.h>

void print_record(const Record *rec) {
    printf("ID: %d, Name: %s, Address: %s, Age: %d\n",
           rec->id, rec->name, rec->address, rec->age);
}

void normalize_string(const char *src, char *dest){
     size_t i;
    for (i = 0; src[i] && i < NAME_LEN - 1; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}