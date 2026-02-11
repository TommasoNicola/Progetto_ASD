#include <string.h>
#include "sort.h"

// Function to swap two elements in an array
static void swap(void *a, void *b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

// Function to partition the array for quick sor
static size_t partition(void *base, size_t low, size_t high, size_t size, 
                        int (*compar)(const void *, const void *)) {
    
    // Choose the last element as the pivot
    char *arr = (char *)base;
    void *pivot = arr + high * size;
    
    // Index i starts from low
    size_t i = low;
    
    // Iterate through the array from low to high - 1
    for (size_t j = low; j < high; j++) {
        void *current = arr + j * size;
        
        // If the current element is less than or equal to the pivot, swap it with the element at index i
        if (compar(current, pivot) <= 0) {
            swap(arr + i * size, arr + j * size, size);
            i++;
        }
    }

    // Swap the pivot element with the element at index i
    swap(arr + i * size, arr + high * size, size);
    
    // Return the index of the pivot
    return i;
}

// Recursive function to perform quick sort
static void quick_sort_recursive(void *base, size_t low, size_t high, size_t size, 
                                 int (*compar)(const void *, const void *)) {
    if (low < high) {
      size_t pivot_index = partition(base, low, high, size, compar);

      // Sort the elements before pivot
      if (pivot_index > 0) {
          quick_sort_recursive(base, low, pivot_index - 1, size, compar);
      }
      // Sort the elements after pivot
      quick_sort_recursive(base, pivot_index + 1, high, size, compar);
    }
}

// Quick sort function
void quick_sort(void *base, size_t nitems, size_t size,
                int (*compar)(const void *, const void *)) {
    // Base case: if the array has 0 or 1 item, or invalid parameters
    if (nitems < 2 || base == NULL || size == 0 || compar == NULL) return;

    // Call the recursive quick sort function
    quick_sort_recursive(base, 0, nitems - 1, size, compar);
}
