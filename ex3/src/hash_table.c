#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

// Node structure for chaining
typedef struct HashNode {
    void* key;
    void* value;
    struct HashNode* next;
} HashNode;

// Hash table structure
struct HashTable {
    HashNode** buckets;
    int capacity;
    int size;
    int (*compare_func)(const void*, const void*);
    unsigned long (*hash_func)(const void*);
};

// Create a new hash node
static HashNode* hash_node_create(const void* key, const void* value) {
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    if (!node) return NULL;
    
    node->key = (void*)key;
    node->value = (void*)value;
    node->next = NULL;
    return node;
}

// Free a hash node
static void hash_node_free(HashNode* node) {
    if (node) {
        free(node);
    }
}

// Resize the hash table when load factor exceeds threshold
static void hash_table_resize(HashTable* table) {
    if (!table) return;
    
    HashNode** old_buckets = table->buckets;
    int old_capacity = table->capacity;
    
    // Double the capacity
    table->capacity *= 2;
    table->buckets = (HashNode**)calloc(table->capacity, sizeof(HashNode*));
    if (!table->buckets) {
        // Restore old state if allocation fails
        table->buckets = old_buckets;
        table->capacity = old_capacity;
        return;
    }
    
    // int old_size = table->size;
    table->size = 0;
    
    // Rehash all existing entries
    for (int i = 0; i < old_capacity; i++) {
        HashNode* node = old_buckets[i];
        while (node) {
            HashNode* next = node->next;
            
            // Find new bucket for this node
            unsigned long hash = table->hash_func(node->key);
            int index = hash % table->capacity;
            
            // Insert at the beginning of the new bucket
            node->next = table->buckets[index];
            table->buckets[index] = node;
            table->size++;
            
            node = next;
        }
    }
    
    free(old_buckets);
}

HashTable* hash_table_create(int (*compare_func)(const void*, const void*), 
                             unsigned long (*hash_func)(const void*)) {
    if (!compare_func || !hash_func) return NULL;
    
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    table->buckets = (HashNode**)calloc(INITIAL_CAPACITY, sizeof(HashNode*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->compare_func = compare_func;
    table->hash_func = hash_func;
    
    return table;
}

void hash_table_put(HashTable* table, const void* key, const void* value) {
    if (!table || !key) return;
    
    unsigned long hash = table->hash_func(key);
    int index = hash % table->capacity;
    
    HashNode* node = table->buckets[index];
    
    // Check if key already exists
    while (node) {
        if (table->compare_func(node->key, key) == 0) {
            // Update existing value
            node->value = (void*)value;
            return;
        }
        node = node->next;
    }
    
    // Create new node and insert at the beginning
    HashNode* new_node = hash_node_create(key, value);
    if (!new_node) return;
    
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;
    
    // Check if resize is needed
    if ((double)table->size / table->capacity > LOAD_FACTOR_THRESHOLD) {
        hash_table_resize(table);
    }
}

void* hash_table_get(const HashTable* table, const void* key) {
    if (!table || !key) return NULL;
    
    unsigned long hash = table->hash_func(key);
    int index = hash % table->capacity;
    
    HashNode* node = table->buckets[index];
    while (node) {
        if (table->compare_func(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    
    return NULL;
}

int hash_table_contains_key(const HashTable* table, const void* key) {
    return hash_table_get(table, key) != NULL;
}

void hash_table_remove(HashTable* table, const void* key) {
    if (!table || !key) return;
    
    unsigned long hash = table->hash_func(key);
    int index = hash % table->capacity;
    
    HashNode* node = table->buckets[index];
    HashNode* prev = NULL;
    
    while (node) {
        if (table->compare_func(node->key, key) == 0) {
            // Remove the node
            if (prev) {
                prev->next = node->next;
            } else {
                table->buckets[index] = node->next;
            }
            hash_node_free(node);
            table->size--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

int hash_table_size(const HashTable* table) {
    return table ? table->size : 0;
}

void** hash_table_keyset(const HashTable* table) {
    if (!table || table->size == 0) return NULL;
    
    void** keys = (void**)malloc(table->size * sizeof(void*));
    if (!keys) return NULL;
    
    int key_index = 0;
    for (int i = 0; i < table->capacity; i++) {
        HashNode* node = table->buckets[i];
        while (node) {
            keys[key_index++] = node->key;
            node = node->next;
        }
    }
    
    return keys;
}

void hash_table_free(HashTable* table) {
    if (!table) return;
    
    // Free all nodes
    for (int i = 0; i < table->capacity; i++) {
        HashNode* node = table->buckets[i];
        while (node) {
            HashNode* next = node->next;
            hash_node_free(node);
            node = next;
        }
    }
    
    free(table->buckets);
    free(table);
}
