#include <stdio.h>
#include <stdlib.h>
#include "record.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input.csv> <output.csv> <field> <algo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int field = atoi(argv[3]);
    int algo = atoi(argv[4]);

    if (field < 1 || field > 3) {
        fprintf(stderr, "Error: field must be 1, 2, or 3\n");
        exit(EXIT_FAILURE);
    }

    if (algo != 1 && algo != 2) {
        fprintf(stderr, "Error: algorithm must be 1 (merge) or 2 (quick)\n");
        exit(EXIT_FAILURE);
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Error: Unable to open input file '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Unable to open output file '%s'\n", argv[2]);
        fclose(in);
        exit(EXIT_FAILURE);
    }

    // Start the sorting process
    sort_records(in, out, field, algo);

    fclose(in);
    fclose(out);

    exit(EXIT_SUCCESS);
}
