#include "bptree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BPTreeNode* bptree_create_node(){
    BPTreeNode* node = (BPTreeNode*)malloc(sizeof(BPTreeNode));
    if(node){
        node->num_keys = 0;
        node->next = NULL;
        node->is_leaf = 1;
        for (int i = 0; i < ORDER - 1; i++) {
            node->keys[i] = 0;
            node->children[i] = NULL;
            node->records[i] = NULL;
        }
        node->children[ORDER - 1] = NULL;
    }
    return node;
}

BPTree* bptree_create() {
    BPTree *tree = (BPTree*)malloc(sizeof(BPTree)); 
    if(tree){
        tree->root = bptree_create_node();
    }
    return tree;
}

// Forward declarations for helpers
void bptree_split_child(BPTreeNode *parent, int idx, BPTreeNode *child);
void bptree_insert_nonfull(BPTreeNode *node, int key, Record *record);

void bptree_insert(BPTree *tree, int key, Record *record) {
    BPTreeNode *root = tree->root;
    if(root->num_keys == ORDER - 1) {
        // Root full, need to split
        BPTreeNode *new_root = bptree_create_node();
        new_root->is_leaf = 0;
        new_root->children[0] = root;
        tree->root = new_root;
        bptree_split_child(new_root, 0, root);
        bptree_insert_nonfull(new_root, key, record);
    } else {
        bptree_insert_nonfull(root, key, record);
    }
}

void bptree_insert_nonfull(BPTreeNode *node, int key, Record *record) {
    int i = node->num_keys - 1;
    if(node->is_leaf) {
        // Insert in sorted order in leaf
        while(i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->records[i+1] = node->records[i];
            i--;
        }
        node->keys[i+1] = key;
        node->records[i+1] = record;
        node->num_keys++;
    } else {
        // Find child to descend
        while(i >= 0 && key < node->keys[i]) i--;
        i++;
        if(node->children[i]->num_keys == ORDER - 1) {
            bptree_split_child(node, i, node->children[i]);
            if(key > node->keys[i]) i++;
        }
        bptree_insert_nonfull(node->children[i], key, record);
    }
}

void bptree_split_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
    // Split child at index idx of parent
    BPTreeNode *new_child = bptree_create_node();
    new_child->is_leaf = child->is_leaf;
    int mid = (ORDER - 1) / 2;

    // For order 4 (ORDER-1=3), mid = 1, so 1 key to left, 1 to parent, 1 to right.
    new_child->num_keys = ORDER - 1 - mid - 1; // For ORDER 4: 3-1-1=1 key to right child

    // Copy keys and records for new child
    for(int j = 0; j < new_child->num_keys; j++) {
        new_child->keys[j] = child->keys[mid + 1 + j];
        if(child->is_leaf) new_child->records[j] = child->records[mid + 1 + j];
    }
    if(!child->is_leaf) {
        for(int j = 0; j < new_child->num_keys + 1; j++) {
            new_child->children[j] = child->children[mid + 1 + j];
        }
    }
    child->num_keys = mid;
    // Insert new child into parent
    for(int j = parent->num_keys; j > idx; j--) {
        parent->children[j+1] = parent->children[j];
        parent->keys[j] = parent->keys[j-1];
    }
    parent->children[idx+1] = new_child;
    parent->keys[idx] = child->keys[mid];
    parent->num_keys++;

    // Leaf node chaining
    if(child->is_leaf) {
        new_child->next = child->next;
        child->next = new_child;
    }
}

Record* bptree_search(BPTree *tree, int key) {
    BPTreeNode *node = tree->root;
    while(node) {
        int i = 0;
        while(i < node->num_keys && key > node->keys[i]) i++;
        if(i < node->num_keys && key == node->keys[i]) {
            if(node->is_leaf) return node->records[i];
            else node = node->children[i+1];
        } else if(node->is_leaf) {
            return NULL;
        } else {
            node = node->children[i];
        }
    }
    return NULL;
}

// For simplicity, delete only from leaf, no merge/borrow (suitable for small assignments)
void bptree_delete(BPTree *tree, int key) {
    BPTreeNode *node = tree->root;
    while(node) {
        int i = 0;
        while(i < node->num_keys && key > node->keys[i]) i++;
        if(node->is_leaf) {
            if(i < node->num_keys && node->keys[i] == key) {
                // Delete key and record at i
                free(node->records[i]);
                for(int j = i; j < node->num_keys - 1; j++) {
                    node->keys[j] = node->keys[j+1];
                    node->records[j] = node->records[j+1];
                }
                node->num_keys--;
            }
            return;
        } else {
            node = node->children[i];
        }
    }
}

// Recursive destroy
void bptree_destroy_node(BPTreeNode *node) {
    if(!node) return;
    if(!node->is_leaf) {
        for(int i = 0; i <= node->num_keys; i++) {
            bptree_destroy_node(node->children[i]);
        }
    } else {
        for(int i = 0; i < node->num_keys; i++) {
            free(node->records[i]);
        }
    }
    free(node);
}

void bptree_destroy(BPTree *tree) {
    if(tree) {
        bptree_destroy_node(tree->root);
        free(tree);
    }
}