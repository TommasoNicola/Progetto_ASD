#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "hash_table.h"

// Structure to represent an adjacency list node
typedef struct AdjNode {
    void* dest;
    void* label;
    struct AdjNode* next;
} AdjNode;

// Structure to represent an adjacency list
typedef struct AdjList {
    AdjNode* head;
    int size;
} AdjList;

// Graph structure using hash table for sparse representation
struct graph {
    HashTable* nodes;          // Hash table mapping nodes to their adjacency lists
    int labelled;              // 1 if labelled, 0 otherwise
    int directed;              // 1 if directed, 0 otherwise
    int num_nodes;             // Number of nodes in the graph
    int (*compare)(const void*, const void*);
    unsigned long (*hash)(const void*);
};

/**
 * Creates a new adjacency list node with destination and label
 */
static AdjNode* adj_node_create(void* dest, void* label) {
    AdjNode* node = (AdjNode*)malloc(sizeof(AdjNode));
    if (!node) {
        fprintf(stderr, "Error: Failed to allocate memory for adjacency node\n");
        return NULL;
    }
    
    node->dest = dest;
    node->label = label;
    node->next = NULL;
    return node;
}

/**
 * Frees memory allocated for an adjacency list node
 */
static void adj_node_free(AdjNode* node) {
    if (node) {
        free(node);
    }
}

/**
 * Creates a new empty adjacency list
 */
static AdjList* adj_list_create() {
    AdjList* list = (AdjList*)malloc(sizeof(AdjList));
    if (!list) {
        fprintf(stderr, "Error: Failed to allocate memory for adjacency list\n");
        return NULL;
    }
    
    list->head = NULL;
    list->size = 0;
    return list;
}

/**
 * Frees all nodes in an adjacency list and the list itself
 */
static void adj_list_free(AdjList* list) {
    if (!list) return;
    
    AdjNode* current = list->head;
    while (current) {
        AdjNode* next = current->next;
        adj_node_free(current);
        current = next;
    }
    free(list);
}

/**
 * Adds an edge to an adjacency list or updates existing edge label
 */
static int adj_list_add_edge(AdjList* list, void* dest, void* label, 
                            int (*compare)(const void*, const void*)) {
    if (!list || !compare) return 0;
    
    // Check if edge already exists and update label
    AdjNode* current = list->head;
    while (current) {
        if (compare(current->dest, dest) == 0) {
            current->label = label;
            return 1;
        }
        current = current->next;
    }
    
    // Add new edge at the beginning of the list
    AdjNode* new_node = adj_node_create(dest, label);
    if (!new_node) return 0;
    
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
    return 1;
}

/**
 * Removes an edge from an adjacency list
 */
