#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "graph.h"

/**
 * Maximum length for city names and file lines
 */
#define MAX_LINE_LENGTH 512
#define MAX_CITY_NAME_LENGTH 256

/**
 * Structure to hold CSV record data
 */
typedef struct {
    char* place1;
    char* place2;
    float distance;
} CSVRecord;

/**
 * Dynamic array to store allocated strings for cleanup
 */
typedef struct {
    char** strings;
    size_t count;
    size_t capacity;
} StringTracker;

static StringTracker string_tracker = {NULL, 0, 0};

/**
 * Initializes the string tracker
 */
void init_string_tracker() {
    string_tracker.capacity = 1000;  // Initial capacity
    string_tracker.strings = malloc(string_tracker.capacity * sizeof(char*));
    string_tracker.count = 0;
    
    if (!string_tracker.strings) {
        fprintf(stderr, "Error: Failed to initialize string tracker\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Adds a string to the tracker for later cleanup
 */
void track_allocated_string(char* str) {
    if (!str) return;
    
    // Resize array if needed
    if (string_tracker.count >= string_tracker.capacity) {
        string_tracker.capacity *= 2;
        char** new_strings = realloc(string_tracker.strings, 
                                   string_tracker.capacity * sizeof(char*));
        if (!new_strings) {
            fprintf(stderr, "Error: Failed to resize string tracker\n");
            exit(EXIT_FAILURE);
        }
        string_tracker.strings = new_strings;
    }
    
    string_tracker.strings[string_tracker.count++] = str;
}

/**
 * Frees all tracked strings and the tracker itself
 */
void free_all_tracked_strings() {
    if (string_tracker.strings) {
        for (size_t i = 0; i < string_tracker.count; i++) {
            free(string_tracker.strings[i]);
        }
        free(string_tracker.strings);
        string_tracker.strings = NULL;
        string_tracker.count = 0;
        string_tracker.capacity = 0;
    }
}

/**
 * Compares two strings for hash table operations
 * Returns 0 if strings are equal, non-zero otherwise
 */
int string_compare(const void* a, const void* b) {
    if (!a || !b) return (a == b) ? 0 : 1;
    return strcmp((const char*)a, (const char*)b);
}

/**
 * Hash function for strings using djb2 algorithm
 * Returns hash value for the given string
 */
unsigned long string_hash(const void* str) {
    if (!str) return 0;
    
    const char* s = (const char*)str;
    unsigned long hash = 5381;
    int c;
    
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

/**
 * Safely duplicates a string with error handling
 * Returns pointer to duplicated string or exits on failure
 */
char* safe_strdup(const char* src) {
    if (!src) {
        fprintf(stderr, "Error: Null string passed to safe_strdup\n");
        exit(EXIT_FAILURE);
    }
    
    size_t len = strlen(src) + 1;
    char* dest = malloc(len);
    if (!dest) {
        fprintf(stderr, "Error: Memory allocation failed for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(dest, src);
    track_allocated_string(dest);  // Track for cleanup
    return dest;
}

/**
 * Parses a CSV line and populates a CSVRecord structure
 * Returns 1 on success, 0 on failure
 */
int parse_csv_line(char* line, CSVRecord* record) {
    if (!line || !record) {
        fprintf(stderr, "Error: Invalid parameters for parse_csv_line\n");
        return 0;
    }

    // Remove trailing newline
    char* newline = strchr(line, '\n');
    if (newline) *newline = '\0';

    // Initialize output record
    record->place1 = NULL;
    record->place2 = NULL;
    record->distance = 0.0f;

    char place1[MAX_CITY_NAME_LENGTH];
    char place2[MAX_CITY_NAME_LENGTH];
    float distance;
  
    // Inizialize buffer to prevent buffer overflow
    memset(place1, 0, sizeof(place1));
    memset(place2, 0, sizeof(place2));

    // Parse the CSV line safely
    int fields = sscanf(line, " %255[^,], %255[^,], %f", place1, place2, &distance);
    if (fields != 3) {
        fprintf(stderr, "Warning: Invalid CSV format: %s\n", line);
        return 0;
    }

    // Validate distance
    if (distance < 0) {
        fprintf(stderr, "Warning: Invalid distance value: %.2f\n", distance);
        return 0;
    }

    // Validate city names
    if (strlen(place1) == 0 || strlen(place2) == 0) {
        fprintf(stderr, "Warning: Empty city names in CSV line: %s\n", line);
        return 0;
    }

    // Duplicate strings with error handling
    record->place1 = safe_strdup(place1);
    record->place2 = safe_strdup(place2);
    record->distance = distance;

    return 1;
}

/**
 * Loads graph data from CSV file and creates an undirected, labeled graph
 * Returns number of edges loaded, or -1 on error
 */
int load_graph_from_csv(const char* filename, Graph* graph) {
    if (!filename || !graph) {
        fprintf(stderr, "Error: Invalid parameters for load_graph_from_csv\n");
        exit(EXIT_FAILURE);
    }
    
    // Open the CSV file
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // Create undirected, labeled graph
    *graph = graph_create(1, 0, string_compare, string_hash);
    if (!*graph) {
        fprintf(stderr, "Error: Failed to create graph\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_LINE_LENGTH];
    int edges_loaded = 0;
    int line_number = 0;
    
    printf("Loading graph data from '%s'...\n", filename);
    
    // Process each line in the CSV file
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        // Skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#') continue;
        
        CSVRecord record;
        if (parse_csv_line(line, &record)) {
            // Convert distance to string for label storage
            char* distance_str = malloc(32);
            if (!distance_str) {
                fprintf(stderr, "Error: Memory allocation failed for distance string\n");
                fclose(file);
                graph_free(*graph);
                exit(EXIT_FAILURE);
            }
            
            snprintf(distance_str, 32, "%.2f", record.distance);
            track_allocated_string(distance_str);  // Track for cleanup
            
            // Add edge to graph (undirected graph)
            if (graph_add_edge(*graph, record.place1, record.place2, distance_str)) {
                edges_loaded++;

                // Print progress every 1000 edges
                if (edges_loaded % 1000 == 0) {
                    printf("Loaded %d edges...\n", edges_loaded);
                }
            } else {
                fprintf(stderr, "Warning: Failed to add edge %s -> %s at line %d\n", 
                       record.place1, record.place2, line_number);
            }
        } else {
            fprintf(stderr, "Warning: Failed to parse line %d\n", line_number);
        }
    }
    
    fclose(file);
    
    // Verify that we loaded at least some data
    if (edges_loaded == 0) {
        fprintf(stderr, "Error: No valid edges found in CSV file\n");
        graph_free(*graph);
        *graph = NULL;
        exit(EXIT_FAILURE);
    }
    
    printf("Graph loading completed:\n");
    printf("  - Nodes: %d\n", graph_num_nodes(*graph));
    printf("  - Edges: %d\n", graph_num_edges(*graph));
    printf("  - Lines processed: %d\n", line_number);
    
    return edges_loaded;
}

/**
 * Performs breadth-first search and saves results to output file
 * Returns 1 on success, exits on failure
 */
int perform_bfs_and_save(Graph graph, const char* start_city, const char* output_filename) {
    if (!graph || !start_city || !output_filename) {
        fprintf(stderr, "Error: Invalid parameters for BFS\n");
        exit(EXIT_FAILURE);
    }
    
    printf("\nStarting breadth-first search from '%s'...\n", start_city);
    
    // Check if starting city exists in graph
    if (!graph_contains_node(graph, start_city)) {
        fprintf(stderr, "Error: Starting city '%s' not found in graph\n", start_city);
        
        // Print available cities for debugging
        printf("Available cities (first 10):\n");
        void** nodes = graph_get_nodes(graph);
        if (nodes) {
            int num_nodes = graph_num_nodes(graph);
            for (int i = 0; i < num_nodes && i < 10; i++) {
                printf("  - %s\n", (char*)nodes[i]);
            }
            if (num_nodes > 10) {
                printf("  ... and %d more cities\n", num_nodes - 10);
            }
            free(nodes);
        }
        exit(EXIT_FAILURE);
    }
    
    // Perform breadth-first search
    void** bfs_result = breadth_first_visit(graph, (void*)start_city, string_compare, string_hash);
    if (!bfs_result) {
        fprintf(stderr, "Error: Breadth-first search failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Open output file
    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Cannot create output file '%s': %s\n", 
               output_filename, strerror(errno));
        free(bfs_result);
        exit(EXIT_FAILURE);
    }
    
    // Write BFS results to file
    int num_nodes = graph_num_nodes(graph);
    int visited_count = 0;
    
    printf("Writing BFS results to '%s'...\n", output_filename);
    
    // Write each visited node to the output file
    for (int i = 0; i < num_nodes && bfs_result[i] != NULL; i++) {
        if (fprintf(output_file, "%s\n", (char*)bfs_result[i]) < 0) {
            fprintf(stderr, "Error: Failed to write to output file\n");
            fclose(output_file);
            free(bfs_result);
            exit(EXIT_FAILURE);
        }
        visited_count++;
    }
    
    fclose(output_file);
    free(bfs_result);
    
    printf("BFS completed successfully:\n");
    printf("  - Starting city: %s\n", start_city);
    printf("  - Cities visited: %d\n", visited_count);
    printf("  - Output file: %s\n", output_filename);
    
    return 1;
}

/**
 * Prints usage information for the program
 */
void print_usage(const char* program_name) {
    printf("Usage: %s <csv_file> <start_city> <output_file>\n", program_name);
    printf("\n");
    printf("Parameters:\n");
    printf("  csv_file    : Path to the CSV file containing city distances\n");
    printf("  start_city  : Name of the city to start breadth-first search from\n");
    printf("  output_file : Path to save the BFS traversal results\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s italian_dist_graph.csv torino output.txt\n", program_name);
    printf("\n");
    printf("The CSV file should have the format:\n");
    printf("  place1,place2,distance\n");
    printf("  where place1 and place2 are city names and distance is in meters\n");
}

/**
 * Main function: loads graph from CSV, performs BFS, and saves results
 */
int main(int argc, char* argv[]) {
    // Initialize string tracker for memory management
    init_string_tracker();
    
    // Validate command line arguments
    if (argc != 4) {
        fprintf(stderr, "Error: Invalid number of arguments\n\n");
        print_usage(argv[0]);
        free_all_tracked_strings();
        exit(EXIT_FAILURE);
    }
    
    const char* csv_filename = argv[1];
    const char* start_city = argv[2];
    const char* output_filename = argv[3];
    
    // Validate input parameters
    if (!csv_filename || strlen(csv_filename) == 0) {
        fprintf(stderr, "Error: Invalid CSV filename\n");
        free_all_tracked_strings();
        exit(EXIT_FAILURE);
    }
    
    if (!start_city || strlen(start_city) == 0) {
        fprintf(stderr, "Error: Invalid start city name\n");
        free_all_tracked_strings();
        exit(EXIT_FAILURE);
    }
    
    if (!output_filename || strlen(output_filename) == 0) {
        fprintf(stderr, "Error: Invalid output filename\n");
        free_all_tracked_strings();
        exit(EXIT_FAILURE);
    }
    
    printf("Graph Analysis Tool - Exercise 4\n");
    printf("=================================\n");
    printf("CSV file: %s\n", csv_filename);
    printf("Start city: %s\n", start_city);
    printf("Output file: %s\n", output_filename);
    printf("\n");
    
    // Load graph from CSV file
    Graph graph = NULL;
    int edges_loaded = load_graph_from_csv(csv_filename, &graph);

    if (edges_loaded < 0 || !graph) {
        fprintf(stderr, "Error: Failed to load graph from CSV file\n");
        free_all_tracked_strings();
        exit(EXIT_FAILURE);
    }
    
    // Perform breadth-first search and save results
    perform_bfs_and_save(graph, start_city, output_filename);
    
    // Clean up resources
    graph_free(graph);           // Free graph first (it references the strings)
    free_all_tracked_strings();  // Then free all tracked strings
    
    printf("\nProgram completed successfully!\n");
    return EXIT_SUCCESS;
}
