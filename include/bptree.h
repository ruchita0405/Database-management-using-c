#ifndef BPTREE_H
#define BPTREE_H

#include "record.h"

#define ORDER 4 // B+ Tree order

// Integer-keyed B+ Tree Node (for ID)
typedef struct BPTreeNode {
    int is_leaf;
    int num_keys;
    int keys[ORDER - 1];
    Record *records[ORDER - 1];
    struct BPTreeNode *children[ORDER];
    struct BPTreeNode *next; // For leaf node chaining
} BPTreeNode;

typedef struct {
    BPTreeNode *root;
} BPTree;

// String-keyed B+ Tree Node (for Name)
typedef struct NameBPTreeNode {
    int is_leaf;
    int num_keys;
    char *keys[ORDER - 1]; // String keys
    // Each name can have multiple records (to handle duplicate names)
    struct RecordList *record_lists[ORDER - 1];
    struct NameBPTreeNode *children[ORDER];
    struct NameBPTreeNode *next; // For leaf node chaining
} NameBPTreeNode;

typedef struct {
    NameBPTreeNode *root;
} NameBPTree;

// Linked list of Record pointers for duplicate names
typedef struct RecordList {
    Record *record;
    struct RecordList *next;
} RecordList;

// ID-based B+ Tree functions
BPTree* bptree_create();
BPTreeNode* bptree_create_node();
void bptree_insert(BPTree *tree, int key, Record *record);
Record* bptree_search(BPTree *tree, int key);
void bptree_delete(BPTree *tree, int key);
void bptree_destroy(BPTree *tree);

// Name-based (string) B+ Tree functions
NameBPTree* name_bptree_create();
NameBPTreeNode* name_bptree_create_node();
void name_bptree_insert(NameBPTree *tree, const char *name, Record *record);
void name_bptree_delete(NameBPTree *tree, const char *name, Record *record); // Removes only the specified record for the name
int name_bptree_search(NameBPTree *tree, const char *name); // Print all records matching the name
void name_bptree_prefix_search(NameBPTree *tree, const char *prefix); // Print all records matching the prefix
void name_bptree_destroy(NameBPTree *tree);

#endif