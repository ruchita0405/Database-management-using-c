#include "bptree.h"
#include "file_ops.h"
#include "record.h"
#include <stdio.h>
#include <stdlib.h>

#define DATA_FILE "data/records.csv"

int main() {
    BPTree *tree = bptree_create();
    load_records_from_csv(DATA_FILE, tree);

    int choice, id, age;
    char name[NAME_LEN], address[ADDRESS_LEN];

    while (1) {
        printf("1. Insert\n2. Search\n3. Delete\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) {
            printf("Enter ID: ");
            scanf("%d", &id);
            printf("Enter Name Address Age: ");
            fgets(name, sizeof(name), stdin);
            printf("Enter address: ");
            fgets(address, sizeof(address), stdin);
            printf("Enter age: ");
            scanf("%d", &age);

            Record *rec = malloc(sizeof(Record));
            rec->id = id; rec->age = age;
            snprintf(rec->name, NAME_LEN, "%s", name);
            snprintf(rec->address, ADDRESS_LEN, "%s", address);
            bptree_insert(tree, id, rec);
            append_record_to_csv(DATA_FILE, rec);
        } else if (choice == 2) {
            printf("Enter ID to search: ");
            scanf("%d", &id);
            Record *rec = bptree_search(tree, id);
            if (rec) print_record(rec);                
            else printf("Not found\n");
        } else if (choice == 3) {
            printf("Enter ID to delete: ");             //improve logic!!
            scanf("%d", &id);
            bptree_delete(tree, id);
            delete_record_from_csv(DATA_FILE, id);
        } else {
            break;
        }
    }
    bptree_destroy(tree);
    return 0;
}