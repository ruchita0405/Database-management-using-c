#include "record.h"
#include <stdio.h>

void print_record(const Record *rec) {
    printf("ID: %d, Name: %s, Address: %s, Age: %d\n",
           rec->id, rec->name, rec->address, rec->age);
}