static int adj_list_remove_edge(AdjList* list, void* dest, 
                               int (*compare)(const void*, const void*)) {
    if (!list || !compare) return 0;
    
    AdjNode* current = list->head;
    AdjNode* prev = NULL;
    
    while (current) {
        if (compare(current->dest, dest) == 0) {
            // Remove node from list
            if (prev) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }
            adj_node_free(current);
            list->size--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    
    return 0;
}

/**
 * Checks if an edge exists in an adjacency list
 */
static int adj_list_contains_edge(const AdjList* list, void* dest,
                                 int (*compare)(const void*, const void*)) {
    if (!list || !compare) return 0;
    
    AdjNode* current = list->head;
    while (current) {
        if (compare(current->dest, dest) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/**
 * Gets the label of an edge in an adjacency list
 */
static void* adj_list_get_label(const AdjList* list, void* dest,
                               int (*compare)(const void*, const void*)) {
    if (!list || !compare) return NULL;
    
    AdjNode* current = list->head;
    while (current) {
        if (compare(current->dest, dest) == 0) {
            return current->label;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Creates a new graph with specified properties
 */
Graph graph_create(int labelled, int directed,
                   int (*compare)(const void*, const void*),
                   unsigned long (*hash)(const void*)) {
    if (!compare || !hash) {
        fprintf(stderr, "Error: Compare and hash functions cannot be NULL\n");
        return NULL;
    }
    
    struct graph* gr = (struct graph*)malloc(sizeof(struct graph));
    if (!gr) {
        fprintf(stderr, "Error: Failed to allocate memory for graph structure\n");
        return NULL;
    }
    
    // Create hash table for nodes
    gr->nodes = hash_table_create(compare, hash);
    if (!gr->nodes) {
        fprintf(stderr, "Error: Failed to create hash table for graph nodes\n");
        free(gr);
        return NULL;
    }
    
    gr->labelled = labelled;
    gr->directed = directed;
    gr->num_nodes = 0;
    gr->compare = compare;
    gr->hash = hash;
    
    return gr;
}

/**
 * Returns whether the graph is directed
 */
int graph_is_directed(const Graph gr) {
    return gr ? gr->directed : 0;
}

/**
 * Returns whether the graph is labelled
 */
int graph_is_labelled(const Graph gr) {
    return gr ? gr->labelled : 0;
}

/**
 * Adds a node to the graph if it doesn't already exist
 */
int graph_add_node(Graph gr, const void* node) {
    if (!gr) {
        fprintf(stderr, "Error: Graph cannot be NULL\n");
        return 0;
    }
    if (!node) {
        fprintf(stderr, "Error: Node cannot be NULL\n");
        return 0;
    }
    
    // Check if node already exists
    if (hash_table_contains_key(gr->nodes, node)) {
        return 0; // Node already exists
    }
    
    // Create new adjacency list for the node
    AdjList* adj_list = adj_list_create();
    if (!adj_list) return 0;
    
    hash_table_put(gr->nodes, node, adj_list);
    gr->num_nodes++;
    return 1;
}

/**
 * Adds an edge between two nodes with optional label
 */
int graph_add_edge(Graph gr, const void* node1, const void* node2, const void* label) {
    if (!gr) {
        fprintf(stderr, "Error: Graph cannot be NULL\n");
        return 0;
    }
    if (!node1 || !node2) {
        fprintf(stderr, "Error: Nodes cannot be NULL\n");
        return 0;
    }
    
    // Validate label requirements
    if (!gr->labelled && label != NULL) {
        fprintf(stderr, "Error: Unlabelled graph cannot have edge labels\n");
        return 0;
    }
    if (gr->labelled && label == NULL) {
        fprintf(stderr, "Error: Labelled graph must have edge labels\n");
        return 0;
    }
    
    // Ensure both nodes exist in the graph
    if (!graph_contains_node(gr, node1)) {
        if (!graph_add_node(gr, node1)) return 0;
    }
    if (!graph_contains_node(gr, node2)) {
        if (!graph_add_node(gr, node2)) return 0;
    }
    
    // Add edge from node1 to node2
    AdjList* adj_list1 = (AdjList*)hash_table_get(gr->nodes, node1);
    if (!adj_list_add_edge(adj_list1, (void*)node2, (void*)label, gr->compare)) {
        return 0;
    }
    
    // For undirected graphs, add reverse edge
    if (!gr->directed) {
        AdjList* adj_list2 = (AdjList*)hash_table_get(gr->nodes, node2);
        if (!adj_list_add_edge(adj_list2, (void*)node1, (void*)label, gr->compare)) {
            // Rollback the first edge on failure
            adj_list_remove_edge(adj_list1, (void*)node2, gr->compare);
            return 0;
        }
    }
    
    return 1;
}

/**
 * Checks if a node exists in the graph
 */
int graph_contains_node(const Graph gr, const void* node) {
    if (!gr || !node) return 0;
    return hash_table_contains_key(gr->nodes, node);
}

/**
 * Checks if an edge exists between two nodes
 */
int graph_contains_edge(const Graph gr, const void* node1, const void* node2) {
    if (!gr || !node1 || !node2) return 0;
    
    AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, node1);
    if (!adj_list) return 0;
    
    return adj_list_contains_edge(adj_list, (void*)node2, gr->compare);
}

/**
 * Removes a node and all its edges from the graph
 */
int graph_remove_node(Graph gr, const void* node) {
    if (!gr) {
        fprintf(stderr, "Error: Graph cannot be NULL\n");
        return 0;
    }
    if (!node) {
        fprintf(stderr, "Error: Node cannot be NULL\n");
        return 0;
    }
    
    if (!graph_contains_node(gr, node)) {
        return 0; // Node doesn't exist
    }
    
    // Remove all edges pointing to this node
    void** all_nodes = hash_table_keyset(gr->nodes);
    if (all_nodes) {
        for (int i = 0; i < gr->num_nodes; i++) {
            if (gr->compare(all_nodes[i], node) != 0) {
                graph_remove_edge(gr, all_nodes[i], node);
            }
        }
        free(all_nodes);
    }
    
    // Remove the node's adjacency list
    AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, node);
    adj_list_free(adj_list);
    hash_table_remove(gr->nodes, node);
    gr->num_nodes--;
    
    return 1;
}

/**
 * Removes an edge between two nodes
 */
int graph_remove_edge(Graph gr, const void* node1, const void* node2) {
    if (!gr || !node1 || !node2) return 0;
    
    AdjList* adj_list1 = (AdjList*)hash_table_get(gr->nodes, node1);
    if (!adj_list1) return 0;
    
    int removed = adj_list_remove_edge(adj_list1, (void*)node2, gr->compare);
    
    // For undirected graphs, remove reverse edge
    if (!gr->directed && removed) {
        AdjList* adj_list2 = (AdjList*)hash_table_get(gr->nodes, node2);
        if (adj_list2) {
            adj_list_remove_edge(adj_list2, (void*)node1, gr->compare);
        }
    }
    
    return removed;
}

/**
 * Returns the number of nodes in the graph
 */
int graph_num_nodes(const Graph gr) {
    return gr ? gr->num_nodes : 0;
}

/**
 * Returns the total number of edges in the graph
 */
int graph_num_edges(const Graph gr) {
    if (!gr) return 0;
    
    int total_edges = 0;
    void** nodes = hash_table_keyset(gr->nodes);
    if (!nodes) return 0;
    
    // Count edges in all adjacency lists
    for (int i = 0; i < gr->num_nodes; i++) {
        AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, nodes[i]);
        if (adj_list) {
            total_edges += adj_list->size;
        }
    }
    
    free(nodes);
    
    // For undirected graphs, each edge is counted twice
    return gr->directed ? total_edges : total_edges / 2;
}

/**
 * Returns an array of all nodes in the graph
 */
void** graph_get_nodes(const Graph gr) {
    if (!gr || gr->num_nodes == 0) return NULL;
    return hash_table_keyset(gr->nodes);
}

/**
 * Returns an array of all edges in the graph
 */
Edge** graph_get_edges(const Graph gr) {
    if (!gr) return NULL;
    
    int num_edges = graph_num_edges(gr);
    if (num_edges == 0) return NULL;
    
    Edge** edges = (Edge**)calloc(num_edges + 1, sizeof(Edge*));
    if (!edges) {
        fprintf(stderr, "Error: Failed to allocate memory for edges array\n");
        return NULL;
    }
    
    void** nodes = hash_table_keyset(gr->nodes);
    if (!nodes) {
        free(edges);
        return NULL;
    }
    
    int edge_index = 0;
    // Iterate through all nodes and their adjacency lists
    for (int i = 0; i < gr->num_nodes; i++) {
        void* source = nodes[i];
        AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, source);
        if (!adj_list) continue;
        
        AdjNode* current = adj_list->head;
        while (current) {
            // For undirected graphs, only add edge once (from smaller to larger node)
            if (!gr->directed && gr->compare(source, current->dest) > 0) {
                current = current->next;
                continue;
            }
            
            Edge* edge = (Edge*)malloc(sizeof(Edge));
            if (edge) {
                edge->source = source;
                edge->dest = current->dest;
                edge->label = current->label;
                edges[edge_index++] = edge;
            } else {
                fprintf(stderr, "Error: Failed to allocate memory for edge\n");
            }
            current = current->next;
        }
    }
    
    free(nodes);
    edges[edge_index] = NULL; // Null-terminate the array
    return edges;
}

/**
 * Returns an array of all neighbors of a given node
 */
void** graph_get_neighbours(const Graph gr, const void* node) {
    if (!gr || !node) return NULL;
    
    AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, node);
    if (!adj_list || adj_list->size == 0) return NULL;
    
    void** neighbors = (void**)malloc(adj_list->size * sizeof(void*));
    if (!neighbors) {
        fprintf(stderr, "Error: Failed to allocate memory for neighbors array\n");
        return NULL;
    }
    
    // Copy all neighbors to array
    AdjNode* current = adj_list->head;
    int index = 0;
    while (current) {
        neighbors[index++] = current->dest;
        current = current->next;
    }
    
    return neighbors;
}

/**
 * Returns the number of neighbors of a given node
 */
int graph_num_neighbours(const Graph gr, const void* node) {
    if (!gr || !node) return 0;
    
    AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, node);
    return adj_list ? adj_list->size : 0;
}

/**
 * Returns the label of an edge between two nodes
 */
void* graph_get_label(const Graph gr, const void* node1, const void* node2) {
    if (!gr || !node1 || !node2) return NULL;
    
    AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, node1);
    if (!adj_list) return NULL;
    
    return adj_list_get_label(adj_list, (void*)node2, gr->compare);
}

