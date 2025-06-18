#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include "record.h"

// Functions to parse and write CSV records
int parse_csv_line(const char *line, Record *rec);
void record_to_csv_line(const Record *rec, char *line, size_t maxlen);

#endif