/**
 * @file utils_demo.c
 * @brief Demonstration of utility functions for SSSP project
 *
 * This example shows how to use:
 * - Random graph generation
 * - Graph file I/O
 * - Performance timing
 * - Graph validation
 * - Memory tracking
 */

#include "sssp_breakthrough.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void demo_graph_generation(void) {
    printf("=== Graph Generation Demo ===\n\n");

    // Generate a random graph
    printf("1. Generating random graph (n=10, m=20)...\n");
    GraphGenParams params = graph_gen_params_default(10, 20);
    params.min_weight = 1.0;
    params.max_weight = 10.0;
    params.seed = 42; // For reproducibility

    Graph* graph = graph_generate_random(&params);
    if (graph) {
        printf("   Generated graph with %d vertices and %d edges\n", graph->n, graph->m);
        free_graph(graph);
    }

    // Generate a grid graph
    printf("\n2. Generating 3x3 grid graph...\n");
    graph = graph_generate_grid(3, 3, 1.0, 5.0, false);
    if (graph) {
        printf("   Generated grid with %d vertices and %d edges\n", graph->n, graph->m);
        free_graph(graph);
    }

    // Generate a DAG
    printf("\n3. Generating random DAG (n=8, m=15)...\n");
    graph = graph_generate_dag(8, 15, 1.0, 10.0);
    if (graph) {
        printf("   Generated DAG with %d vertices and %d edges\n", graph->n, graph->m);
        free_graph(graph);
    }

    // Generate a tree
    printf("\n4. Generating random tree (n=10)...\n");
    graph = graph_generate_tree(10, 1.0, 5.0);
    if (graph) {
        printf("   Generated tree with %d vertices and %d edges\n", graph->n, graph->m);
        free_graph(graph);
    }

    printf("\n");
}

void demo_file_io(void) {
    printf("=== Graph File I/O Demo ===\n\n");

    // Create a simple graph
    printf("1. Creating a simple graph...\n");
    Graph* graph = create_graph(5);
    add_edge(graph, 0, 1, 2.0);
    add_edge(graph, 0, 2, 4.0);
    add_edge(graph, 1, 2, 1.0);
    add_edge(graph, 1, 3, 7.0);
    add_edge(graph, 2, 3, 3.0);
    add_edge(graph, 3, 4, 1.0);

    printf("   Graph: %d vertices, %d edges\n", graph->n, graph->m);

    // Write to edge list format
    printf("\n2. Writing graph to 'test_graph.txt' (edge list format)...\n");
    if (graph_write_edge_list(graph, "test_graph.txt")) {
        printf("   Graph saved successfully\n");
    }

    // Write to DIMACS format
    printf("\n3. Writing graph to 'test_graph.dimacs' (DIMACS format)...\n");
    if (graph_write_dimacs(graph, "test_graph.dimacs", 0)) {
        printf("   Graph saved successfully\n");
    }

    // Write to DOT format for visualization
    printf("\n4. Writing graph to 'test_graph.dot' (DOT format)...\n");
    if (graph_write_dot(graph, "test_graph.dot", NULL)) {
        printf("   Graph saved successfully\n");
        printf("   Visualize with: dot -Tpng test_graph.dot -o test_graph.png\n");
    }

    // Read back from edge list
    printf("\n5. Reading graph from 'test_graph.txt'...\n");
    Graph* loaded_graph = graph_read_edge_list("test_graph.txt");
    if (loaded_graph) {
        printf("   Loaded graph: %d vertices, %d edges\n",
               loaded_graph->n, loaded_graph->m);
        free_graph(loaded_graph);
    }

    free_graph(graph);
    printf("\n");
}

void demo_timing(void) {
    printf("=== Performance Timing Demo ===\n\n");

    // Create a timer
    printf("1. Creating timer...\n");
    Timer* timer = timer_create();

    // Simulate some work
    printf("\n2. Timing graph generation...\n");
    timer_start(timer);

    Graph* graph = graph_generate_random(&graph_gen_params_default(1000, 5000));

    double elapsed = timer_stop(timer);

    char time_str[64];
    format_time(elapsed, time_str, sizeof(time_str));
    printf("   Generated 1000-vertex graph in %s\n", time_str);

    // Time SSSP algorithm
    printf("\n3. Timing SSSP computation...\n");
    timer_start(timer);

    sssp_breakthrough(graph, 0);

    elapsed = timer_stop(timer);
    format_time(elapsed, time_str, sizeof(time_str));
    printf("   SSSP completed in %s\n", time_str);

    free_graph(graph);
    timer_destroy(timer);
    printf("\n");
}

void demo_validation(void) {
    printf("=== Graph Validation Demo ===\n\n");

    // Create and validate a simple graph
    printf("1. Creating and validating a simple graph...\n");
    Graph* graph = graph_generate_random(&graph_gen_params_default(20, 40));

    if (graph_validate(graph)) {
        printf("   Graph structure is valid\n");
    }

    // Get and print graph properties
    printf("\n2. Analyzing graph properties...\n");
    GraphProperties props = graph_get_properties(graph);
    graph_print_properties(&props);

    // Check specific properties
    printf("\n3. Checking specific properties...\n");
    printf("   Is connected: %s\n", graph_is_connected(graph) ? "yes" : "no");
    printf("   Is DAG: %s\n", graph_is_dag(graph) ? "yes" : "no");
    printf("   Has negative cycle: %s\n",
           graph_has_negative_cycle(graph, 0) ? "yes" : "no");

    free_graph(graph);

    // Test with a DAG
    printf("\n4. Testing with a known DAG...\n");
    graph = graph_generate_dag(15, 30, 1.0, 10.0);
    props = graph_get_properties(graph);
    printf("   Is DAG: %s (expected: yes)\n", props.is_dag ? "yes" : "no");
    free_graph(graph);

    printf("\n");
}

void demo_memory_tracking(void) {
    printf("=== Memory Tracking Demo ===\n\n");

    memory_reset_stats();

    printf("1. Initial memory state:\n");
    memory_print_stats();

    printf("\n2. Allocating graph...\n");
    Graph* graph = graph_generate_random(&graph_gen_params_default(500, 2000));
    memory_print_stats();

    printf("\n3. Allocating another graph...\n");
    Graph* graph2 = graph_generate_complete(50, 1.0, 10.0);
    memory_print_stats();

    printf("\n4. Freeing first graph...\n");
    free_graph(graph);
    memory_print_stats();

    printf("\n5. Freeing second graph...\n");
    free_graph(graph2);
    memory_print_stats();

    printf("\n");
}

void demo_random_utilities(void) {
    printf("=== Random Utilities Demo ===\n\n");

    // Set random seed for reproducibility
    printf("1. Setting random seed to 12345...\n");
    utils_set_random_seed(12345);

    // Generate random integers
    printf("\n2. Random integers in range [1, 100]:\n   ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", random_int(1, 100));
    }
    printf("\n");

    // Generate random doubles
    printf("\n3. Random doubles in range [0.0, 1.0]:\n   ");
    for (int i = 0; i < 10; i++) {
        printf("%.3f ", random_double(0.0, 1.0));
    }
    printf("\n");

    printf("\n");
}

int main(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         SSSP Utilities Demonstration Program              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");

    // Run all demos
    demo_graph_generation();
    demo_file_io();
    demo_timing();
    demo_validation();
    demo_memory_tracking();
    demo_random_utilities();

    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                 All Demos Completed                        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