/**
 * Frees all memory allocated for the graph
 */
void graph_free(Graph gr) {
    if (!gr) return;
    
    // Free all adjacency lists
    void** nodes = hash_table_keyset(gr->nodes);
    if (nodes) {
        for (int i = 0; i < gr->num_nodes; i++) {
            AdjList* adj_list = (AdjList*)hash_table_get(gr->nodes, nodes[i]);
            adj_list_free(adj_list);
        }
        free(nodes);
    }
    
    hash_table_free(gr->nodes);
    free(gr);
}

// Queue structure for BFS implementation
typedef struct QueueNode {
    void* data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;
} Queue;

/**
 * Creates a new empty queue
 */
static Queue* queue_create() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        fprintf(stderr, "Error: Failed to allocate memory for queue\n");
        return NULL;
    }
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

/**
 * Adds an element to the rear of the queue
 */
static void queue_enqueue(Queue* queue, void* data) {
    if (!queue) return;
    
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        fprintf(stderr, "Error: Failed to allocate memory for queue node\n");
        return;
    }
    
    node->data = data;
    node->next = NULL;
    
    // Add to rear of queue
    if (queue->rear) {
        queue->rear->next = node;
    } else {
        queue->front = node;
    }
    queue->rear = node;
    queue->size++;
}

