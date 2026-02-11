#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

/**
 * Node structure for chaining in hash table buckets
 */
typedef struct HashNode {
    void* key;
    void* value;
    struct HashNode* next;
} HashNode;

/**
 * Hash table structure with dynamic resizing capability
 */
struct HashTable {
    HashNode** buckets;           // Array of bucket pointers
    int capacity;                 // Current capacity of the table
    int size;                     // Number of elements in the table
    int (*compare_func)(const void*, const void*);  // Key comparison function
    unsigned long (*hash_func)(const void*);        // Hash function for keys
};

/**
 * Creates a new hash node with given key and value
 * Returns pointer to new node or NULL on failure
 */
static HashNode* hash_node_create(const void* key, const void* value) {
    if (!key) {
        fprintf(stderr, "Error: Cannot create hash node with NULL key\n");
        return NULL;
    }
    
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for hash node\n");
        return NULL;
    }
    
    node->key = (void*)key;
    node->value = (void*)value;
    node->next = NULL;
    return node;
}

/**
 * Frees memory allocated for a hash node
 */
static void hash_node_free(HashNode* node) {
    if (node) {
        free(node);
    }
}

/**
 * Resizes the hash table when load factor exceeds threshold
 * Doubles the capacity and rehashes all existing entries
 */
static void hash_table_resize(HashTable* table) {
    if (!table) {
        fprintf(stderr, "Error: Cannot resize NULL hash table\n");
        return;
    }
    
    HashNode** old_buckets = table->buckets;
    int old_capacity = table->capacity;
    
    // Double the capacity
    table->capacity *= 2;
    table->buckets = (HashNode**)calloc(table->capacity, sizeof(HashNode*));
    if (!table->buckets) {
        fprintf(stderr, "Error: Memory allocation failed during hash table resize\n");
        // Restore old state if allocation fails
        table->buckets = old_buckets;
        table->capacity = old_capacity;
        return;
    }
    
    // Reset size to 0 for rehashing
    table->size = 0;
    
    // Rehash all existing entries
    for (int i = 0; i < old_capacity; i++) {
        HashNode* node = old_buckets[i];
        while (node) {
            HashNode* next = node->next;
            
            // Calculate new bucket for this node
            unsigned long hash = table->hash_func(node->key);
            int index = hash % table->capacity;
            
            // Insert at the beginning of the new bucket
            node->next = table->buckets[index];
            table->buckets[index] = node;
            table->size++;
            
            node = next;
        }
    }
    
    // Free the old bucket array
    free(old_buckets);
}

/**
 * Creates a new hash table with specified comparison and hash functions
 * Returns pointer to new hash table or NULL on failure
 */
HashTable* hash_table_create(int (*compare_func)(const void*, const void*), 
                             unsigned long (*hash_func)(const void*)) {
    if (!compare_func || !hash_func) {
        fprintf(stderr, "Error: Hash table requires valid compare and hash functions\n");
        return NULL;
    }
    
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        fprintf(stderr, "Error: Memory allocation failed for hash table structure\n");
        return NULL;
    }
    
    table->buckets = (HashNode**)calloc(INITIAL_CAPACITY, sizeof(HashNode*));
    if (!table->buckets) {
        fprintf(stderr, "Error: Memory allocation failed for hash table buckets\n");
        free(table);
        return NULL;
    }
    
    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->compare_func = compare_func;
    table->hash_func = hash_func;
    
    return table;
}

/**
 * Inserts or updates a key-value pair in the hash table
 * If key exists, updates the value; otherwise creates new entry
 */
void hash_table_put(HashTable* table, const void* key, const void* value) {
    if (!table) {
        fprintf(stderr, "Error: Cannot put into NULL hash table\n");
        return;
    }
    
    if (!key) {
        fprintf(stderr, "Error: Cannot insert NULL key into hash table\n");
        return;
    }
    
    unsigned long hash = table->hash_func(key);
    int index = hash % table->capacity;
    
    HashNode* node = table->buckets[index];
    
    // Check if key already exists and update value
    while (node) {
        if (table->compare_func(node->key, key) == 0) {
            node->value = (void*)value;
            return;
        }
        node = node->next;
    }
    
    // Create new node and insert at the beginning of bucket
    HashNode* new_node = hash_node_create(key, value);
    if (!new_node) {
        return; // Error already reported in hash_node_create
    }
    
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;
    
    // Check if resize is needed
    if ((double)table->size / table->capacity > LOAD_FACTOR_THRESHOLD) {
        hash_table_resize(table);
    }
}

/**
 * Retrieves the value associated with a key
 * Returns pointer to value if found, NULL otherwise
 */
void* hash_table_get(const HashTable* table, const void* key) {
    if (!table) {
        fprintf(stderr, "Error: Cannot get from NULL hash table\n");
        return NULL;
    }
    
    if (!key) {
        fprintf(stderr, "Error: Cannot search for NULL key in hash table\n");
        return NULL;
    }
    
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

/**
 * Checks if a key exists in the hash table
 * Returns 1 if key exists, 0 otherwise
 */
int hash_table_contains_key(const HashTable* table, const void* key) {
    if (!table || !key) {
        return 0;
    }
    
    return hash_table_get(table, key) != NULL;
}

/**
 * Removes a key-value pair from the hash table
 * Does nothing if key doesn't exist
 */
void hash_table_remove(HashTable* table, const void* key) {
    if (!table) {
        fprintf(stderr, "Error: Cannot remove from NULL hash table\n");
        return;
    }
    
    if (!key) {
        fprintf(stderr, "Error: Cannot remove NULL key from hash table\n");
        return;
    }
    
    unsigned long hash = table->hash_func(key);
    int index = hash % table->capacity;
    
    HashNode* node = table->buckets[index];
    HashNode* prev = NULL;
    
    // Search for the node to remove
    while (node) {
        if (table->compare_func(node->key, key) == 0) {
            // Remove the node from the chain
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

/**
 * Returns the number of key-value pairs in the hash table
 */
int hash_table_size(const HashTable* table) {
    if (!table) {
        fprintf(stderr, "Error: Cannot get size of NULL hash table\n");
        return 0;
    }
    
    return table->size;
}

/**
 * Returns an array of all keys in the hash table
 * Caller is responsible for freeing the returned array
 */
void** hash_table_keyset(const HashTable* table) {
    if (!table) {
        fprintf(stderr, "Error: Cannot get keyset from NULL hash table\n");
        return NULL;
    }
    
    if (table->size == 0) {
        return NULL;
    }
    
    void** keys = (void**)malloc(table->size * sizeof(void*));
    if (!keys) {
        fprintf(stderr, "Error: Memory allocation failed for hash table keyset\n");
        return NULL;
    }
    
    int key_index = 0;
    // Iterate through all buckets and collect keys
    for (int i = 0; i < table->capacity; i++) {
        HashNode* node = table->buckets[i];
        while (node) {
            if (key_index >= table->size) {
                fprintf(stderr, "Error: Inconsistent hash table state during keyset creation\n");
                free(keys);
                return NULL;
            }
            keys[key_index++] = node->key;
            node = node->next;
        }
    }
    
    return keys;
}

/**
 * Frees all memory allocated for the hash table
 * Does not free the actual keys and values, only the table structure
 */
void hash_table_free(HashTable* table) {
    if (!table) {
        return;
    }
    
    // Free all nodes in all buckets
    for (int i = 0; i < table->capacity; i++) {
        HashNode* node = table->buckets[i];
        while (node) {
            HashNode* next = node->next;
            hash_node_free(node);
            node = next;
        }
    }
    
    // Free the bucket array and table structure
    free(table->buckets);
    free(table);
}
