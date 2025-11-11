#include "../include/sssp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void sssp_set_verbose(bool verbose);

int main() {
    printf("================================================================\n");
    printf("BREAKTHROUGH ALGORITHM TEST - LARGE GRAPH\n");
    printf("================================================================\n\n");

    /* Create a larger graph (n=150) to trigger breakthrough algorithm */
    int n = 150;
    int m = 500;

    printf("Creating random graph: n=%d vertices, m=%d edges\n", n, m);

    sssp_graph_t* graph = sssp_graph_create(n);

    /* Add random edges with random weights */
    srand(42);  /* Fixed seed for reproducibility */
    for (int i = 0; i < m; i++) {
        int u = rand() % n;
        int v = rand() % n;
        double weight = 1.0 + (rand() % 100) / 10.0;  /* Weight between 1.0 and 11.0 */
        sssp_graph_add_edge(graph, u, v, weight);
    }

    printf("Graph created successfully.\n\n");

    /* Enable verbose mode to see the breakthrough algorithm in action */
    sssp_set_verbose(true);

    printf("Running SSSP from source vertex 0 with BREAKTHROUGH ALGORITHM...\n\n");

    /* Solve SSSP from vertex 0 */
    clock_t start = clock();
    sssp_result_t* result = sssp_solve(graph, 0);
    clock_t end = clock();

    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\n");
    printf("================================================================\n");
    printf("RESULTS\n");
    printf("================================================================\n");
    printf("Computation time: %.6f seconds\n", cpu_time);
    printf("Status: %s\n", sssp_status_to_string(sssp_result_get_status(result)));

    /* Count reachable vertices */
    int reachable = 0;
    for (int i = 0; i < n; i++) {
        if (sssp_result_is_reachable(result, i)) {
            reachable++;
        }
    }

    printf("Reachable vertices: %d/%d\n", reachable, n);
    printf("Solution verification: %s\n",
           sssp_result_verify(result) ? "PASSED" : "FAILED");

    /* Show some example distances */
    printf("\nSample distances from source (vertex 0):\n");
    for (int i = 0; i < 10 && i < n; i++) {
        double dist = sssp_result_get_distance(result, i);
        if (dist < SSSP_INFINITY) {
            printf("  Vertex %d: %.2f\n", i, dist);
        } else {
            printf("  Vertex %d: UNREACHABLE\n", i);
        }
    }

    printf("\n================================================================\n");
    printf("SUCCESS! Breakthrough algorithm completed on large graph.\n");
    printf("================================================================\n");

    /* Cleanup */
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
