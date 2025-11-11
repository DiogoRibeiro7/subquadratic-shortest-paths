#include "../include/sssp.h"
#include <stdio.h>

/* Declare the verbose control function */
extern void sssp_set_verbose(bool verbose);

int main() {
    /* Enable verbose output for debugging */
    sssp_set_verbose(true);

    /* Create the same graph as in the failing test */
    sssp_graph_t* graph = sssp_graph_create(5);
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 2.0);
    sssp_graph_add_edge(graph, 0, 3, 3.0);
    sssp_graph_add_edge(graph, 2, 4, 1.0);
    sssp_graph_add_edge(graph, 3, 4, 2.0);
    sssp_graph_add_edge(graph, 4, 3, 2.0);

    printf("\nGraph structure:\n");
    printf("  0 → 1 (1.0)\n");
    printf("  1 → 2 (2.0)\n");
    printf("  0 → 3 (3.0)\n");
    printf("  2 → 4 (1.0)\n");
    printf("  3 → 4 (2.0)\n");
    printf("  4 → 3 (2.0)\n\n");

    printf("Expected distances from vertex 0:\n");
    printf("  0: 0.0\n");
    printf("  1: 1.0 (0→1)\n");
    printf("  2: 3.0 (0→1→2)\n");
    printf("  3: 3.0 (0→3)\n");
    printf("  4: 4.0 (0→1→2→4)\n\n");

    /* Solve SSSP from vertex 0 */
    sssp_result_t* result = sssp_solve(graph, 0);

    printf("\nActual distances computed:\n");
    for (int i = 0; i < 5; i++) {
        double dist = sssp_result_get_distance(result, i);
        if (dist < SSSP_INFINITY) {
            printf("  %d: %.1f\n", i, dist);
        } else {
            printf("  %d: INFINITY\n", i);
        }
    }

    printf("\nPredecessors:\n");
    for (int i = 0; i < 5; i++) {
        int pred = sssp_result_get_predecessor(result, i);
        if (pred != SSSP_INVALID_VERTEX) {
            printf("  %d ← %d\n", i, pred);
        }
    }

    /* Cleanup */
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
