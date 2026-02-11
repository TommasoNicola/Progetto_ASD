#include <string.h>
#include "sort.h"

// Merge two sorted halves into a temporary array, then copy back into base
static void merge(void *base, size_t size, int (*compar)(const void *, const void *),
                  char *left, size_t left_count, char *right, size_t right_count) {
    // Allocate temporary array to hold merged result
    char *temp = malloc(size * (left_count + right_count));
    size_t i = 0, j = 0, k = 0;

    // Merge elements from left and right into temp
    while (i < left_count && j < right_count) {
        if (compar(left + i * size, right + j * size) <= 0)
            memcpy(temp + k++ * size, left + i++ * size, size);
        else
            memcpy(temp + k++ * size, right + j++ * size, size);
    }

    // Copy any remaining elements from left
    while (i < left_count) memcpy(temp + k++ * size, left + i++ * size, size);
    // Copy any remaining elements from right
    while (j < right_count) memcpy(temp + k++ * size, right + j++ * size, size);

    // Copy merged result back to base
    memcpy(base, temp, size * (left_count + right_count));
    free(temp);
}

// Recursive merge sort function
void merge_sort(void *base, size_t nitems, size_t size,
                int (*compar)(const void *, const void *)) {
    // Base case: if the array has 0 or 1 item, or invalid parameters
    if (nitems < 2 || base == NULL || size == 0 || compar == NULL) return;
    size_t mid = nitems / 2;
    // Cast base to char * for pointer arithmetic
    char *b = (char *)base;

    merge_sort(b, mid, size, compar);
    merge_sort(b + mid * size, nitems - mid, size, compar);
    merge(b, size, compar, b, mid, b + mid * size, nitems - mid);
}
