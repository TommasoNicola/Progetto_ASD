#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"

#define MAX_WORD_LENGTH 100

// Structure to store word frequency
typedef struct {
    char* word;
    int frequency;
} WordFrequency;

// Comparison function for string keys
int string_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

// Hash function for strings (djb2 algorithm)
unsigned long string_hash(const void* key) {
    const char* str = (const char*)key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

// Clean a word: remove punctuation and convert to lowercase
void clean_word(char* word) {
    int j = 0;
    for (int i = 0; word[i]; i++) {
        if (isalpha((unsigned char)word[i])) {
            word[j++] = tolower((unsigned char)word[i]);
        }
    }
    word[j] = '\0';
}

// Check if a word meets the minimum length requirement
int is_valid_word(const char* word, int min_length) {
    return strlen(word) >= (size_t)min_length;
}

// Create a copy of a string (helper function)
char* string_duplicate(const char* str) {
    size_t len = strlen(str);
    char* copy = malloc(len + 1);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

// Process the text file and count word frequencies
void process_file(const char* filename, int min_length, HashTable* word_counts) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char word[MAX_WORD_LENGTH];
    int total_words = 0;
    int valid_words = 0;

    printf("Processing file: %s\n", filename);
    printf("Minimum word length: %d\n\n", min_length);

    while (fscanf(file, "%99s", word) == 1) {
        total_words++;
        clean_word(word);

        if (strlen(word) == 0) continue;
        
        if (is_valid_word(word, min_length)) {
            valid_words++;
            
            // Check if word already exists in hash table
            int* current_count = (int*)hash_table_get(word_counts, word);
            
            if (current_count) {
                // Word exists, increment count
                (*current_count)++;
            } else {
                // New word, add to hash table
                char* word_copy = string_duplicate(word);
                int* count = malloc(sizeof(int));
                if (word_copy && count) {
                    *count = 1;
                    hash_table_put(word_counts, word_copy, count);
                } else {
                    // Handle allocation failure
                    free(word_copy);
                    free(count);
                    fprintf(stderr, "Error: memory allocation failed\n");
                }
            }
        }
    }

    fclose(file);
    
    printf("Statistics:\n");
    printf("- Total words processed: %d\n", total_words);
    printf("- Words meeting length requirement: %d\n", valid_words);
    printf("- Unique words in hash table: %d\n\n", hash_table_size(word_counts));
}

// Find the most frequent word
char* find_most_frequent_word(HashTable* word_counts) {
    if (hash_table_size(word_counts) == 0) {
        return NULL;
    }

    void** keys = hash_table_keyset(word_counts);
    if (!keys) return NULL;

    char* most_frequent_word = NULL;
    int max_frequency = 0;
    int table_size = hash_table_size(word_counts);

    for (int i = 0; i < table_size; i++) {
        char* word = (char*)keys[i];
        int* frequency = (int*)hash_table_get(word_counts, word);
        
        if (frequency && *frequency > max_frequency) {
            max_frequency = *frequency;
            most_frequent_word = word;
        }
    }

    free(keys);
    return most_frequent_word;
}

// Print top N most frequent words
void print_top_words(HashTable* word_counts, int top_n) {
    int table_size = hash_table_size(word_counts);
    if (table_size == 0) {
        printf("No words found.\n");
        return;
    }

    void** keys = hash_table_keyset(word_counts);
    if (!keys) return;

    // Create array of WordFrequency structures
    WordFrequency* word_freqs = malloc(table_size * sizeof(WordFrequency));
    if (!word_freqs) {
        free(keys);
        return;
    }

    // Fill the array
    for (int i = 0; i < table_size; i++) {
        word_freqs[i].word = (char*)keys[i];
        int* frequency = (int*)hash_table_get(word_counts, keys[i]);
        word_freqs[i].frequency = frequency ? *frequency : 0;
    }

    // Simple bubble sort by frequency (descending)
    for (int i = 0; i < table_size - 1; i++) {
        for (int j = 0; j < table_size - i - 1; j++) {
            if (word_freqs[j].frequency < word_freqs[j + 1].frequency) {
                WordFrequency temp = word_freqs[j];
                word_freqs[j] = word_freqs[j + 1];
                word_freqs[j + 1] = temp;
            }
        }
    }

    // Print top N words
    int limit = (top_n < table_size) ? top_n : table_size;
    printf("Top %d most frequent words:\n", limit);
    for (int i = 0; i < limit; i++) {
        printf("%2d. %-20s (frequency: %d)\n", 
               i + 1, word_freqs[i].word, word_freqs[i].frequency);
    }

    free(word_freqs);
    free(keys);
}

// Free all allocated memory in the hash table
void cleanup_hash_table(HashTable* table) {
    if (!table) return;

    void** keys = hash_table_keyset(table);
    if (keys) {
        int size = hash_table_size(table);
        for (int i = 0; i < size; i++) {
            char* word = (char*)keys[i];
            int* frequency = (int*)hash_table_get(table, word);
            free(word);      // Free the duplicated word string
            free(frequency); // Free the frequency counter
        }
        free(keys);
    }
    hash_table_free(table);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <text_file> <min_word_length>\n", argv[0]);
        fprintf(stderr, "Example: %s /tmp/data/iliade.txt 6\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    int min_length = atoi(argv[2]);

    if (min_length < 1) {
        fprintf(stderr, "Error: minimum word length must be at least 1\n");
        return EXIT_FAILURE;
    }

    // Create hash table for word counting
    HashTable* word_counts = hash_table_create(string_compare, string_hash);
    if (!word_counts) {
        fprintf(stderr, "Error: failed to create hash table\n");
        return EXIT_FAILURE;
    }

    // Process the input file
    process_file(filename, min_length, word_counts);

    // Find and display the most frequent word
    char* most_frequent = find_most_frequent_word(word_counts);
    if (most_frequent) {
        int* frequency = (int*)hash_table_get(word_counts, most_frequent);
        printf("Most frequent word: '%s' (frequency: %d)\n\n", 
               most_frequent, frequency ? *frequency : 0);
    } else {
        printf("No words found meeting the criteria.\n\n");
    }

    // Show top 10 most frequent words
    print_top_words(word_counts, 10);

    // Clean up allocated memory
    cleanup_hash_table(word_counts);

    return EXIT_SUCCESS;
}
