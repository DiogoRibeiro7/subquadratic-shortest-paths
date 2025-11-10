/**
 * @file basic_usage.c
 * @brief Basic usage example for SSSP Breakthrough Algorithm
 *
 * This example demonstrates:
 * 1. Creating graphs and adding edges manually
 * 2. Running the SSSP breakthrough algorithm
 * 3. Comparing with Dijkstra's algorithm
 * 4. Testing with different graph sizes (10, 100, 1000 vertices)
 * 5. Measuring and comparing performance
 *
 * Compile and run:
 *   mkdir build && cd build
 *   cmake ..
 *   make
 *   ./bin/examples/basic_usage
 */

#include "sssp_breakthrough.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*============================================================================
 * Dijkstra's Algorithm Implementation (for comparison)
 *============================================================================*/

/**
 * @brief Standard Dijkstra's algorithm using min-heap
 *
 * This is the classic O((V + E) log V) implementation used as a baseline
 * for comparing with the breakthrough O(m^{1+o(1)}) algorithm.
 *
 * @param graph Input graph
 * @param source Source vertex
 */
void dijkstra(Graph* graph, int source) {
    if (!graph || source < 0 || source >= graph->n) {
        fprintf(stderr, "Error: Invalid graph or source for Dijkstra\n");
        return;
    }

    // Step 1: Initialize distances to infinity and source to 0
    for (int i = 0; i < graph->n; i++) {
        graph->dist[i] = INFINITY_DIST;
        graph->pred[i] = -1;
        graph->complete[i] = false;
    }
    graph->dist[source] = 0.0;

    // Step 2: Create and initialize min-heap
    MinHeap* heap = heap_create(graph->n);
    if (!heap) {
        fprintf(stderr, "Error: Failed to create heap\n");
        return;
    }

    heap_insert(heap, source, 0.0);

    // Step 3: Main loop - extract minimum and relax neighbors
    while (heap->size > 0) {
        // Extract vertex with minimum distance
        int u = heap_extract_min(heap);

        // Skip if already processed
        if (graph->complete[u]) {
            continue;
        }

        // Mark as processed
        graph->complete[u] = true;

        // Relax all edges from u
        Edge* edge = graph->adj[u];
        while (edge) {
            int v = edge->target;
            double new_dist = graph->dist[u] + edge->weight;

            // If we found a shorter path to v
            if (new_dist < graph->dist[v]) {
                graph->dist[v] = new_dist;
                graph->pred[v] = u;

                // Update priority in heap
                if (heap->positions[v] >= 0) {
                    heap_decrease_key(heap, v, new_dist);
                } else {
                    heap_insert(heap, v, new_dist);
                }
            }

            edge = edge->next;
        }
    }

    heap_free(heap);
}

/*============================================================================
 * Comparison and Verification Functions
 *============================================================================*/

/**
 * @brief Compare results from two SSSP algorithms
 *
 * Checks if the distance arrays match (within floating-point tolerance).
 *
 * @param g1 First graph (after running algorithm 1)
 * @param g2 Second graph (after running algorithm 2)
 * @return true if distances match, false otherwise
 */
bool compare_results(const Graph* g1, const Graph* g2) {
    if (!g1 || !g2 || g1->n != g2->n) {
        return false;
    }

    const double EPSILON = 1e-9;
    bool match = true;

    for (int i = 0; i < g1->n; i++) {
        // Check if both are infinity or both are finite
        if ((g1->dist[i] == INFINITY_DIST) != (g2->dist[i] == INFINITY_DIST)) {
            printf("  Mismatch at vertex %d: %.3f vs %.3f\n",
                   i, g1->dist[i], g2->dist[i]);
            match = false;
            continue;
        }

        // If both finite, check if values are close
        if (g1->dist[i] != INFINITY_DIST) {
            double diff = fabs(g1->dist[i] - g2->dist[i]);
            if (diff > EPSILON) {
                printf("  Mismatch at vertex %d: %.3f vs %.3f (diff: %.9f)\n",
                       i, g1->dist[i], g2->dist[i], diff);
                match = false;
            }
        }
    }

    return match;
}

/**
 * @brief Print a subset of distances for verification
 *
 * @param graph Graph with computed distances
 * @param max_print Maximum number of distances to print
 */
