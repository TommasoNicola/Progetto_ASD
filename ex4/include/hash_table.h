#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/**
 * Generic hash table implementation with chaining
 * Supports key-value pairs where keys and values can be of any type
 */

typedef struct HashTable HashTable;

/**
 * Creates a new hash table
 * 
 * @param compare_func Function to compare keys (returns 0 if equal, non-zero otherwise)
 * @param hash_func Function to compute hash value for keys
 * @return Pointer to the newly created hash table, or NULL on failure
 */
HashTable* hash_table_create(int (*compare_func)(const void*, const void*), 
                             unsigned long (*hash_func)(const void*));

/**
 * Inserts or updates a key-value pair in the hash table
 * 
 * @param table The hash table
 * @param key Pointer to the key
 * @param value Pointer to the value
 */
void hash_table_put(HashTable* table, const void* key, const void* value);

/**
 * Retrieves the value associated with a key
 * 
 * @param table The hash table
 * @param key Pointer to the key to search for
 * @return Pointer to the value if found, NULL otherwise
 */
void* hash_table_get(const HashTable* table, const void* key);

/**
 * Checks if a key exists in the hash table
 * 
 * @param table The hash table
 * @param key Pointer to the key to search for
 * @return 1 if key exists, 0 otherwise
 */
int hash_table_contains_key(const HashTable* table, const void* key);

/**
 * Removes a key-value pair from the hash table
 * 
 * @param table The hash table
 * @param key Pointer to the key to remove
 */
void hash_table_remove(HashTable* table, const void* key);

/**
 * Returns the number of key-value pairs in the hash table
 * 
 * @param table The hash table
 * @return Number of pairs in the table
 */
int hash_table_size(const HashTable* table);

/**
 * Returns an array of all keys in the hash table
 * The caller is responsible for freeing the returned array
 * 
 * @param table The hash table
 * @return Array of key pointers, NULL if table is empty
 */
void** hash_table_keyset(const HashTable* table);

/**
 * Frees the memory allocated for the hash table
 * Note: This does not free the actual keys and values, only the table structure
 * 
 * @param table The hash table to free
 */
void hash_table_free(HashTable* table);

#endif
