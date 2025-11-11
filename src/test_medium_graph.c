#include "../include/sssp.h"
#include <stdio.h>
#include <stdlib.h>

extern void sssp_set_verbose(bool verbose);

int main() {
    printf("================================================================\n");
    printf("MEDIUM GRAPH TEST (n=50) - Tests Breakthrough Algorithm\n");
    printf("================================================================\n\n");

    /* Create a graph with exactly 50 vertices to test the threshold */
    int n = 50;

    printf("Creating graph with n=%d vertices (triggers breakthrough algorithm)...\n", n);

    sssp_graph_t* graph = sssp_graph_create(n);

    /* Create a simple connected graph: each vertex connects to next few vertices */
    int edges_added = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= 3 && i + j < n; j++) {
            double weight = 1.0 + (double)(i % 5);
            sssp_graph_add_edge(graph, i, i + j, weight);
            edges_added++;
        }
    }

    printf("Graph created: %d vertices, %d edges\n\n", n, edges_added);

    /* Enable verbose mode to see algorithm execution */
    sssp_set_verbose(true);

    printf("Running SSSP from vertex 0...\n\n");

    /* Solve SSSP from vertex 0 */
    sssp_result_t* result = sssp_solve(graph, 0);

    if (result == NULL) {
        printf("ERROR: SSSP solve failed\n");
        return 1;
    }

    printf("\n");
    printf("================================================================\n");
    printf("RESULTS\n");
    printf("================================================================\n");

    /* Count reachable vertices */
    int reachable = 0;
    for (int i = 0; i < n; i++) {
        if (sssp_result_is_reachable(result, i)) {
            reachable++;
        }
    }

    printf("Reachable vertices: %d/%d\n", reachable, n);

    /* Verify solution */
    bool verified = sssp_result_verify(result);
    printf("Solution verification: %s\n", verified ? "✓ PASSED" : "✗ FAILED");

    /* Show sample distances */
    printf("\nSample distances:\n");
    for (int i = 0; i < 10 && i < n; i++) {
        double dist = sssp_result_get_distance(result, i);
        if (dist < SSSP_INFINITY) {
            printf("  Vertex %2d: %.2f\n", i, dist);
        } else {
            printf("  Vertex %2d: UNREACHABLE\n", i);
        }
    }

    printf("\nLast few vertices:\n");
    for (int i = n - 5; i < n; i++) {
        double dist = sssp_result_get_distance(result, i);
        if (dist < SSSP_INFINITY) {
            printf("  Vertex %2d: %.2f\n", i, dist);
        } else {
            printf("  Vertex %2d: UNREACHABLE\n", i);
        }
    }

    printf("\n================================================================\n");
    if (reachable == n && verified) {
        printf("✓ SUCCESS: All vertices reached and solution verified!\n");
    } else {
        printf("✗ PARTIAL: Some issues detected\n");
        printf("  Expected %d reachable, got %d\n", n, reachable);
        printf("  Verification: %s\n", verified ? "passed" : "failed");
    }
    printf("================================================================\n");

    /* Cleanup */
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return (reachable == n && verified) ? 0 : 1;
}
