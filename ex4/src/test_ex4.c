#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "graph.h"

// Test data and utility functions
static int string_compare(const void* a, const void* b) {
    if (!a || !b) return (a == b) ? 0 : 1;
    return strcmp((const char*)a, (const char*)b);
}

static unsigned long string_hash(const void* str) {
    if (!str) return 0;
    
    const char* s = (const char*)str;
    unsigned long hash = 5381;
    int c;
    
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

static int int_compare(const void* a, const void* b) {
    if (!a || !b) return (a == b) ? 0 : 1;
    int val_a = *(const int*)a;
    int val_b = *(const int*)b;
    return (val_a == val_b) ? 0 : (val_a < val_b ? -1 : 1);
}

static unsigned long int_hash(const void* ptr) {
    if (!ptr) return 0;
    return (unsigned long)(*(const int*)ptr);
}

// Test nodes
static char* city1 = "Rome";
static char* city2 = "Milan";
static char* city3 = "Naples";
static char* city4 = "Florence";
static char* label1 = "100.5";
static char* label2 = "200.0";
static char* label3 = "150.75";

static int node1 = 1;
static int node2 = 2;
static int node3 = 3;
static int node4 = 4;

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

// Test graph creation
void test_graph_create_directed_labelled(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    TEST_ASSERT_NOT_NULL(gr);
    TEST_ASSERT_TRUE(graph_is_directed(gr));
    TEST_ASSERT_TRUE(graph_is_labelled(gr));
    TEST_ASSERT_EQUAL_INT(0, graph_num_nodes(gr));
    TEST_ASSERT_EQUAL_INT(0, graph_num_edges(gr));
    
    graph_free(gr);
}

void test_graph_create_undirected_unlabelled(void) {
    Graph gr = graph_create(0, 0, string_compare, string_hash);
    
    TEST_ASSERT_NOT_NULL(gr);
    TEST_ASSERT_FALSE(graph_is_directed(gr));
    TEST_ASSERT_FALSE(graph_is_labelled(gr));
    TEST_ASSERT_EQUAL_INT(0, graph_num_nodes(gr));
    TEST_ASSERT_EQUAL_INT(0, graph_num_edges(gr));
    
    graph_free(gr);
}

void test_graph_create_with_null_parameters(void) {
    Graph gr1 = graph_create(1, 1, NULL, string_hash);
    Graph gr2 = graph_create(1, 1, string_compare, NULL);
    
    TEST_ASSERT_NULL(gr1);
    TEST_ASSERT_NULL(gr2);
}

// Test node operations
void test_graph_add_node(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    TEST_ASSERT_TRUE(graph_add_node(gr, city1));
    TEST_ASSERT_EQUAL_INT(1, graph_num_nodes(gr));
    TEST_ASSERT_TRUE(graph_contains_node(gr, city1));
    
    // Adding same node again should fail
    TEST_ASSERT_FALSE(graph_add_node(gr, city1));
    TEST_ASSERT_EQUAL_INT(1, graph_num_nodes(gr));
    
    graph_free(gr);
}

void test_graph_add_multiple_nodes(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    TEST_ASSERT_TRUE(graph_add_node(gr, city1));
    TEST_ASSERT_TRUE(graph_add_node(gr, city2));
    TEST_ASSERT_TRUE(graph_add_node(gr, city3));
    
    TEST_ASSERT_EQUAL_INT(3, graph_num_nodes(gr));
    TEST_ASSERT_TRUE(graph_contains_node(gr, city1));
    TEST_ASSERT_TRUE(graph_contains_node(gr, city2));
    TEST_ASSERT_TRUE(graph_contains_node(gr, city3));
    TEST_ASSERT_FALSE(graph_contains_node(gr, city4));
    
    graph_free(gr);
}

void test_graph_remove_node(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_node(gr, city1);
    graph_add_node(gr, city2);
    
    TEST_ASSERT_TRUE(graph_remove_node(gr, city1));
    TEST_ASSERT_EQUAL_INT(1, graph_num_nodes(gr));
    TEST_ASSERT_FALSE(graph_contains_node(gr, city1));
    TEST_ASSERT_TRUE(graph_contains_node(gr, city2));
    
    // Removing non-existent node should fail
    TEST_ASSERT_FALSE(graph_remove_node(gr, city1));
    
    graph_free(gr);
}

// Test edge operations for directed graphs
void test_graph_add_edge_directed_labelled(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    // Adding edge should automatically add nodes
    TEST_ASSERT_TRUE(graph_add_edge(gr, city1, city2, label1));
    TEST_ASSERT_EQUAL_INT(2, graph_num_nodes(gr));
    TEST_ASSERT_EQUAL_INT(1, graph_num_edges(gr));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city1, city2));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city2, city1)); // Directed
    
    graph_free(gr);
}

