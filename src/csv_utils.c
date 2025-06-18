#include "csv_utils.h"
#include <stdio.h>
#include <string.h>

int parse_csv_line(const char *line, Record *rec) {
    // Simple CSV parsing (improve for edge cases in a real project)
    return sscanf(line, "%d,%99[^,],%199[^,],%d", &rec->id, rec->name, rec->address, &rec->age);
}

void record_to_csv_line(const Record *rec, char *line, size_t maxlen) {
    snprintf(line, maxlen, "%d,%s,%s,%d", rec->id, rec->name, rec->address, rec->age);
}