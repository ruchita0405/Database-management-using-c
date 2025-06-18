#include "file_ops.h"
#include "csv_utils.h"
#include "bptree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

int load_records_from_csv(const char *filename, BPTree *tree) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    char line[512];
    Record rec;
    while (fgets(line, sizeof(line), fp)) {
        if (parse_csv_line(line, &rec) == 4) {
            Record *new_rec = malloc(sizeof(Record));
            *new_rec = rec;
            bptree_insert(tree, rec.id, new_rec);
        }
    }
    fclose(fp);
    return 0;
}

int append_record_to_csv(const char *filename, const Record *rec) {
    FILE *fp = fopen(filename, "a");
    if (!fp) return -1;
    char line[512];
    record_to_csv_line(rec, line, sizeof(line));
    fprintf(fp, "%s\n", line);
    fclose(fp);
    return 0;
}

int delete_record_from_csv(const char *filename, int key) {
    FILE *src = fopen(filename, "r");
    if (!src) return -1;

    char tmp_filename[260];
    snprintf(tmp_filename, sizeof(tmp_filename), "%s.tmp", filename);
    FILE *tmp = fopen(tmp_filename, "w");
    if (!tmp) {
        fclose(src);
        return -1;
    }

    char line[512];
    Record rec;
    int deleted = 0;
    while (fgets(line, sizeof(line), src)) {
        if (parse_csv_line(line, &rec) == 4) {
            if (rec.id == key) {
                deleted = 1; // Skip this record (delete)
                continue;
            }
        }
        fputs(line, tmp);
    }

    fclose(src);
    fclose(tmp);

    // Replace original file with temp file
    remove(filename);
    rename(tmp_filename, filename);

    return deleted ? 0 : 1; // 0 if deleted, 1 if not found
}