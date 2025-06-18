#ifndef BPTREE_H
#define BPTREE_H

#include "record.h"

#define ORDER 4 // B+ Tree order

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

BPTree* bptree_create();
BPTreeNode* bptree_create_node();
void bptree_insert(BPTree *tree, int key, Record *record);
Record* bptree_search(BPTree *tree, int key);
void bptree_delete(BPTree *tree, int key);
void bptree_destroy(BPTree *tree);

#endif