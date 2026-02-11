#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

/** 
 * Sorts the array pointed to by `base` using the merge sort algorithm.
 * 
 * @param base    A pointer to the first element of the array to sort.
 * @param nitems  The number of elements in the array to sort.
 * @param size    The size in bytes of each element in the array.
 * @param compar  A pointer to a comparison function that determines the sort order.
 *
 * The comparison function `compar` defines the sorting criterion. It must accept as input
 * two `const void *` pointers to elements of the array and return:
 * - a negative value if the first element is less than the second;
 * - zero if the two elements are equal;
 * - a positive value if the first element is greater than the second.
 *
 * This function allows sorting of arbitrary data types by providing the size of each element
 * and a suitable comparison function. The `base` parameter must not be NULL.
 */
void merge_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

/** 
 * Sorts the array pointed to by `base` using the quick sort algorithm.
 * 
 * @param base    A pointer to the first element of the array to sort.
 * @param nitems  The number of elements in the array to sort.
 * @param size    The size in bytes of each element in the array.
 * @param compar  A pointer to a comparison function that determines the sort order.
 *
 * The comparison function `compar` defines the sorting criterion. It must accept as input
 * two `const void *` pointers to elements of the array and return:
 * - a negative value if the first element is less than the second;
 * - zero if the two elements are equal;
 * - a positive value if the first element is greater than the second.
 *
 * This function allows sorting of arbitrary data types by providing the size of each element
 * and a suitable comparison function. The `base` parameter must not be NULL.
 */
void quick_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

#endif // SORT_H