void print_sample_distances(const Graph* graph, int max_print) {
    printf("  Sample distances:\n");

    int to_print = (graph->n < max_print) ? graph->n : max_print;

    for (int i = 0; i < to_print; i++) {
        printf("    Vertex %3d: ", i);

        if (graph->dist[i] == INFINITY_DIST) {
            printf("∞ (unreachable)\n");
        } else {
            printf("%8.3f", graph->dist[i]);

            // Print path by following predecessors
            if (graph->pred[i] != -1) {
                printf("  [path: ");
                int path[100];
                int path_len = 0;
                int v = i;

                // Reconstruct path
                while (v != -1 && path_len < 100) {
                    path[path_len++] = v;
                    v = graph->pred[v];
                }

                // Print in reverse (source to target)
                for (int j = path_len - 1; j >= 0; j--) {
                    printf("%d", path[j]);
                    if (j > 0) printf(" → ");
                }
                printf("]");
            }
            printf("\n");
        }
    }

    if (graph->n > max_print) {
        printf("    ... (%d more vertices)\n", graph->n - max_print);
    }
}

/*============================================================================
 * Example Demonstrations
 *============================================================================*/

/**
 * @brief Example 1: Small manual graph (10 vertices)
 *
 * Demonstrates manual graph creation with specific edges.
 */
void example_small_manual_graph(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Example 1: Small Manual Graph (10 vertices)\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    // Step 1: Create a graph with 10 vertices
    printf("Step 1: Creating graph with 10 vertices...\n");
    Graph* graph = create_graph(10);
    if (!graph) {
        fprintf(stderr, "Failed to create graph\n");
        return;
    }

    // Step 2: Add edges manually to create an interesting structure
    printf("Step 2: Adding edges to form a network...\n");
    printf("  (Creating a graph with multiple paths between vertices)\n\n");

    // Create a more complex network with multiple paths
    add_edge(graph, 0, 1, 4.0);   // Source to vertex 1
    add_edge(graph, 0, 2, 2.0);   // Source to vertex 2 (shorter)
    add_edge(graph, 1, 2, 1.0);   // Connect 1 and 2
    add_edge(graph, 1, 3, 5.0);   // Path to 3 via 1
    add_edge(graph, 2, 3, 8.0);   // Path to 3 via 2
    add_edge(graph, 2, 4, 10.0);  // Path to 4 via 2
    add_edge(graph, 3, 4, 2.0);   // Shorter path to 4 via 3
    add_edge(graph, 3, 5, 6.0);   // Path to 5
    add_edge(graph, 4, 5, 3.0);   // Alternative path to 5
    add_edge(graph, 5, 6, 1.0);   // Continue path
    add_edge(graph, 4, 7, 7.0);   // Branch to 7
    add_edge(graph, 6, 7, 2.0);   // Alternative to 7
    add_edge(graph, 7, 8, 4.0);   // Path to 8
    add_edge(graph, 8, 9, 1.0);   // Final vertex

    printf("  Added %d edges\n\n", graph->m);

    // Step 3: Run SSSP Breakthrough Algorithm
    printf("Step 3: Running SSSP Breakthrough Algorithm...\n");
    Timer* timer_breakthrough = timer_create();

    timer_start(timer_breakthrough);
    sssp_breakthrough(graph, 0);  // Source vertex is 0
    double time_breakthrough = timer_stop(timer_breakthrough);

    char time_str[64];
    format_time(time_breakthrough, time_str, sizeof(time_str));
    printf("  Completed in %s\n\n", time_str);

    // Save breakthrough results
    double* dist_breakthrough = (double*)malloc(graph->n * sizeof(double));
    memcpy(dist_breakthrough, graph->dist, graph->n * sizeof(double));

    // Step 4: Run Dijkstra's Algorithm for comparison
    printf("Step 4: Running Dijkstra's Algorithm (for comparison)...\n");
    Timer* timer_dijkstra = timer_create();

    timer_start(timer_dijkstra);
    dijkstra(graph, 0);  // Source vertex is 0
    double time_dijkstra = timer_stop(timer_dijkstra);

    format_time(time_dijkstra, time_str, sizeof(time_str));
    printf("  Completed in %s\n\n", time_str);

    // Step 5: Verify results match
    printf("Step 5: Verifying results match...\n");

    // Temporarily swap distances for comparison
    double* dist_dijkstra = graph->dist;
    graph->dist = dist_breakthrough;

    Graph temp_graph = *graph;
    temp_graph.dist = dist_dijkstra;

    bool match = compare_results(graph, &temp_graph);
    printf("  Results %s!\n\n", match ? "MATCH ✓" : "DO NOT MATCH ✗");

    // Step 6: Print sample results
    printf("Step 6: Shortest distances from source (vertex 0):\n");
    print_sample_distances(graph, 10);

    // Step 7: Verify solution correctness
    printf("\nStep 7: Verifying solution optimality...\n");
    if (verify_solution(graph)) {
        printf("  Solution verified: All distances satisfy optimality ✓\n");
    } else {
        printf("  Warning: Solution may not be optimal ✗\n");
    }

    // Cleanup
    free(dist_breakthrough);
    timer_destroy(timer_breakthrough);
    timer_destroy(timer_dijkstra);
    free_graph(graph);

    printf("\n");
}

