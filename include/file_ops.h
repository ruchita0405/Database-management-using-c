#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "record.h"

// File operations for reading and writing records to CSV
int load_records_from_csv(const char *filename, BPTree *tree);
int append_record_to_csv(const char *filename, const Record *rec);
int delete_record_from_csv(const char *filename, int key);

#endif