void test_graph_add_edge_undirected_labelled(void) {
    Graph gr = graph_create(1, 0, string_compare, string_hash);
    
    TEST_ASSERT_TRUE(graph_add_edge(gr, city1, city2, label1));
    TEST_ASSERT_EQUAL_INT(2, graph_num_nodes(gr));
    TEST_ASSERT_EQUAL_INT(1, graph_num_edges(gr));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city1, city2));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city2, city1)); // Undirected
    
    graph_free(gr);
}

void test_graph_add_edge_unlabelled(void) {
    Graph gr = graph_create(0, 1, string_compare, string_hash);
    
    TEST_ASSERT_TRUE(graph_add_edge(gr, city1, city2, NULL));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city1, city2));
    
    // Adding edge with label to unlabelled graph should fail
    TEST_ASSERT_FALSE(graph_add_edge(gr, city2, city3, label1));
    
    graph_free(gr);
}

void test_graph_add_edge_labelled_with_null_label(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    // Adding edge without label to labelled graph should fail
    TEST_ASSERT_FALSE(graph_add_edge(gr, city1, city2, NULL));
    TEST_ASSERT_EQUAL_INT(0, graph_num_edges(gr));
    
    graph_free(gr);
}

void test_graph_remove_edge(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    graph_add_edge(gr, city2, city3, label2);
    
    TEST_ASSERT_TRUE(graph_remove_edge(gr, city1, city2));
    TEST_ASSERT_EQUAL_INT(1, graph_num_edges(gr));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city1, city2));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city2, city3));
    
    // Removing non-existent edge should fail
    TEST_ASSERT_FALSE(graph_remove_edge(gr, city1, city2));
    
    graph_free(gr);
}

void test_graph_remove_edge_undirected(void) {
    Graph gr = graph_create(1, 0, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    
    TEST_ASSERT_TRUE(graph_remove_edge(gr, city1, city2));
    TEST_ASSERT_EQUAL_INT(0, graph_num_edges(gr));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city1, city2));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city2, city1));
    
    graph_free(gr);
}

// Test label operations
void test_graph_get_label(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    graph_add_edge(gr, city2, city3, label2);
    
    void* retrieved_label1 = graph_get_label(gr, city1, city2);
    void* retrieved_label2 = graph_get_label(gr, city2, city3);
    void* retrieved_label3 = graph_get_label(gr, city1, city3);
    
    TEST_ASSERT_EQUAL_PTR(label1, retrieved_label1);
    TEST_ASSERT_EQUAL_PTR(label2, retrieved_label2);
    TEST_ASSERT_NULL(retrieved_label3);
    
    graph_free(gr);
}

// Test neighbor operations
void test_graph_get_neighbours(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    graph_add_edge(gr, city1, city3, label2);
    
    void** neighbors = graph_get_neighbours(gr, city1);
    int num_neighbors = graph_num_neighbours(gr, city1);
    
    TEST_ASSERT_NOT_NULL(neighbors);
    TEST_ASSERT_EQUAL_INT(2, num_neighbors);
    
    // Check that both neighbors are present (order may vary)
    int found_city2 = 0, found_city3 = 0;
    for (int i = 0; i < num_neighbors; i++) {
        if (string_compare(neighbors[i], city2) == 0) found_city2 = 1;
        if (string_compare(neighbors[i], city3) == 0) found_city3 = 1;
    }
    TEST_ASSERT_TRUE(found_city2);
    TEST_ASSERT_TRUE(found_city3);
    
    free(neighbors);
    graph_free(gr);
}

void test_graph_get_neighbours_empty(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_node(gr, city1);
    
    void** neighbors = graph_get_neighbours(gr, city1);
    int num_neighbors = graph_num_neighbours(gr, city1);
    
    TEST_ASSERT_NULL(neighbors);
    TEST_ASSERT_EQUAL_INT(0, num_neighbors);
    
    graph_free(gr);
}

