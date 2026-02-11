#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>

typedef struct {
    int id;
    char field1[128];
    int field2;
    float field3;
} Record;

/* Function to set the field to compare records by.
 * 
 * @param field The index of the field to compare (0 for id, 1 for field1, etc.).
 */
void set_compare_field(int field);

/* Function to read a record from a file.
 * 
 * @param file Pointer to the file to read from.
 * @param record Pointer to the Record structure to fill with data.
 * @return 0 on success, -1 on failure.
 */
int compare_record(const void *a, const void *b);

/* * Function to sort records from an input file and write them to an output file.
 * 
 * @param infile  Pointer to the input file containing records.
 * @param outfile Pointer to the output file where sorted records will be written.
 * @param field   The field index to sort by (0 for id, 1 for field1, etc.).
 * @param algo    The sorting algorithm to use (0 for merge sort, 1 for quick sort).
 */
void sort_records(FILE *infile, FILE *outfile, size_t field, size_t algo);

#endif
