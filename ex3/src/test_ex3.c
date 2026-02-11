#include "../lib/unity/unity.h"
#include "hash_table.h"
#include <string.h>
#include <stdlib.h>

// Helper functions for testing
int string_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

unsigned long string_hash(const void* key) {
    const char* str = (const char*)key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

int int_compare(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return ia - ib;
}

unsigned long int_hash(const void* key) {
    int k = *(const int*)key;
    return (unsigned long)k;
}

// Global test variables
HashTable* table;

// Called before each test
void setUp(void) {
    table = hash_table_create(string_compare, string_hash);
    TEST_ASSERT_NOT_NULL(table);
}

// Called after each test
void tearDown(void) {
    if (table) {
        hash_table_free(table);
        table = NULL;
    }
}

// Test hash table creation
void test_hash_table_create_valid(void) {
    HashTable* test_table = hash_table_create(string_compare, string_hash);
    TEST_ASSERT_NOT_NULL(test_table);
    TEST_ASSERT_EQUAL_INT(0, hash_table_size(test_table));
    hash_table_free(test_table);
}

void test_hash_table_create_null_params(void) {
    HashTable* test_table1 = hash_table_create(NULL, string_hash);
    HashTable* test_table2 = hash_table_create(string_compare, NULL);
    HashTable* test_table3 = hash_table_create(NULL, NULL);
    
    TEST_ASSERT_NULL(test_table1);
    TEST_ASSERT_NULL(test_table2);
    TEST_ASSERT_NULL(test_table3);
}

// Test hash table put and get operations
void test_hash_table_put_get_single(void) {
    char* key = "test_key";
    char* value = "test_value";
    
    hash_table_put(table, key, value);
    
    TEST_ASSERT_EQUAL_INT(1, hash_table_size(table));
    TEST_ASSERT_EQUAL_STRING(value, (char*)hash_table_get(table, key));
}

void test_hash_table_put_get_multiple(void) {
    char* keys[] = {"key1", "key2", "key3"};
    char* values[] = {"value1", "value2", "value3"};
    
    for (int i = 0; i < 3; i++) {
        hash_table_put(table, keys[i], values[i]);
    }
    
    TEST_ASSERT_EQUAL_INT(3, hash_table_size(table));
    
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_STRING(values[i], (char*)hash_table_get(table, keys[i]));
    }
}

void test_hash_table_put_update_existing(void) {
    char* key = "update_key";
    char* value1 = "old_value";
    char* value2 = "new_value";
    
    hash_table_put(table, key, value1);
    TEST_ASSERT_EQUAL_INT(1, hash_table_size(table));
    TEST_ASSERT_EQUAL_STRING(value1, (char*)hash_table_get(table, key));
    
    hash_table_put(table, key, value2);
    TEST_ASSERT_EQUAL_INT(1, hash_table_size(table));
    TEST_ASSERT_EQUAL_STRING(value2, (char*)hash_table_get(table, key));
}

void test_hash_table_get_nonexistent(void) {
    TEST_ASSERT_NULL(hash_table_get(table, "nonexistent"));
}

// Test hash table contains_key
void test_hash_table_contains_key_existing(void) {
    char* key = "existing_key";
    char* value = "some_value";
    
    hash_table_put(table, key, value);
    TEST_ASSERT_TRUE(hash_table_contains_key(table, key));
}

void test_hash_table_contains_key_nonexistent(void) {
    TEST_ASSERT_FALSE(hash_table_contains_key(table, "nonexistent_key"));
}

// Test hash table remove
void test_hash_table_remove_existing(void) {
    char* key = "remove_key";
    char* value = "remove_value";
    
    hash_table_put(table, key, value);
    TEST_ASSERT_EQUAL_INT(1, hash_table_size(table));
    TEST_ASSERT_TRUE(hash_table_contains_key(table, key));
    
    hash_table_remove(table, key);
    TEST_ASSERT_EQUAL_INT(0, hash_table_size(table));
    TEST_ASSERT_FALSE(hash_table_contains_key(table, key));
}

void test_hash_table_remove_nonexistent(void) {
    hash_table_put(table, "keep_key", "keep_value");
    int initial_size = hash_table_size(table);
    
    hash_table_remove(table, "nonexistent_key");
    TEST_ASSERT_EQUAL_INT(initial_size, hash_table_size(table));
}

// Test hash table size
void test_hash_table_size_empty(void) {
    TEST_ASSERT_EQUAL_INT(0, hash_table_size(table));
}

