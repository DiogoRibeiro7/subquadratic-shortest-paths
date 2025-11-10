/**
 * @file simple_example.c
 * @brief Simple example demonstrating the SSSP public API
 *
 * This example shows how to:
 * - Create a graph
 * - Add edges
 * - Solve SSSP
 * - Query results
 * - Handle errors
 */

#include <sssp.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== SSSP Breakthrough Algorithm - Simple Example ===\n\n");

    // Print library information
    printf("Library Version: %s\n", sssp_get_version());
    printf("Algorithm: %s\n\n", sssp_get_info());

    // Create a graph with 5 vertices (0, 1, 2, 3, 4)
    printf("Creating graph with 5 vertices...\n");
    sssp_graph_t* graph = sssp_graph_create(5);
    if (!graph) {
        fprintf(stderr, "Error: Failed to create graph\n");
        return EXIT_FAILURE;
    }

    // Add directed edges
    printf("Adding edges to the graph...\n");
    printf("  Edge 0 -> 1 (weight: 4.0)\n");
    sssp_graph_add_edge(graph, 0, 1, 4.0);

    printf("  Edge 0 -> 2 (weight: 1.0)\n");
    sssp_graph_add_edge(graph, 0, 2, 1.0);

    printf("  Edge 2 -> 1 (weight: 2.0)\n");
    sssp_graph_add_edge(graph, 2, 1, 2.0);

    printf("  Edge 1 -> 3 (weight: 1.0)\n");
    sssp_graph_add_edge(graph, 1, 3, 1.0);

    printf("  Edge 2 -> 3 (weight: 5.0)\n");
    sssp_graph_add_edge(graph, 2, 3, 5.0);

    printf("  Edge 3 -> 4 (weight: 3.0)\n");
    sssp_graph_add_edge(graph, 3, 4, 3.0);

    // Print graph statistics
    printf("\nGraph created:\n");
    printf("  Vertices: %d\n", sssp_graph_get_num_vertices(graph));
    printf("  Edges: %d\n\n", sssp_graph_get_num_edges(graph));

    // Solve SSSP from source vertex 0
    printf("Solving SSSP from source vertex 0...\n");
    sssp_result_t* result = sssp_solve(graph, 0);
    if (!result) {
        fprintf(stderr, "Error: Failed to solve SSSP\n");
        sssp_graph_destroy(graph);
        return EXIT_FAILURE;
    }

    // Check computation status
    sssp_status_t status = sssp_result_get_status(result);
    if (status != SSSP_SUCCESS) {
        fprintf(stderr, "Error: SSSP computation failed: %s\n",
                sssp_status_to_string(status));
        sssp_result_destroy(result);
        sssp_graph_destroy(graph);
        return EXIT_FAILURE;
    }

    printf("SSSP solved successfully!\n\n");

    // Print shortest distances from source
    printf("Shortest distances from vertex 0:\n");
    printf("%-10s %-15s %-15s %s\n", "Vertex", "Distance", "Predecessor", "Reachable");
    printf("------------------------------------------------------\n");

    for (int v = 0; v < 5; v++) {
        double dist = sssp_result_get_distance(result, v);
        int pred = sssp_result_get_predecessor(result, v);
        bool reachable = sssp_result_is_reachable(result, v);

        printf("%-10d ", v);

        if (dist == SSSP_INFINITY) {
            printf("%-15s ", "infinity");
        } else {
            printf("%-15.2f ", dist);
        }

        if (pred == SSSP_INVALID_VERTEX) {
            printf("%-15s ", "none");
        } else {
            printf("%-15d ", pred);
        }

        printf("%s\n", reachable ? "yes" : "no");
    }

    // Reconstruct and print shortest path to vertex 4
    printf("\nShortest path from vertex 0 to vertex 4:\n");
    int path[SSSP_MAX_VERTICES];
    int path_length;

    status = sssp_result_get_path(result, 4, path, &path_length);
    if (status == SSSP_SUCCESS && path_length > 0) {
        printf("  Path: ");
        for (int i = 0; i < path_length; i++) {
            printf("%d", path[i]);
            if (i < path_length - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
        printf("  Total distance: %.2f\n", sssp_result_get_distance(result, 4));
    } else {
        printf("  Vertex 4 is unreachable from vertex 0\n");
    }

    // Verify solution correctness
    printf("\nVerifying solution...\n");
    if (sssp_result_verify(result)) {
        printf("✓ Solution verified successfully!\n");
    } else {
        printf("✗ Solution verification failed!\n");
    }

    // Clean up resources
    printf("\nCleaning up...\n");
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    printf("\n=== Example completed successfully ===\n");

    return EXIT_SUCCESS;
}