/**
 * Removes and returns an element from the front of the queue
 */
static void* queue_dequeue(Queue* queue) {
    if (!queue || !queue->front) return NULL;
    
    QueueNode* node = queue->front;
    void* data = node->data;
    
    queue->front = node->next;
    if (!queue->front) {
        queue->rear = NULL;
    }
    
    free(node);
    queue->size--;
    return data;
}

/**
 * Checks if the queue is empty
 */
static int queue_is_empty(const Queue* queue) {
    return !queue || queue->size == 0;
}

/**
 * Frees all memory allocated for the queue
 */
static void queue_free(Queue* queue) {
    if (!queue) return;
    
    // Dequeue all remaining elements
    while (!queue_is_empty(queue)) {
        queue_dequeue(queue);
    }
    free(queue);
}

/**
 * Performs breadth-first search starting from a given node
 */
void** breadth_first_visit(Graph gr, void* start, 
                          int (*compare)(const void*, const void*), 
                          unsigned long (*hash)(const void*)) {
    if (!gr) {
        fprintf(stderr, "Error: Graph cannot be NULL for BFS\n");
        return NULL;
    }
    if (!start) {
        fprintf(stderr, "Error: Start node cannot be NULL for BFS\n");
        return NULL;
    }
    if (!compare || !hash) {
        fprintf(stderr, "Error: Compare and hash functions cannot be NULL for BFS\n");
        return NULL;
    }
    
    if (!graph_contains_node(gr, start)) {
        return NULL;
    }
    
    // Create visited set using hash table
    HashTable* visited = hash_table_create(compare, hash);
    if (!visited) {
        fprintf(stderr, "Error: Failed to create visited set for BFS\n");
        return NULL;
    }
    
    // Create result array
    void** result = (void**)calloc(graph_num_nodes(gr) + 1, sizeof(void*));
    if (!result) {
        fprintf(stderr, "Error: Failed to allocate memory for BFS result\n");
        hash_table_free(visited);
        return NULL;
    }
    
    // Create queue for BFS
    Queue* queue = queue_create();
    if (!queue) {
        free(result);
        hash_table_free(visited);
        return NULL;
    }
    
    int result_index = 0;
    int visited_marker = 1;
    
    // Initialize BFS with start node
    queue_enqueue(queue, start);
    hash_table_put(visited, start, &visited_marker);
    
    while (!queue_is_empty(queue)) {
        void* current = queue_dequeue(queue);
        result[result_index++] = current;
        
        // Get neighbors and add unvisited ones to queue
        void** neighbors = graph_get_neighbours(gr, current);
        int num_neighbors = graph_num_neighbours(gr, current);
        
        if (neighbors) {
            for (int i = 0; i < num_neighbors; i++) {
                if (!hash_table_contains_key(visited, neighbors[i])) {
                    queue_enqueue(queue, neighbors[i]);
                    hash_table_put(visited, neighbors[i], &visited_marker);
                }
            }
            free(neighbors);
        }
    }

    // Clean up resources
    queue_free(queue);
    hash_table_free(visited);
   
    result[result_index] = NULL; // Null-terminate the result array
    return result;
}
