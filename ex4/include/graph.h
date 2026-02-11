#ifndef GRAPH_H
#define GRAPH_H

/**
 * Generic sparse graph implementation using adjacency lists
 * Optimized for sparse graphs using hash tables from exercise 3
 */

typedef struct graph *Graph;

typedef struct edge {
    void* source;      // nodo d'origine
    void* dest;        // nodo di destinazione
    void* label;       // etichetta dell'arco
} Edge;

/**
 * Creates a new empty graph
 * 
 * @param labelled 1 if the graph is labelled, 0 otherwise
 * @param directed 1 if the graph is directed, 0 otherwise
 * @param compare Function to compare nodes (returns 0 if equal, non-zero otherwise)
 * @param hash Function to compute hash value for nodes
 * @return Pointer to the newly created graph, or NULL on failure
 * Complexity: O(1)
 */
Graph graph_create(int labelled, int directed,
                   int (*compare)(const void*, const void*),
                   unsigned long (*hash)(const void*));

/**
 * Checks if the graph is directed
 * 
 * @param gr The graph
 * @return 1 if directed, 0 if undirected
 * Complexity: O(1)
 */
int graph_is_directed(const Graph gr);

/**
 * Checks if the graph is labelled
 * 
 * @param gr The graph
 * @return 1 if labelled, 0 if not labelled
 * Complexity: O(1)
 */
int graph_is_labelled(const Graph gr);

/**
 * Adds a node to the graph
 * 
 * @param gr The graph
 * @param node Pointer to the node to add
 * @return 1 if successful, 0 if node already exists or on error
 * Complexity: O(1)
 */
int graph_add_node(Graph gr, const void* node);

/**
 * Adds an edge to the graph
 * 
 * @param gr The graph
 * @param node1 Source node
 * @param node2 Destination node
 * @param label Edge label (can be NULL for unlabelled graphs)
 * @return 1 if successful, 0 on error
 * Complexity: O(1) for sparse graphs
 */
int graph_add_edge(Graph gr, const void* node1, const void* node2, const void* label);

/**
 * Checks if a node exists in the graph
 * 
 * @param gr The graph
 * @param node The node to search for
 * @return 1 if node exists, 0 otherwise
 * Complexity: O(1)
 */
int graph_contains_node(const Graph gr, const void* node);

/**
 * Checks if an edge exists in the graph
 * 
 * @param gr The graph
 * @param node1 Source node
 * @param node2 Destination node
 * @return 1 if edge exists, 0 otherwise
 * Complexity: O(1) for sparse graphs
 */
int graph_contains_edge(const Graph gr, const void* node1, const void* node2);

/**
 * Removes a node from the graph
 * 
 * @param gr The graph
 * @param node The node to remove
 * @return 1 if successful, 0 if node doesn't exist
 * Complexity: O(N) where N is the number of edges
 */
int graph_remove_node(Graph gr, const void* node);

/**
 * Removes an edge from the graph
 * 
 * @param gr The graph
 * @param node1 Source node
 * @param node2 Destination node
 * @return 1 if successful, 0 if edge doesn't exist
 * Complexity: O(1) for sparse graphs
 */
int graph_remove_edge(Graph gr, const void* node1, const void* node2);

/**
 * Returns the number of nodes in the graph
 * 
 * @param gr The graph
 * @return Number of nodes
 * Complexity: O(1)
 */
int graph_num_nodes(const Graph gr);

/**
 * Returns the number of edges in the graph
 * 
 * @param gr The graph
 * @return Number of edges
 * Complexity: O(N) where N is the number of nodes
 */
int graph_num_edges(const Graph gr);

/**
 * Returns an array of all nodes in the graph
 * The caller is responsible for freeing the returned array
 * 
 * @param gr The graph
 * @return Array of node pointers, NULL if graph is empty
 * Complexity: O(N) where N is the number of nodes
 */
void** graph_get_nodes(const Graph gr);

/**
 * Returns an array of all edges in the graph
 * The caller is responsible for freeing the returned array and the Edge structures
 * 
 * @param gr The graph
 * @return Array of Edge pointers, NULL if graph has no edges
 * Complexity: O(N) where N is the number of edges
 */
Edge** graph_get_edges(const Graph gr);

/**
 * Returns an array of all nodes adjacent to a given node
 * The caller is responsible for freeing the returned array
 * 
 * @param gr The graph
 * @param node The node whose neighbors to retrieve
 * @return Array of neighbor node pointers, NULL if node has no neighbors or doesn't exist
 * Complexity: O(1) for sparse graphs
 */
void** graph_get_neighbours(const Graph gr, const void* node);

/**
 * Returns the number of nodes adjacent to a given node
 * 
 * @param gr The graph
 * @param node The node whose neighbor count to retrieve
 * @return Number of neighbors, 0 if node doesn't exist
 * Complexity: O(1)
 */
int graph_num_neighbours(const Graph gr, const void* node);

/**
 * Returns the label of an edge
 * 
 * @param gr The graph
 * @param node1 Source node
 * @param node2 Destination node
 * @return Edge label if edge exists, NULL otherwise
 * Complexity: O(1) for sparse graphs
 */
void* graph_get_label(const Graph gr, const void* node1, const void* node2);

/**
 * Frees the memory allocated for the graph
 * Note: This does not free the actual nodes and labels, only the graph structure
 * 
 * @param gr The graph to free
 */
void graph_free(Graph gr);

/**
 * Performs breadth-first search starting from a given node
 * 
 * @param gr The graph
 * @param start The starting node
 * @param compare Function to compare nodes
 * @param hash Function to compute hash value for nodes
 * @return Array of nodes in visit order, NULL if start node doesn't exist
 */
void** breadth_first_visit(Graph gr, void* start, 
                          int (*compare)(const void*, const void*), 
                          unsigned long (*hash)(const void*));

#endif
