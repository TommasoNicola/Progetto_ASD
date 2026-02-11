#include "record.h"
#include "sort.h"
#include <stdlib.h>
#include <string.h>

static int selected_field = 1;

// Function to set the field to compare records by
void set_compare_field(int field) {
    selected_field = field;
}

int compare_record(const void *a, const void *b) {
    const Record *ra = (const Record *)a;
    const Record *rb = (const Record *)b;

    // Compare based on the selected field
    switch (selected_field) {
        case 1:
            return strcmp(ra->field1, rb->field1);
        case 2:
            return ra->field2 - rb->field2;
        case 3:
            return (ra->field3 > rb->field3) - (ra->field3 < rb->field3);
        default:
            return 0;
    }
}

// Function to sort records from an input file and write them to an output file
void sort_records(FILE *infile, FILE *outfile, size_t field, size_t algo) {
    printf("Sorting by field %zu using algorithm %zu\n", field, algo);
    // Allocate memory for records
    Record *records = malloc(sizeof(Record) * 20000000);
    if (!records) return;

    // Read records from the input file
    size_t count = 0;
    while (fscanf(infile, "%d,%127[^,],%d,%f\n", &records[count].id, records[count].field1,
                  &records[count].field2, &records[count].field3) == 4) {
        count++;
    }

    set_compare_field(field);

    if (algo == 1)
        merge_sort(records, count, sizeof(Record), compare_record);
    else
        quick_sort(records, count, sizeof(Record), compare_record);

    // Write sorted records to the output file
    for (size_t i = 0; i < count; ++i) {
        fprintf(outfile, "%d,%s,%d,%f\n", records[i].id, records[i].field1, records[i].field2, records[i].field3);
    }

    free(records);
}
