#ifndef RECORD_H
#define RECORD_H

#define NAME_LEN 100
#define ADDRESS_LEN 200

typedef struct {
    int id;
    char name[NAME_LEN];
    char address[ADDRESS_LEN];
    int age;
} Record;

void print_record(const Record *rec);

#endif