#include "bptree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BPTreeNode* bptree_create_node() {
    BPTreeNode* node = (BPTreeNode*)malloc(sizeof(BPTreeNode));
    if(node) {
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
    if(tree) {
        tree->root = bptree_create_node();
    }
    return tree;
}

// Forward declarations for helpers (int)
void bptree_split_child(BPTreeNode *parent, int idx, BPTreeNode *child);
void bptree_insert_nonfull(BPTreeNode *node, int key, Record *record);

void bptree_insert(BPTree *tree, int key, Record *record) {
    BPTreeNode *root = tree->root;
    if(root->num_keys == ORDER - 1) {
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
        while(i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->records[i+1] = node->records[i];
            i--;
        }
        node->keys[i+1] = key;
        node->records[i+1] = record;
        node->num_keys++;
    } else {
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
    BPTreeNode *new_child = bptree_create_node();
    new_child->is_leaf = child->is_leaf;
    int mid = (ORDER - 1) / 2;
    new_child->num_keys = ORDER - 1 - mid - 1;
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
    for(int j = parent->num_keys; j > idx; j--) {
        parent->children[j+1] = parent->children[j];
        parent->keys[j] = parent->keys[j-1];
    }
    parent->children[idx+1] = new_child;
    parent->keys[idx] = child->keys[mid];
    parent->num_keys++;
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

void bptree_delete(BPTree *tree, int key) {
    BPTreeNode *node = tree->root;
    while(node) {
        int i = 0;
        while(i < node->num_keys && key > node->keys[i]) i++;
        if(node->is_leaf) {
            if(i < node->num_keys && node->keys[i] == key) {
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


NameBPTreeNode* name_bptree_create_node() {
    NameBPTreeNode* node = (NameBPTreeNode*)malloc(sizeof(NameBPTreeNode));
    if(node){
        node->num_keys = 0;
        node->next = NULL;
        node->is_leaf = 1;
        for (int i = 0; i < ORDER - 1; i++) {
            node->keys[i] = NULL;
            node->children[i] = NULL;
            node->record_lists[i] = NULL;
        }
        node->children[ORDER - 1] = NULL;
    }
    return node;
}

NameBPTree* name_bptree_create() {
    NameBPTree *tree = (NameBPTree*)malloc(sizeof(NameBPTree)); 
    if(tree){
        tree->root = name_bptree_create_node();
    }
    return tree;
}

RecordList* record_list_insert(RecordList* head, Record* rec) {
    RecordList* new_rec = (RecordList*)malloc(sizeof(RecordList));
    new_rec->record = rec;
    new_rec->next = head;
    return new_rec;
}

RecordList* record_list_remove(RecordList* head, Record* rec) {
    RecordList* prev = NULL;
    RecordList* curr = head;
    while (curr) {
        if (curr->record == rec) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    return head;
}

void record_list_free(RecordList* head) {
    RecordList* curr = head;
    while(curr) {
        RecordList* tmp = curr;
        curr = curr->next;
        free(tmp);
    }
}

void name_bptree_split_child(NameBPTreeNode *parent, int idx, NameBPTreeNode *child);
void name_bptree_insert_nonfull(NameBPTreeNode *node, const char *name, Record *record);

// String compare for B+ Tree (lexicographical)
int keycmp(const char* s1, const char* s2) {
    for (; *s1 && *s2; ++s1, ++s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2)
            return c1 - c2;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Insert into name-based (string) B+ Tree
void name_bptree_insert(NameBPTree *tree, const char *name_lower, Record *record) {
    NameBPTreeNode *root = tree->root;
    if(root->num_keys == ORDER - 1) {
        NameBPTreeNode *new_root = name_bptree_create_node();
        new_root->is_leaf = 0;
        new_root->children[0] = root;
        tree->root = new_root;
        name_bptree_split_child(new_root, 0, root);
        name_bptree_insert_nonfull(new_root, name_lower, record);
    } else {
        name_bptree_insert_nonfull(root, name_lower, record);
    }
}


void name_bptree_insert_nonfull(NameBPTreeNode *node, const char *name, Record *record) {
    int i = node->num_keys - 1;
    if(node->is_leaf) {
        // Find position to insert
        while(i >= 0 && keycmp(name, node->keys[i]) < 0) i--;
        // Check if name already exists at i+1
        if (i >= 0 && keycmp(name, node->keys[i]) == 0) {
            // Insert into existing list
            node->record_lists[i] = record_list_insert(node->record_lists[i], record);
            return;
        }
        // Shift to make room
        for(int j = node->num_keys - 1; j > i; j--) {
            node->keys[j+1] = node->keys[j];
            node->record_lists[j+1] = node->record_lists[j];
        }
        // Insert new key and a new list
        node->keys[i+1] = strdup(name); // duplicate string
        node->record_lists[i+1] = record_list_insert(NULL, record);
        node->num_keys++;
    } else {
        while(i >= 0 && keycmp(name, node->keys[i]) < 0) i--;
        i++;
        if(node->children[i]->num_keys == ORDER - 1) {
            name_bptree_split_child(node, i, node->children[i]);
            if(keycmp(name, node->keys[i]) > 0) i++;
        }
        name_bptree_insert_nonfull(node->children[i], name, record);
    }
}

void name_bptree_split_child(NameBPTreeNode *parent, int idx, NameBPTreeNode *child) {
    NameBPTreeNode *new_child = name_bptree_create_node();
    new_child->is_leaf = child->is_leaf;
    int mid = (ORDER - 1) / 2;
    new_child->num_keys = ORDER - 1 - mid - 1;
    for(int j = 0; j < new_child->num_keys; j++) {
        new_child->keys[j] = child->keys[mid + 1 + j];
        new_child->record_lists[j] = child->record_lists[mid + 1 + j];
    }
    if(!child->is_leaf) {
        for(int j = 0; j < new_child->num_keys + 1; j++) {
            new_child->children[j] = child->children[mid + 1 + j];
        }
    }
    child->num_keys = mid;
    for(int j = parent->num_keys; j > idx; j--) {
        parent->children[j+1] = parent->children[j];
        parent->keys[j] = parent->keys[j-1];
    }
    parent->children[idx+1] = new_child;
    parent->keys[idx] = child->keys[mid];
    parent->num_keys++;
    if(child->is_leaf) {
        new_child->next = child->next;
        child->next = new_child;
    }
}

// Print all records for a given name, returns count
int name_bptree_search(NameBPTree *tree, const char *name_lower) {
    NameBPTreeNode *node = tree->root;
    while(node) {
        int i = 0;
        while(i < node->num_keys && keycmp(name_lower, node->keys[i]) > 0) i++;
        if(i < node->num_keys && keycmp(name_lower, node->keys[i]) == 0) {
            RecordList *rl = node->record_lists[i];
            int count = 0;
            while(rl) {
                print_record(rl->record);
                rl = rl->next;
                count++;
            }
            return count;
        } else if(node->is_leaf) {
            return 0;
        } else {
            node = node->children[i];
        }
    }
    return 0;
}

// Print all records where name starts with prefix
void name_bptree_prefix_search(NameBPTree *tree, const char *prefix) {
    NameBPTreeNode *node = tree->root;
    // Find leftmost leaf that could have prefix
    while(node && !node->is_leaf) node = node->children[0];
    int prefixlen = strlen(prefix);
    while(node) {
        for(int i = 0; i < node->num_keys; i++) {
            if(strncmp(node->keys[i], prefix, prefixlen) == 0) {
                RecordList *rl = node->record_lists[i];
                while(rl) {
                    print_record(rl->record);
                    rl = rl->next;
                }
            }
        }
        node = node->next;
    }
}

// Remove the record pointer from the name tree
void name_bptree_delete(NameBPTree *tree, const char *name_lower, Record *record) {
    NameBPTreeNode *node = tree->root;
    while(node) {
        int i = 0;
        while(i < node->num_keys && keycmp(name_lower, node->keys[i]) > 0) i++;
        if(i < node->num_keys && keycmp(name_lower, node->keys[i]) == 0) {
            node->record_lists[i] = record_list_remove(node->record_lists[i], record);
            if (!node->record_lists[i]) {
                free(node->keys[i]);
                for(int j = i; j < node->num_keys - 1; j++) {
                    node->keys[j] = node->keys[j+1];
                    node->record_lists[j] = node->record_lists[j+1];
                }
                node->num_keys--;
            }
            return;
        } else if(node->is_leaf) {
            return;
        } else {
            node = node->children[i];
        }
    }
}

// Free NameBPTree nodes
void name_bptree_destroy_node(NameBPTreeNode *node) {
    if(!node) return;
    if(!node->is_leaf) {
        for(int i = 0; i <= node->num_keys; i++) {
            name_bptree_destroy_node(node->children[i]);
        }
    } else {
        for(int i = 0; i < node->num_keys; i++) {
            free(node->keys[i]);
            record_list_free(node->record_lists[i]);
        }
    }
    free(node);
}

void name_bptree_destroy(NameBPTree *tree) {
    if(tree) {
        name_bptree_destroy_node(tree->root);
        free(tree);
    }
}