void test_hash_table_size_after_operations(void) {
    char* keys[] = {"a", "b", "c", "d", "e"};
    char* values[] = {"1", "2", "3", "4", "5"};
    
    // Add elements
    for (int i = 0; i < 5; i++) {
        hash_table_put(table, keys[i], values[i]);
        TEST_ASSERT_EQUAL_INT(i + 1, hash_table_size(table));
    }
    
    // Remove elements
    for (int i = 0; i < 3; i++) {
        hash_table_remove(table, keys[i]);
        TEST_ASSERT_EQUAL_INT(5 - i - 1, hash_table_size(table));
    }
}

// Test hash table keyset
void test_hash_table_keyset_empty(void) {
    void** keys = hash_table_keyset(table);
    TEST_ASSERT_NULL(keys);
}

void test_hash_table_keyset_populated(void) {
    char* test_keys[] = {"key1", "key2", "key3"};
    char* values[] = {"val1", "val2", "val3"};
    
    for (int i = 0; i < 3; i++) {
        hash_table_put(table, test_keys[i], values[i]);
    }
    
    void** keys = hash_table_keyset(table);
    TEST_ASSERT_NOT_NULL(keys);
    
    // Check that all original keys are present
    int found_count = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (strcmp(test_keys[i], (char*)keys[j]) == 0) {
                found_count++;
                break;
            }
        }
    }
    
    TEST_ASSERT_EQUAL_INT(3, found_count);
    free(keys);
}

// Test with integer keys and values
void test_hash_table_with_integers(void) {
    HashTable* int_table = hash_table_create(int_compare, int_hash);
    TEST_ASSERT_NOT_NULL(int_table);
    
    int keys[] = {10, 20, 30};
    int values[] = {100, 200, 300};
    
    for (int i = 0; i < 3; i++) {
        hash_table_put(int_table, &keys[i], &values[i]);
    }
    
    TEST_ASSERT_EQUAL_INT(3, hash_table_size(int_table));
    
    for (int i = 0; i < 3; i++) {
        int* retrieved = (int*)hash_table_get(int_table, &keys[i]);
        TEST_ASSERT_NOT_NULL(retrieved);
        TEST_ASSERT_EQUAL_INT(values[i], *retrieved);
    }
    
    hash_table_free(int_table);
}

// Test collision handling (put many items that might collide)
void test_hash_table_collision_handling(void) {
    // Add many items to force some collisions
    char keys[50][10];
    char values[50][20];
    
    for (int i = 0; i < 50; i++) {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        hash_table_put(table, keys[i], values[i]);
    }
    
    TEST_ASSERT_EQUAL_INT(50, hash_table_size(table));
    
    // Verify all items are retrievable
    for (int i = 0; i < 50; i++) {
        char* retrieved = (char*)hash_table_get(table, keys[i]);
        TEST_ASSERT_NOT_NULL(retrieved);
        TEST_ASSERT_EQUAL_STRING(values[i], retrieved);
    }
}

// Test null parameter handling
void test_hash_table_null_parameters(void) {
    hash_table_put(NULL, "key", "value");  // Should not crash
    hash_table_put(table, NULL, "value");  // Should not crash
    
    TEST_ASSERT_NULL(hash_table_get(NULL, "key"));
    TEST_ASSERT_NULL(hash_table_get(table, NULL));
    
    TEST_ASSERT_FALSE(hash_table_contains_key(NULL, "key"));
    TEST_ASSERT_FALSE(hash_table_contains_key(table, NULL));
    
    hash_table_remove(NULL, "key");  // Should not crash
    hash_table_remove(table, NULL);  // Should not crash
    
    TEST_ASSERT_EQUAL_INT(0, hash_table_size(NULL));
    
    TEST_ASSERT_NULL(hash_table_keyset(NULL));
    
    hash_table_free(NULL);  // Should not crash
}

int main(void) {
    UNITY_BEGIN();

    // Creation tests
    RUN_TEST(test_hash_table_create_valid);
    RUN_TEST(test_hash_table_create_null_params);

    // Put/Get tests
    RUN_TEST(test_hash_table_put_get_single);
    RUN_TEST(test_hash_table_put_get_multiple);
    RUN_TEST(test_hash_table_put_update_existing);
    RUN_TEST(test_hash_table_get_nonexistent);

    // Contains key tests
    RUN_TEST(test_hash_table_contains_key_existing);
    RUN_TEST(test_hash_table_contains_key_nonexistent);

    // Remove tests
    RUN_TEST(test_hash_table_remove_existing);
    RUN_TEST(test_hash_table_remove_nonexistent);

    // Size tests
    RUN_TEST(test_hash_table_size_empty);
    RUN_TEST(test_hash_table_size_after_operations);

    // Keyset tests
    RUN_TEST(test_hash_table_keyset_empty);
    RUN_TEST(test_hash_table_keyset_populated);

    // Advanced tests
    RUN_TEST(test_hash_table_with_integers);
    RUN_TEST(test_hash_table_collision_handling);
    RUN_TEST(test_hash_table_null_parameters);

    return UNITY_END();
}