// Test graph data retrieval
void test_graph_get_nodes(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_node(gr, city1);
    graph_add_node(gr, city2);
    graph_add_node(gr, city3);
    
    void** nodes = graph_get_nodes(gr);
    int num_nodes = graph_num_nodes(gr);
    
    TEST_ASSERT_NOT_NULL(nodes);
    TEST_ASSERT_EQUAL_INT(3, num_nodes);
    
    // Check that all nodes are present
    int found_city1 = 0, found_city2 = 0, found_city3 = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (string_compare(nodes[i], city1) == 0) found_city1 = 1;
        if (string_compare(nodes[i], city2) == 0) found_city2 = 1;
        if (string_compare(nodes[i], city3) == 0) found_city3 = 1;
    }
    TEST_ASSERT_TRUE(found_city1);
    TEST_ASSERT_TRUE(found_city2);
    TEST_ASSERT_TRUE(found_city3);
    
    free(nodes);
    graph_free(gr);
}

void test_graph_get_edges(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    graph_add_edge(gr, city2, city3, label2);
    
    Edge** edges = graph_get_edges(gr);
    int num_edges = graph_num_edges(gr);
    
    TEST_ASSERT_NOT_NULL(edges);
    TEST_ASSERT_EQUAL_INT(2, num_edges);
    
    // Check edges (order may vary)
    int found_edge1 = 0, found_edge2 = 0;
    for (int i = 0; i < num_edges; i++) {
        if (string_compare(edges[i]->source, city1) == 0 && 
            string_compare(edges[i]->dest, city2) == 0) {
            found_edge1 = 1;
            TEST_ASSERT_EQUAL_PTR(label1, edges[i]->label);
        }
        if (string_compare(edges[i]->source, city2) == 0 && 
            string_compare(edges[i]->dest, city3) == 0) {
            found_edge2 = 1;
            TEST_ASSERT_EQUAL_PTR(label2, edges[i]->label);
        }
        free(edges[i]);
    }
    
    TEST_ASSERT_TRUE(found_edge1);
    TEST_ASSERT_TRUE(found_edge2);
    
    free(edges);
    graph_free(gr);
}

// Test breadth-first search
void test_breadth_first_visit_single_node(void) {
    Graph gr = graph_create(0, 0, int_compare, int_hash);
    
    graph_add_node(gr, &node1);
    
    void** result = breadth_first_visit(gr, &node1, int_compare, int_hash);
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(node1, *(int*)result[0]);
    
    free(result);
    graph_free(gr);
}

void test_breadth_first_visit_linear_graph(void) {
    Graph gr = graph_create(0, 0, int_compare, int_hash);
    
    // Create linear graph: 1-2-3-4
    graph_add_edge(gr, &node1, &node2, NULL);
    graph_add_edge(gr, &node2, &node3, NULL);
    graph_add_edge(gr, &node3, &node4, NULL);
    
    void** result = breadth_first_visit(gr, &node1, int_compare, int_hash);
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(node1, *(int*)result[0]);
    TEST_ASSERT_EQUAL_INT(node2, *(int*)result[1]);
    TEST_ASSERT_EQUAL_INT(node3, *(int*)result[2]);
    TEST_ASSERT_EQUAL_INT(node4, *(int*)result[3]);
    
    free(result);
    graph_free(gr);
}

void test_breadth_first_visit_star_graph(void) {
    Graph gr = graph_create(0, 0, int_compare, int_hash);
    
    // Create star graph: 2,3,4 connected to 1
    graph_add_edge(gr, &node1, &node2, NULL);
    graph_add_edge(gr, &node1, &node3, NULL);
    graph_add_edge(gr, &node1, &node4, NULL);
    
    void** result = breadth_first_visit(gr, &node1, int_compare, int_hash);
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(node1, *(int*)result[0]);
    
    // The next three nodes should be 2, 3, 4 in some order
    int found_nodes[3] = {0, 0, 0};
    for (int i = 1; i <= 3; i++) {
        int val = *(int*)result[i];
        if (val == node2) found_nodes[0] = 1;
        else if (val == node3) found_nodes[1] = 1;
        else if (val == node4) found_nodes[2] = 1;
    }
    
    TEST_ASSERT_TRUE(found_nodes[0]);
    TEST_ASSERT_TRUE(found_nodes[1]);
    TEST_ASSERT_TRUE(found_nodes[2]);
    
    free(result);
    graph_free(gr);
}

