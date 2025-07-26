#include "bptree.h"
#include "file_ops.h"
#include "record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "data/records.csv"

// ANSI color codes for output (can be removed if not supported)
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"

// Helper to clear input buffer
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Simple header/banner
void print_banner() {
    printf(COLOR_CYAN "=====================================\n");
    printf("  Municipal Corp DMS - Main Menu\n");
    printf("=====================================\n" COLOR_RESET);
}

int main() {
    BPTree *id_tree = bptree_create();
    NameBPTree *name_tree = name_bptree_create();
    load_records_from_csv(DATA_FILE, id_tree, name_tree); // You should update this to load into both trees

    int choice, id, age;
    char name[NAME_LEN], address[ADDRESS_LEN];
    char name_lower[NAME_LEN];

    while (1) {
        print_banner();
        printf(COLOR_YELLOW "1. Insert Record\n");
        printf("2. Search by ID\n");
        printf("3. Search by Name\n");
        printf("4. Delete by ID\n");
        printf("5. Exit\n" COLOR_RESET);
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf(COLOR_RED "Invalid input, please enter a number.\n" COLOR_RESET);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer(); // Remove trailing newline

        if (choice == 1) {
            printf("Enter ID: ");
            if (scanf("%d", &id) != 1) {
                printf(COLOR_RED "Invalid ID!\n" COLOR_RESET);
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            printf("Enter Name: ");
            if (!fgets(name, sizeof(name), stdin)) continue;
            name[strcspn(name, "\n")] = 0;
            normalize_string(name, name_lower);

            printf("Enter Address: ");
            if (!fgets(address, sizeof(address), stdin)) continue;
            address[strcspn(address, "\n")] = 0;

            printf("Enter Age: ");
            if (scanf("%d", &age) != 1) {
                printf(COLOR_RED "Invalid age!\n" COLOR_RESET);
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            // Check if ID already exists
            if (bptree_search(id_tree, id)) {
                printf(COLOR_RED "ID already exists! Try a different ID.\n" COLOR_RESET);
                continue;
            }

            Record *rec = malloc(sizeof(Record));
            if (!rec) {
                printf(COLOR_RED "Memory allocation failed!\n" COLOR_RESET);
                exit(1);
            }
            rec->id = id;
            rec->age = age;
            strncpy(rec->name, name, NAME_LEN);
            strncpy(rec->name_lower, name_lower, NAME_LEN);
            strncpy(rec->address, address, ADDRESS_LEN);

            bptree_insert(id_tree, id, rec);
            name_bptree_insert(name_tree, rec->name_lower, rec);
            append_record_to_csv(DATA_FILE, rec);
            printf(COLOR_GREEN "Record inserted successfully.\n" COLOR_RESET);

        } else if (choice == 2) {
            printf("Enter ID to search: ");
            if (scanf("%d", &id) != 1) {
                printf(COLOR_RED "Invalid ID!\n" COLOR_RESET);
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();
            Record *rec = bptree_search(id_tree, id);
            if (rec)
                print_record(rec);
            else
                printf(COLOR_YELLOW "No record found with ID %d.\n" COLOR_RESET, id);

        } else if (choice == 3) {
           printf("Enter Name to search: ");
            if (!fgets(name, sizeof(name), stdin)) continue;
            name[strcspn(name, "\n")] = 0;
            normalize_string(name, name_lower);
            int found = name_bptree_search(name_tree, name_lower); // Search by normalized name
            if (!found)
                printf(COLOR_YELLOW "No records found with name '%s'.\n" COLOR_RESET, name);
        } else if (choice == 4) {
            printf("Enter ID to delete: ");
            if (scanf("%d", &id) != 1) {
                printf(COLOR_RED "Invalid ID!\n" COLOR_RESET);
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();
            Record *rec = bptree_search(id_tree, id);
            if (!rec) {
                printf(COLOR_YELLOW "ID %d does not exist.\n" COLOR_RESET, id);
                continue;
            }

            // Show the record before deletion
            printf(COLOR_YELLOW "Record to be deleted:\n" COLOR_RESET);
            print_record(rec);

            bptree_delete(id_tree, id);
            name_bptree_delete(name_tree, rec->name_lower, rec); // Use normalized name
            delete_record_from_csv(DATA_FILE, id);
            printf(COLOR_GREEN "Record with ID %d deleted successfully.\n" COLOR_RESET, id);

        } else if (choice == 5) {
            printf(COLOR_CYAN "Exiting. Have a great day!\n" COLOR_RESET);
            break;
        } else {
            printf(COLOR_RED "Invalid choice. Please select a valid option.\n" COLOR_RESET);
        }
    }

    bptree_destroy(id_tree);
    name_bptree_destroy(name_tree);
    return 0;
}