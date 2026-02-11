#include "../lib/unity/unity.h"
#include "record.h"
#include "sort.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

// Test compare_record function with field1 (string)
void test_compare_field1_string(void) {
    Record r1 = {1, "apple", 10, 1.5f};
    Record r2 = {2, "banana", 20, 2.5f};
    Record r3 = {3, "apple", 30, 3.5f};

    set_compare_field(1);
    
    // "apple" < "banana"
    TEST_ASSERT_TRUE(compare_record(&r1, &r2) < 0);
    TEST_ASSERT_TRUE(compare_record(&r2, &r1) > 0);
    
    // "apple" == "apple"
    TEST_ASSERT_EQUAL_INT(0, compare_record(&r1, &r3));
}

// Test compare_record function with field2 (int)
void test_compare_field2_integer(void) {
    Record r1 = {1, "test", 5, 1.0f};
    Record r2 = {2, "test", 10, 2.0f};
    Record r3 = {3, "test", 5, 3.0f};

    set_compare_field(2);
    
    // 5 < 10
    TEST_ASSERT_TRUE(compare_record(&r1, &r2) < 0);
    TEST_ASSERT_TRUE(compare_record(&r2, &r1) > 0);
    
    // 5 == 5
    TEST_ASSERT_EQUAL_INT(0, compare_record(&r1, &r3));
}

// Test compare_record function with field3 (float)
void test_compare_field3_float(void) {
    Record r1 = {1, "test", 1, 1.5f};
    Record r2 = {2, "test", 2, 2.5f};
    Record r3 = {3, "test", 3, 1.5f};

    set_compare_field(3);
    
    // 1.5 < 2.5
    TEST_ASSERT_TRUE(compare_record(&r1, &r2) < 0);
    TEST_ASSERT_TRUE(compare_record(&r2, &r1) > 0);
    
    // 1.5 == 1.5
    TEST_ASSERT_EQUAL_INT(0, compare_record(&r1, &r3));
}

// Test invalid field (should return 0)
void test_compare_invalid_field(void) {
    Record r1 = {1, "test", 1, 1.0f};
    Record r2 = {2, "test", 2, 2.0f};

    set_compare_field(0);  // Invalid field
    TEST_ASSERT_EQUAL_INT(0, compare_record(&r1, &r2));
    
    set_compare_field(4);  // Invalid field
    TEST_ASSERT_EQUAL_INT(0, compare_record(&r1, &r2));
}

// Test merge_sort with empty array
void test_merge_sort_empty_array(void) {
    int *arr = NULL;
    // Should not crash
    merge_sort(arr, 0, sizeof(int), NULL);
    TEST_ASSERT_TRUE(1); // If we reach here, test passed
}

// Test merge_sort with single element
void test_merge_sort_single_element(void) {
    int arr[] = {42};
    merge_sort(arr, 1, sizeof(int), NULL);
    TEST_ASSERT_EQUAL_INT(42, arr[0]);
}

// Comparison function for integers (for sorting tests)
int compare_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// Test merge_sort with integer array
void test_merge_sort_integers(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int expected[] = {11, 12, 22, 25, 34, 64, 90};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    merge_sort(arr, n, sizeof(int), compare_int);
    
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(expected[i], arr[i]);
    }
}

// Test quick_sort with empty array
void test_quick_sort_empty_array(void) {
    int *arr = NULL;
    // Should not crash
    quick_sort(arr, 0, sizeof(int), NULL);
    TEST_ASSERT_TRUE(1); // If we reach here, test passed
}

// Test quick_sort with single element
void test_quick_sort_single_element(void) {
    int arr[] = {42};
    quick_sort(arr, 1, sizeof(int), NULL);
    TEST_ASSERT_EQUAL_INT(42, arr[0]);
}

// Test quick_sort with integer array
void test_quick_sort_integers(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int expected[] = {11, 12, 22, 25, 34, 64, 90};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    quick_sort(arr, n, sizeof(int), compare_int);
    
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(expected[i], arr[i]);
    }
}

// Test merge_sort with Record structures
void test_merge_sort_records(void) {
    Record records[] = {
        {3, "charlie", 30, 3.0f},
        {1, "alice", 10, 1.0f},
        {2, "bob", 20, 2.0f}
    };
    
    set_compare_field(1); // Sort by field1 (string)
    merge_sort(records, 3, sizeof(Record), compare_record);
    
    TEST_ASSERT_EQUAL_STRING("alice", records[0].field1);
    TEST_ASSERT_EQUAL_STRING("bob", records[1].field1);
    TEST_ASSERT_EQUAL_STRING("charlie", records[2].field1);
}

// Test quick_sort with Record structures
void test_quick_sort_records(void) {
    Record records[] = {
        {3, "charlie", 30, 3.0f},
        {1, "alice", 10, 1.0f},
        {2, "bob", 20, 2.0f}
    };
    
    set_compare_field(2); // Sort by field2 (int)
    quick_sort(records, 3, sizeof(Record), compare_record);
    
    TEST_ASSERT_EQUAL_INT(10, records[0].field2);
    TEST_ASSERT_EQUAL_INT(20, records[1].field2);
    TEST_ASSERT_EQUAL_INT(30, records[2].field2);
}

// Test sorting already sorted array
void test_sorting_already_sorted(void) {
    int arr[] = {1, 2, 3, 4, 5};
    int original[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    merge_sort(arr, n, sizeof(int), compare_int);
    
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(original[i], arr[i]);
    }
}

// Test sorting reverse sorted array
void test_sorting_reverse_sorted(void) {
    int arr[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    quick_sort(arr, n, sizeof(int), compare_int);
    
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(expected[i], arr[i]);
    }
}

// Test sorting array with duplicate elements
void test_sorting_duplicates(void) {
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    int expected[] = {1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    merge_sort(arr, n, sizeof(int), compare_int);
    
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(expected[i], arr[i]);
    }
}

// Test algorithm stability with Record structures
void test_sorting_stability(void) {
    Record records[] = {
        {1, "alice", 10, 1.0f},
        {2, "bob", 10, 2.0f},
        {3, "charlie", 10, 3.0f}
    };
    
    set_compare_field(2); // Sort by field2 (all have value 10)
    merge_sort(records, 3, sizeof(Record), compare_record);
    
    // Original order should be preserved for equal elements
    TEST_ASSERT_EQUAL_INT(1, records[0].id);
    TEST_ASSERT_EQUAL_INT(2, records[1].id);
    TEST_ASSERT_EQUAL_INT(3, records[2].id);
}

int main(void) {
    UNITY_BEGIN();
    
    // Tests for compare_record function
    RUN_TEST(test_compare_field1_string);
    RUN_TEST(test_compare_field2_integer);
    RUN_TEST(test_compare_field3_float);
    RUN_TEST(test_compare_invalid_field);
    
    // Tests for merge_sort algorithm
    RUN_TEST(test_merge_sort_empty_array);
    RUN_TEST(test_merge_sort_single_element);
    RUN_TEST(test_merge_sort_integers);
    RUN_TEST(test_merge_sort_records);
    
    // Tests for quick_sort algorithm
    RUN_TEST(test_quick_sort_empty_array);
    RUN_TEST(test_quick_sort_single_element);
    RUN_TEST(test_quick_sort_integers);
    RUN_TEST(test_quick_sort_records);
    
    // Tests for special cases
    RUN_TEST(test_sorting_already_sorted);
    RUN_TEST(test_sorting_reverse_sorted);
    RUN_TEST(test_sorting_duplicates);
    RUN_TEST(test_sorting_stability);
    
    return UNITY_END();
}