void test_breadth_first_visit_nonexistent_start(void) {
    Graph gr = graph_create(0, 0, int_compare, int_hash);
    
    graph_add_node(gr, &node1);
    
    void** result = breadth_first_visit(gr, &node2, int_compare, int_hash);
    
    TEST_ASSERT_NULL(result);
    
    graph_free(gr);
}

void test_breadth_first_visit_with_null_parameters(void) {
    Graph gr = graph_create(0, 0, int_compare, int_hash);
    graph_add_node(gr, &node1);
    
    void** result1 = breadth_first_visit(NULL, &node1, int_compare, int_hash);
    void** result2 = breadth_first_visit(gr, NULL, int_compare, int_hash);
    void** result3 = breadth_first_visit(gr, &node1, NULL, int_hash);
    void** result4 = breadth_first_visit(gr, &node1, int_compare, NULL);
    
    TEST_ASSERT_NULL(result1);
    TEST_ASSERT_NULL(result2);
    TEST_ASSERT_NULL(result3);
    TEST_ASSERT_NULL(result4);
    
    graph_free(gr);
}

// Test complex scenarios
void test_graph_remove_node_with_edges(void) {
    Graph gr = graph_create(1, 1, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, label1);
    graph_add_edge(gr, city2, city3, label2);
    graph_add_edge(gr, city3, city1, label3);
    
    TEST_ASSERT_EQUAL_INT(3, graph_num_edges(gr));
    
    // Remove node should remove all connected edges
    TEST_ASSERT_TRUE(graph_remove_node(gr, city2));
    TEST_ASSERT_EQUAL_INT(2, graph_num_nodes(gr));
    TEST_ASSERT_EQUAL_INT(1, graph_num_edges(gr));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city1, city2));
    TEST_ASSERT_FALSE(graph_contains_edge(gr, city2, city3));
    TEST_ASSERT_TRUE(graph_contains_edge(gr, city3, city1));
    
    graph_free(gr);
}

void test_graph_edge_count_undirected(void) {
    Graph gr = graph_create(0, 0, string_compare, string_hash);
    
    graph_add_edge(gr, city1, city2, NULL);
    graph_add_edge(gr, city2, city3, NULL);
    
    // Undirected graph should count each edge only once
    TEST_ASSERT_EQUAL_INT(2, graph_num_edges(gr));
    
    graph_free(gr);
}

// Test runner
int main(void) {
    UNITY_BEGIN();
    
    // Graph creation tests
    RUN_TEST(test_graph_create_directed_labelled);
    RUN_TEST(test_graph_create_undirected_unlabelled);
    RUN_TEST(test_graph_create_with_null_parameters);
    
    // Node operation tests
    RUN_TEST(test_graph_add_node);
    RUN_TEST(test_graph_add_multiple_nodes);
    RUN_TEST(test_graph_remove_node);
    
    // Edge operation tests
    RUN_TEST(test_graph_add_edge_directed_labelled);
    RUN_TEST(test_graph_add_edge_undirected_labelled);
    RUN_TEST(test_graph_add_edge_unlabelled);
    RUN_TEST(test_graph_add_edge_labelled_with_null_label);
    RUN_TEST(test_graph_remove_edge);
    RUN_TEST(test_graph_remove_edge_undirected);
    
    // Label operation tests
    RUN_TEST(test_graph_get_label);
    
    // Neighbor operation tests
    RUN_TEST(test_graph_get_neighbours);
    RUN_TEST(test_graph_get_neighbours_empty);
    
    // Data retrieval tests
    RUN_TEST(test_graph_get_nodes);
    RUN_TEST(test_graph_get_edges);
    
    // BFS tests
    RUN_TEST(test_breadth_first_visit_single_node);
    RUN_TEST(test_breadth_first_visit_linear_graph);
    RUN_TEST(test_breadth_first_visit_star_graph);
    RUN_TEST(test_breadth_first_visit_nonexistent_start);
    RUN_TEST(test_breadth_first_visit_with_null_parameters);
    
    // Complex scenario tests
    RUN_TEST(test_graph_remove_node_with_edges);
    RUN_TEST(test_graph_edge_count_undirected);
    
    return UNITY_END();
}
