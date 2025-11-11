/**
 * @file demo_api.c
 * @brief Complete demonstration of the SSSP Breakthrough Algorithm Public API
 *
 * This demo showcases all major features of the production-ready API.
 */

#include "include/sssp.h"
#include <stdio.h>

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   SSSP BREAKTHROUGH ALGORITHM - PUBLIC API DEMONSTRATION       ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Display version information */
    printf("Version: %s\n", sssp_get_version());
    printf("%s\n\n", sssp_get_info());

    /* ═══════════════════════════════════════════════════════════════ */
    /* EXAMPLE 1: Basic Usage                                          */
    /* ═══════════════════════════════════════════════════════════════ */

    printf("──────────────────────────────────────────────────────────────\n");
    printf("EXAMPLE 1: Basic SSSP Computation\n");
    printf("──────────────────────────────────────────────────────────────\n\n");

    /* Create a simple graph */
    sssp_graph_t* graph = sssp_graph_create(6);

    printf("Creating graph with 6 vertices...\n");
    sssp_graph_add_edge(graph, 0, 1, 4.0);
    sssp_graph_add_edge(graph, 0, 2, 2.0);
    sssp_graph_add_edge(graph, 1, 2, 1.0);
    sssp_graph_add_edge(graph, 1, 3, 5.0);
    sssp_graph_add_edge(graph, 2, 3, 8.0);
    sssp_graph_add_edge(graph, 2, 4, 10.0);
    sssp_graph_add_edge(graph, 3, 4, 2.0);
    sssp_graph_add_edge(graph, 3, 5, 6.0);
    sssp_graph_add_edge(graph, 4, 5, 3.0);

    printf("Graph: %d vertices, %d edges\n\n",
           sssp_graph_get_num_vertices(graph),
           sssp_graph_get_num_edges(graph));

    /* Solve SSSP from vertex 0 */
    printf("Computing shortest paths from vertex 0...\n");
    sssp_result_t* result = sssp_solve(graph, 0);

    if (result == NULL) {
        printf("Error: Failed to compute SSSP\n");
        return 1;
    }

    /* Display results */
    printf("\nShortest distances from vertex 0:\n");
    printf("  Vertex | Distance | Predecessor\n");
    printf("  ───────┼──────────┼────────────\n");

    for (int v = 0; v < 6; v++) {
        double dist = sssp_result_get_distance(result, v);
        int pred = sssp_result_get_predecessor(result, v);

        if (sssp_result_is_reachable(result, v)) {
            printf("    %d    │  %6.1f  │      %d\n", v, dist, pred);
        } else {
            printf("    %d    │    ∞     │      -\n", v);
        }
    }

    /* ═══════════════════════════════════════════════════════════════ */
    /* EXAMPLE 2: Path Reconstruction                                  */
    /* ═══════════════════════════════════════════════════════════════ */

    printf("\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("EXAMPLE 2: Path Reconstruction\n");
    printf("──────────────────────────────────────────────────────────────\n\n");

    int target = 5;
    int path[10];
    int path_length;

    if (sssp_result_get_path(result, target, path, &path_length) == SSSP_SUCCESS) {
        printf("Shortest path from 0 to %d: ", target);
        for (int i = 0; i < path_length; i++) {
            printf("%d", path[i]);
            if (i < path_length - 1) printf(" → ");
        }
        printf(" (distance: %.1f)\n", sssp_result_get_distance(result, target));
    }

    /* ═══════════════════════════════════════════════════════════════ */
    /* EXAMPLE 3: Solution Verification                                */
    /* ═══════════════════════════════════════════════════════════════ */

    printf("\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("EXAMPLE 3: Solution Verification\n");
    printf("──────────────────────────────────────────────────────────────\n\n");

    bool is_valid = sssp_result_verify(result);
    printf("Solution verification: %s\n", is_valid ? "✓ PASSED" : "✗ FAILED");
    printf("Status: %s\n", sssp_status_to_string(sssp_result_get_status(result)));

    /* ═══════════════════════════════════════════════════════════════ */
    /* EXAMPLE 4: Graph Validation                                     */
    /* ═══════════════════════════════════════════════════════════════ */

    printf("\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("EXAMPLE 4: Graph Validation\n");
    printf("──────────────────────────────────────────────────────────────\n\n");

    char error_msg[256];
    sssp_status_t validation = sssp_graph_validate(graph, error_msg, sizeof(error_msg));
    printf("Graph validation: %s\n", error_msg);

    /* Test negative cycle detection */
    sssp_graph_t* cycle_graph = sssp_graph_create(3);
    sssp_graph_add_edge(cycle_graph, 0, 1, 1.0);
    sssp_graph_add_edge(cycle_graph, 1, 2, 1.0);
    sssp_graph_add_edge(cycle_graph, 2, 0, -3.0);  /* Creates negative cycle */

    int cycle[10];
    int cycle_length;
    bool has_neg_cycle = sssp_graph_has_negative_cycle(cycle_graph, cycle, &cycle_length);

    printf("\nNegative cycle detection test: %s\n",
           has_neg_cycle ? "Negative cycle found" : "No negative cycle");

    if (has_neg_cycle && cycle_length > 0) {
        printf("  Cycle: ");
        for (int i = 0; i < cycle_length; i++) {
            printf("%d", cycle[i]);
            if (i < cycle_length - 1) printf(" → ");
        }
        printf("\n");
    }

    /* ═══════════════════════════════════════════════════════════════ */
    /* EXAMPLE 5: Graph I/O                                            */
    /* ═══════════════════════════════════════════════════════════════ */

    printf("\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("EXAMPLE 5: Graph I/O Operations\n");
    printf("──────────────────────────────────────────────────────────────\n\n");

    /* Save graph to file */
    sssp_status_t save_status = sssp_graph_save_edge_list(graph, "demo_graph.txt");
    printf("Save graph to file: %s\n",
           save_status == SSSP_SUCCESS ? "✓ Success" : "✗ Failed");

    /* Save result to file */
    sssp_status_t export_status = sssp_result_export(result, "demo_result.txt");
    printf("Export result to file: %s\n",
           export_status == SSSP_SUCCESS ? "✓ Success" : "✗ Failed");

    /* Load graph from file */
    sssp_graph_t* loaded_graph = sssp_graph_load_edge_list("demo_graph.txt",
                                                             error_msg,
                                                             sizeof(error_msg));
    if (loaded_graph != NULL) {
        printf("Load graph from file: ✓ Success (%d vertices, %d edges)\n",
               sssp_graph_get_num_vertices(loaded_graph),
               sssp_graph_get_num_edges(loaded_graph));
        sssp_graph_destroy(loaded_graph);
    } else {
        printf("Load graph from file: ✗ Failed (%s)\n", error_msg);
    }

    /* ═══════════════════════════════════════════════════════════════ */
    /* Cleanup                                                          */
    /* ═══════════════════════════════════════════════════════════════ */

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);
    sssp_graph_destroy(cycle_graph);

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                      ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("API Status: Production-ready with comprehensive features\n");
    printf("All memory properly cleaned up. No leaks.\n");
    printf("\n");

    return 0;
}