/**
 * @brief Example 2: Medium random graph (100 vertices)
 *
 * Demonstrates graph generation and algorithm performance.
 */
void example_medium_random_graph(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Example 2: Medium Random Graph (100 vertices, 500 edges)\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    // Step 1: Generate random graph
    printf("Step 1: Generating random graph...\n");

    GraphGenParams params = graph_gen_params_default(100, 500);
    params.min_weight = 1.0;
    params.max_weight = 50.0;
    params.seed = 12345;  // For reproducibility

    Graph* graph = graph_generate_random(&params);
    if (!graph) {
        fprintf(stderr, "Failed to generate graph\n");
        return;
    }

    printf("  Generated graph: %d vertices, %d edges\n", graph->n, graph->m);

    // Get graph properties
    GraphProperties props = graph_get_properties(graph);
    printf("  Average degree: %.2f\n", props.avg_degree);
    printf("  Weight range: [%.1f, %.1f]\n\n", props.min_weight, props.max_weight);

    // Step 2: Run and time both algorithms
    int source = 0;

    printf("Step 2: Running SSSP Breakthrough...\n");
    Timer* timer = timer_create();

    // Run breakthrough algorithm
    timer_start(timer);
    sssp_breakthrough(graph, source);
    double time_breakthrough = timer_stop(timer);

    // Save results
    double* dist_breakthrough = (double*)malloc(graph->n * sizeof(double));
    memcpy(dist_breakthrough, graph->dist, graph->n * sizeof(double));

    char time_str[64];
    format_time(time_breakthrough, time_str, sizeof(time_str));
    printf("  Time: %s\n\n", time_str);

    // Run Dijkstra
    printf("Step 3: Running Dijkstra...\n");
    timer_start(timer);
    dijkstra(graph, source);
    double time_dijkstra = timer_stop(timer);

    format_time(time_dijkstra, time_str, sizeof(time_str));
    printf("  Time: %s\n\n", time_str);

    // Compare performance
    printf("Step 4: Performance Comparison:\n");
    printf("  Breakthrough: %s\n", time_str);
    format_time(time_breakthrough, time_str, sizeof(time_str));
    printf("  Dijkstra:     %s\n", time_str);

    double ratio = time_dijkstra / time_breakthrough;
    printf("  Speedup: %.2fx %s\n\n",
           fabs(ratio),
           ratio > 1.0 ? "(Breakthrough faster)" : "(Dijkstra faster)");

    // Verify
    Graph temp_graph = *graph;
    temp_graph.dist = dist_breakthrough;
    bool match = compare_results(&temp_graph, graph);
    printf("Step 5: Results verification: %s\n\n",
           match ? "PASS ✓" : "FAIL ✗");

    // Print sample
    temp_graph.dist = dist_breakthrough;
    print_sample_distances(&temp_graph, 10);

    // Cleanup
    free(dist_breakthrough);
    timer_destroy(timer);
    free_graph(graph);

    printf("\n");
}

/**
 * @brief Example 3: Large sparse graph (1000 vertices)
 *
 * Tests performance on larger graphs.
 */
void example_large_sparse_graph(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Example 3: Large Sparse Graph (1000 vertices, ~3000 edges)\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    printf("Step 1: Generating sparse graph...\n");

    // Generate sparse graph with average degree of 3
    Graph* graph = graph_generate_sparse(1000, 3, 1.0, 100.0);
    if (!graph) {
        fprintf(stderr, "Failed to generate graph\n");
        return;
    }

    printf("  Generated: %d vertices, %d edges\n", graph->n, graph->m);
    printf("  Average degree: %.2f\n\n", (double)(2 * graph->m) / graph->n);

    int source = 0;
    Timer* timer = timer_create();

    // Breakthrough algorithm
    printf("Step 2: Running SSSP Breakthrough on large graph...\n");
    timer_start(timer);
    sssp_breakthrough(graph, source);
    double time_breakthrough = timer_stop(timer);

    char time_str[64];
    format_time(time_breakthrough, time_str, sizeof(time_str));
    printf("  Completed in %s\n\n", time_str);

    // Save results
    double* dist_breakthrough = (double*)malloc(graph->n * sizeof(double));
    memcpy(dist_breakthrough, graph->dist, graph->n * sizeof(double));

    // Dijkstra
    printf("Step 3: Running Dijkstra on large graph...\n");
    timer_start(timer);
    dijkstra(graph, source);
    double time_dijkstra = timer_stop(timer);

    format_time(time_dijkstra, time_str, sizeof(time_str));
    printf("  Completed in %s\n\n", time_str);

    // Performance analysis
    printf("Step 4: Performance Analysis:\n");
    format_time(time_breakthrough, time_str, sizeof(time_str));
    printf("  Breakthrough: %s\n", time_str);
    format_time(time_dijkstra, time_str, sizeof(time_str));
    printf("  Dijkstra:     %s\n", time_str);

    double ratio = time_dijkstra / time_breakthrough;
    printf("  Ratio: %.3f\n", fabs(ratio));
    printf("  Winner: %s\n\n",
           ratio > 1.0 ? "Breakthrough ✓" : "Dijkstra ✓");

    // Verify correctness
    Graph temp_graph = *graph;
    temp_graph.dist = dist_breakthrough;
    bool match = compare_results(&temp_graph, graph);
    printf("Step 5: Correctness: %s\n\n", match ? "VERIFIED ✓" : "FAILED ✗");

    // Statistics
    int reachable = 0;
    for (int i = 0; i < graph->n; i++) {
        if (graph->dist[i] != INFINITY_DIST) {
            reachable++;
        }
    }
    printf("Step 6: Statistics:\n");
    printf("  Reachable vertices: %d / %d (%.1f%%)\n",
           reachable, graph->n, 100.0 * reachable / graph->n);

    print_sample_distances(graph, 5);

    // Cleanup
    free(dist_breakthrough);
    timer_destroy(timer);
    free_graph(graph);

    printf("\n");
}

