#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "edit_distance.h"

#define MAX_WORD_LENGTH 100
#define MAX_DICTIONARY_SIZE 661562
#define MAX_SUGGESTIONS 5

// Structure to store a word and its edit distance
typedef struct {
    char word[MAX_WORD_LENGTH];
    int distance;
} WordDistance;

// Comparison function for sorting suggestions by distance, then alphabetically
int compare_word_distance(const void *a, const void *b) {
    const WordDistance *wa = (const WordDistance *)a;
    const WordDistance *wb = (const WordDistance *)b;

    // First sort by distance
    if (wa->distance != wb->distance)
        return wa->distance - wb->distance;

    // Then alphabetically
    return strcmp(wa->word, wb->word);
}

// Clean a word: remove non-alphabetical characters and convert to lowercase
void clean_word(char *word) {
    int j = 0;
    for (int i = 0; word[i]; i++) {
        // Keep only letters, convert to lowercase
        if (isalpha((unsigned char)word[i])) {
            word[j++] = tolower((unsigned char)word[i]);
        }
    }
    word[j] = '\0';  // Null-terminate the cleaned word
}

// Check if a word exists in the dictionary
int is_in_dictionary(char dict[][MAX_WORD_LENGTH], int size, const char *word) {
    for (int i = 0; i < size; i++) {
        // Compare the word with each dictionary entry
        if (strcmp(dict[i], word) == 0)
            return 1;  // Found
    }
    return 0;  // Not found
}

// Load words from the dictionary file into memory
int load_dictionary(const char *filename, char dict[][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open dictionary file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), file) && count < MAX_DICTIONARY_SIZE) {
        line[strcspn(line, "\n")] = '\0';  // Remove newline character
        clean_word(line);  // Normalize the word
        if (strlen(line) > 0) {
            strcpy(dict[count++], line);  // Store cleaned word
        }
    }

    fclose(file);
    return count;  // Return number of words loaded
}

// Find closest dictionary words to the misspelled word using minimum edit distance
void find_closest_words(char dict[][MAX_WORD_LENGTH], int size, const char *word,
                        WordDistance *results, int *n_results) {
    int min_dist = INT_MAX;
    *n_results = 0;

    // First pass: find the minimum distance
    for (int i = 0; i < size; i++) {
        int d = edit_distance_dyn(word, dict[i]);
        if (d < min_dist)
            min_dist = d;
    }

    // Second pass: collect words at the minimum distance
    for (int i = 0; i < size && *n_results < MAX_SUGGESTIONS; i++) {
        int d = edit_distance_dyn(word, dict[i]);
        if (d == min_dist) {
            strcpy(results[*n_results].word, dict[i]);
            results[*n_results].distance = d;
            (*n_results)++;
        }
    }

    // Sort suggestions by distance and alphabetically
    qsort(results, *n_results, sizeof(WordDistance), compare_word_distance);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dictionary_file> <input_text_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dict_file = argv[1];
    const char *input_file = argv[2];

    // Allocate memory for the dictionary
    char (*dictionary)[MAX_WORD_LENGTH] = malloc(MAX_DICTIONARY_SIZE * MAX_WORD_LENGTH);
    if (!dictionary) {
        fprintf(stderr, "Error: memory allocation for dictionary failed\n");
        return EXIT_FAILURE;
    }

    // Load dictionary words
    int dict_size = load_dictionary(dict_file, dictionary);

    // Open the input text file
    FILE *input = fopen(input_file, "r");
    if (!input) {
        fprintf(stderr, "Error: cannot open input file '%s'\n", input_file);
        free(dictionary);
        return EXIT_FAILURE;
    }

    printf("Analyzing file: %s...\n\n", input_file);

    char word[MAX_WORD_LENGTH];
    int total_words = 0;
    int incorrect = 0;

    // Process each word in the input text
    while (fscanf(input, "%s", word) == 1) {
        char original[MAX_WORD_LENGTH];
        strcpy(original, word);  // Keep the original for display
        clean_word(word);        // Normalize the word (lowercase, no punctuation)

        if (strlen(word) == 0) continue;

        total_words++;

        // Check if the word is in the dictionary
        if (!is_in_dictionary(dictionary, dict_size, word)) {
            incorrect++;
            printf("Parola non trovata: '%s'\n", original);

            // Get suggestions for the misspelled word
            WordDistance suggestions[MAX_SUGGESTIONS];
            int count = 0;
            find_closest_words(dictionary, dict_size, word, suggestions, &count);

            if (count > 0) {
                // Print all suggestions on the same line
                printf("Suggerimenti (distanza %d):", suggestions[0].distance);
                for (int i = 0; i < count; i++) {
                    printf(" %s", suggestions[i].word);
                    if (i < count - 1) printf(",");
                }
                printf("\n");
            }
        }
    }

    fclose(input);
    free(dictionary);

    // Final summary
    printf("\nAnalisi completata:\n");
    printf("- Parole elaborate: %d\n", total_words);
    printf("- Parole da correggere: %d\n", incorrect);
    if (incorrect == 0) {
        printf("Nessuna correzione necessaria!\n");
    }

    return EXIT_SUCCESS;
}