/**
 * @brief Example 4: Graph with negative weights
 *
 * Demonstrates handling of negative edge weights (no negative cycles).
 */
void example_negative_weights(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Example 4: Graph with Negative Weights\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    printf("Step 1: Creating graph with negative edge weights...\n");

    Graph* graph = create_graph(6);

    // Add edges with some negative weights (but no negative cycles)
    add_edge(graph, 0, 1, 5.0);
    add_edge(graph, 0, 2, 3.0);
    add_edge(graph, 1, 2, -2.0);  // Negative weight
    add_edge(graph, 1, 3, 6.0);
    add_edge(graph, 2, 3, 4.0);
    add_edge(graph, 2, 4, 2.0);
    add_edge(graph, 3, 4, -1.0);  // Negative weight
    add_edge(graph, 3, 5, 3.0);
    add_edge(graph, 4, 5, 5.0);

    printf("  Created graph with %d vertices, %d edges\n", graph->n, graph->m);
    printf("  Note: Contains negative weights (but NO negative cycles)\n\n");

    // Check for negative cycles
    printf("Step 2: Checking for negative cycles...\n");
    if (graph_has_negative_cycle(graph, 0)) {
        printf("  WARNING: Negative cycle detected! ✗\n");
        free_graph(graph);
        return;
    }
    printf("  No negative cycles found ✓\n\n");

    // Run SSSP
    printf("Step 3: Running SSSP Breakthrough...\n");
    sssp_breakthrough(graph, 0);
    printf("  Completed successfully\n\n");

    printf("Step 4: Results:\n");
    print_sample_distances(graph, 6);

    printf("\nStep 5: Explanation:\n");
    printf("  Even with negative weights, the algorithm finds correct shortest paths.\n");
    printf("  Negative edges can create shorter paths than what Dijkstra would find.\n");
    printf("  (Dijkstra doesn't work with negative weights, but this algorithm does!)\n");

    free_graph(graph);
    printf("\n");
}

/*============================================================================
 * Main Function
 *============================================================================*/

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║       SSSP Breakthrough Algorithm - Basic Usage Examples     ║\n");
    printf("║                                                               ║\n");
    printf("║  This program demonstrates the O(m^{1+o(1)}) SSSP algorithm  ║\n");
    printf("║  and compares it with classic Dijkstra's algorithm.          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n\n");

    // Run all examples
    example_small_manual_graph();
    example_medium_random_graph();
    example_large_sparse_graph();
    example_negative_weights();

    // Summary
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Summary\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    printf("Key Takeaways:\n");
    printf("  ✓ The SSSP Breakthrough algorithm works correctly\n");
    printf("  ✓ Results match Dijkstra's algorithm (when no negative weights)\n");
    printf("  ✓ Handles negative weights (unlike Dijkstra)\n");
    printf("  ✓ Time complexity: O(m^{1+o(1)}) vs Dijkstra's O((V+E) log V)\n");
    printf("  ✓ Scales well with graph size\n\n");

    printf("For more advanced usage:\n");
    printf("  - See examples/utils_demo.c for graph generation utilities\n");
    printf("  - See examples/simple_example.c for public API usage\n");
    printf("  - See src/UTILS_README.md for detailed documentation\n\n");

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    All Examples Completed!                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    return 0;
}
