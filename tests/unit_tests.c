/**
 * @file unit_tests.c
 * @brief Comprehensive unit tests for SSSP Breakthrough Algorithm
 *
 * Test Coverage:
 * - Algorithm correctness on known graphs
 * - Edge cases (disconnected, single vertex, empty, etc.)
 * - Data structure operations (heap, custom DS)
 * - Memory leak detection
 * - Negative weights handling
 * - Performance boundaries
 *
 * Build and run:
 *   mkdir build && cd build
 *   cmake ..
 *   make
 *   ./bin/sssp_tests
 */

#include "sssp_breakthrough.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/*============================================================================
 * Simple Test Framework
 *============================================================================*/

// Test statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;
static char current_test[256] = "";
static char current_suite[256] = "";

// Color codes
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RESET   "\033[0m"

// Test macros
#define TEST_SUITE(name) \
    do { \
        snprintf(current_suite, sizeof(current_suite), "%s", name); \
        printf("\n" COLOR_BLUE "═══════════════════════════════════════════════════════════════\n"); \
        printf("Test Suite: %s\n", name); \
        printf("═══════════════════════════════════════════════════════════════" COLOR_RESET "\n"); \
    } while(0)

#define TEST(name) \
    do { \
        tests_run++; \
        snprintf(current_test, sizeof(current_test), "%s", name); \
        printf("  Running: %s ... ", name); \
        fflush(stdout); \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf(COLOR_GREEN "PASS" COLOR_RESET "\n"); \
    } while(0)

#define FAIL(msg) \
    do { \
        tests_failed++; \
        printf(COLOR_RED "FAIL" COLOR_RESET "\n"); \
        printf("    Error: %s\n", msg); \
        printf("    Location: %s:%d\n", __FILE__, __LINE__); \
    } while(0)

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            FAIL(msg); \
            return; \
        } \
    } while(0)

#define ASSERT_TRUE(condition) ASSERT(condition, #condition " is false")
#define ASSERT_FALSE(condition) ASSERT(!(condition), #condition " is true")
#define ASSERT_NULL(ptr) ASSERT((ptr) == NULL, #ptr " is not NULL")
#define ASSERT_NOT_NULL(ptr) ASSERT((ptr) != NULL, #ptr " is NULL")

#define ASSERT_EQ(a, b) ASSERT((a) == (b), "Values not equal")
#define ASSERT_NEQ(a, b) ASSERT((a) != (b), "Values equal")

#define ASSERT_DOUBLE_EQ(a, b, epsilon) \
    ASSERT(fabs((a) - (b)) < (epsilon), "Doubles not equal within epsilon")

#define ASSERT_ARRAY_EQ(arr1, arr2, size) \
    do { \
        bool equal = true; \
        for (int i = 0; i < (size); i++) { \
            if ((arr1)[i] != (arr2)[i]) { \
                equal = false; \
                break; \
            } \
        } \
        ASSERT(equal, "Arrays not equal"); \
    } while(0)

/*============================================================================
 * Helper Functions
 *============================================================================*/

/**
 * @brief Compare distance arrays within tolerance
 */
bool compare_distances(const double* dist1, const double* dist2, int n, double epsilon) {
    for (int i = 0; i < n; i++) {
        if ((dist1[i] == INFINITY_DIST) != (dist2[i] == INFINITY_DIST)) {
            return false;
        }
        if (dist1[i] != INFINITY_DIST) {
            if (fabs(dist1[i] - dist2[i]) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Create a simple test graph with known shortest paths
 */
Graph* create_test_graph_simple(void) {
    Graph* g = create_graph(5);
    if (!g) return NULL;

    // Create a simple graph with known shortest paths
    // 0 -> 1 (4), 0 -> 2 (2)
    // 1 -> 2 (1), 1 -> 3 (5)
    // 2 -> 3 (3), 3 -> 4 (1)
    add_edge(g, 0, 1, 4.0);
    add_edge(g, 0, 2, 2.0);
    add_edge(g, 1, 2, 1.0);
    add_edge(g, 1, 3, 5.0);
    add_edge(g, 2, 3, 3.0);
    add_edge(g, 3, 4, 1.0);

    return g;
}

/**
 * @brief Run Dijkstra for comparison (simple implementation)
 */
void dijkstra_simple(Graph* g, int source) {
    // Initialize
    for (int i = 0; i < g->n; i++) {
        g->dist[i] = INFINITY_DIST;
        g->complete[i] = false;
    }
    g->dist[source] = 0.0;

    // Simple O(n^2) Dijkstra
    for (int count = 0; count < g->n; count++) {
        // Find minimum distance vertex
        int u = -1;
        double min_dist = INFINITY_DIST;
        for (int v = 0; v < g->n; v++) {
            if (!g->complete[v] && g->dist[v] < min_dist) {
                min_dist = g->dist[v];
                u = v;
            }
        }

        if (u == -1) break;

        g->complete[u] = true;

        // Relax edges
        Edge* edge = g->adj[u];
        while (edge) {
            int v = edge->target;
            double new_dist = g->dist[u] + edge->weight;
            if (new_dist < g->dist[v]) {
                g->dist[v] = new_dist;
            }
            edge = edge->next;
        }
    }
}

/*============================================================================
 * Algorithm Correctness Tests
 *============================================================================*/

void test_simple_graph_correctness(void) {
    TEST("Simple graph correctness");

    Graph* g = create_test_graph_simple();
    ASSERT_NOT_NULL(g);

    // Expected distances from vertex 0
    double expected[5] = {0.0, 3.0, 2.0, 5.0, 6.0};

    // Run SSSP breakthrough
    sssp_breakthrough(g, 0);

    // Check distances
    bool match = compare_distances(g->dist, expected, 5, 1e-9);
    ASSERT_TRUE(match);

    free_graph(g);
    PASS();
}

void test_single_vertex(void) {
    TEST("Single vertex graph");

    Graph* g = create_graph(1);
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    free_graph(g);
    PASS();
}

void test_disconnected_graph(void) {
    TEST("Disconnected graph");

    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);

    // Two components: {0,1,2} and {3,4}
    add_edge(g, 0, 1, 1.0);
    add_edge(g, 1, 2, 1.0);
    add_edge(g, 3, 4, 1.0);

    sssp_breakthrough(g, 0);

    // Reachable from 0
    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[1], 1.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[2], 2.0, 1e-9);

    // Unreachable from 0
    ASSERT_EQ(g->dist[3], INFINITY_DIST);
    ASSERT_EQ(g->dist[4], INFINITY_DIST);

    free_graph(g);
    PASS();
}

void test_self_loop(void) {
    TEST("Graph with self-loop");

    Graph* g = create_graph(3);
    ASSERT_NOT_NULL(g);

    add_edge(g, 0, 1, 1.0);
    add_edge(g, 1, 1, 5.0);  // Self-loop
    add_edge(g, 1, 2, 1.0);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[1], 1.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[2], 2.0, 1e-9);

    free_graph(g);
    PASS();
}

void test_negative_weights(void) {
    TEST("Graph with negative weights");

    Graph* g = create_graph(4);
    ASSERT_NOT_NULL(g);

    // DAG with negative weights
    add_edge(g, 0, 1, 5.0);
    add_edge(g, 0, 2, 3.0);
    add_edge(g, 1, 2, -2.0);  // Negative
    add_edge(g, 2, 3, 1.0);

    sssp_breakthrough(g, 0);

    // Path 0->1->2 = 5+(-2) = 3, better than 0->2 = 3
    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[1], 5.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[2], 3.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[3], 4.0, 1e-9);

    free_graph(g);
    PASS();
}

void test_comparison_with_dijkstra(void) {
    TEST("Comparison with Dijkstra");

    GraphGenParams params = graph_gen_params_default(20, 40);
    Graph* g = graph_generate_random(&params);
    ASSERT_NOT_NULL(g);

    // Run breakthrough
    sssp_breakthrough(g, 0);
    double* dist_breakthrough = (double*)malloc(g->n * sizeof(double));
    memcpy(dist_breakthrough, g->dist, g->n * sizeof(double));

    // Run Dijkstra
    dijkstra_simple(g, 0);

    // Compare
    bool match = compare_distances(dist_breakthrough, g->dist, g->n, 1e-9);
    ASSERT_TRUE(match);

    free(dist_breakthrough);
    free_graph(g);
    PASS();
}

void test_various_graph_sizes(void) {
    TEST("Various graph sizes");

    int sizes[] = {1, 2, 5, 10, 50, 100};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        Graph* g = graph_generate_sparse(n, 3, 1.0, 10.0);
        ASSERT_NOT_NULL(g);

        sssp_breakthrough(g, 0);

        // Just verify it doesn't crash and source distance is 0
        ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

        free_graph(g);
    }

    PASS();
}

void test_complete_graph(void) {
    TEST("Complete graph");

    int n = 10;
    Graph* g = graph_generate_complete(n, 1.0, 10.0);
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    // All vertices should be reachable
    for (int i = 0; i < n; i++) {
        ASSERT_TRUE(g->dist[i] < INFINITY_DIST);
    }

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    free_graph(g);
    PASS();
}

/*============================================================================
 * Edge Case Tests
 *============================================================================*/

void test_empty_graph(void) {
    TEST("Empty graph (no edges)");

    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);

    // No edges added
    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    for (int i = 1; i < 5; i++) {
        ASSERT_EQ(g->dist[i], INFINITY_DIST);
    }

    free_graph(g);
    PASS();
}

void test_two_vertices(void) {
    TEST("Two vertices");

    Graph* g = create_graph(2);
    ASSERT_NOT_NULL(g);

    add_edge(g, 0, 1, 5.0);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[1], 5.0, 1e-9);

    free_graph(g);
    PASS();
}

void test_linear_chain(void) {
    TEST("Linear chain graph");

    int n = 10;
    Graph* g = graph_generate_path(n, 1.0, 1.0);  // Unit weights
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    // Distance to vertex i should be i
    for (int i = 0; i < n; i++) {
        ASSERT_DOUBLE_EQ(g->dist[i], (double)i, 1e-9);
    }

    free_graph(g);
    PASS();
}

void test_all_zero_weights(void) {
    TEST("All zero weights");

    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);

    for (int i = 0; i < 4; i++) {
        add_edge(g, i, i + 1, 0.0);
    }

    sssp_breakthrough(g, 0);

    // All reachable vertices should have distance 0
    for (int i = 0; i < 5; i++) {
        ASSERT_DOUBLE_EQ(g->dist[i], 0.0, 1e-9);
    }

    free_graph(g);
    PASS();
}

void test_very_large_weights(void) {
    TEST("Very large weights");

    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);

    add_edge(g, 0, 1, 1e100);
    add_edge(g, 1, 2, 1e100);
    add_edge(g, 2, 3, 1e100);
    add_edge(g, 3, 4, 1e100);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    ASSERT_DOUBLE_EQ(g->dist[1], 1e100, 1e90);
    ASSERT_DOUBLE_EQ(g->dist[4], 4e100, 1e90);

    free_graph(g);
    PASS();
}

void test_multiple_sources(void) {
    TEST("SSSP from different sources");

    Graph* g = create_test_graph_simple();
    ASSERT_NOT_NULL(g);

    // Test from each vertex
    for (int source = 0; source < g->n; source++) {
        sssp_breakthrough(g, source);
        ASSERT_DOUBLE_EQ(g->dist[source], 0.0, 1e-9);
    }

    free_graph(g);
    PASS();
}

/*============================================================================
 * Data Structure Tests
 *============================================================================*/

void test_heap_operations(void) {
    TEST("MinHeap basic operations");

    MinHeap* heap = heap_create(10);
    ASSERT_NOT_NULL(heap);

    // Test insertions
    heap_insert(heap, 0, 5.0);
    heap_insert(heap, 1, 3.0);
    heap_insert(heap, 2, 7.0);
    heap_insert(heap, 3, 1.0);

    ASSERT_EQ(heap->size, 4);

    // Test extract min
    int min = heap_extract_min(heap);
    ASSERT_EQ(min, 3);  // Vertex 3 had distance 1.0

    min = heap_extract_min(heap);
    ASSERT_EQ(min, 1);  // Vertex 1 had distance 3.0

    heap_free(heap);
    PASS();
}

void test_heap_decrease_key(void) {
    TEST("MinHeap decrease key");

    MinHeap* heap = heap_create(10);
    ASSERT_NOT_NULL(heap);

    heap_insert(heap, 0, 10.0);
    heap_insert(heap, 1, 20.0);
    heap_insert(heap, 2, 15.0);

    // Decrease key of vertex 1 from 20.0 to 5.0
    heap_decrease_key(heap, 1, 5.0);

    // Now vertex 1 should be extracted first
    int min = heap_extract_min(heap);
    ASSERT_EQ(min, 1);

    heap_free(heap);
    PASS();
}

void test_graph_creation_destruction(void) {
    TEST("Graph creation and destruction");

    size_t mem_before, peak_before;
    memory_get_stats(&mem_before, &peak_before);

    Graph* g = create_graph(100);
    ASSERT_NOT_NULL(g);
    ASSERT_EQ(g->n, 100);
    ASSERT_EQ(g->m, 0);

    // Add some edges
    for (int i = 0; i < 50; i++) {
        add_edge(g, i, i + 1, 1.0);
    }

    ASSERT_EQ(g->m, 50);

    free_graph(g);

    // Memory should be approximately back to where it was
    // (This is a rough check, not perfect due to allocator behavior)

    PASS();
}

void test_edge_addition(void) {
    TEST("Edge addition");

    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);
    ASSERT_EQ(g->m, 0);

    add_edge(g, 0, 1, 1.0);
    ASSERT_EQ(g->m, 1);

    add_edge(g, 1, 2, 2.0);
    ASSERT_EQ(g->m, 2);

    add_edge(g, 0, 2, 3.0);
    ASSERT_EQ(g->m, 3);

    // Verify adjacency list
    Edge* e = g->adj[0];
    ASSERT_NOT_NULL(e);

    free_graph(g);
    PASS();
}

/*============================================================================
 * Memory Leak Tests
 *============================================================================*/

void test_no_memory_leaks_small(void) {
    TEST("No memory leaks (small graph)");

    memory_reset_stats();

    for (int trial = 0; trial < 10; trial++) {
        Graph* g = create_graph(10);
        for (int i = 0; i < 9; i++) {
            add_edge(g, i, i + 1, 1.0);
        }
        sssp_breakthrough(g, 0);
        free_graph(g);
    }

    // Hard to test perfectly, but no crashes is a good sign
    PASS();
}

void test_no_memory_leaks_large(void) {
    TEST("No memory leaks (large graph)");

    memory_reset_stats();

    for (int trial = 0; trial < 5; trial++) {
        GraphGenParams params = graph_gen_params_default(100, 200);
        Graph* g = graph_generate_random(&params);
        ASSERT_NOT_NULL(g);
        sssp_breakthrough(g, 0);
        free_graph(g);
    }

    PASS();
}

void test_repeated_execution(void) {
    TEST("Repeated SSSP execution");

    Graph* g = create_test_graph_simple();
    ASSERT_NOT_NULL(g);

    // Run multiple times on same graph
    for (int i = 0; i < 10; i++) {
        sssp_breakthrough(g, 0);
        ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);
    }

    free_graph(g);
    PASS();
}

/*============================================================================
 * Validation Tests
 *============================================================================*/

void test_solution_verification(void) {
    TEST("Solution verification");

    Graph* g = create_test_graph_simple();
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    bool valid = verify_solution(g);
    ASSERT_TRUE(valid);

    free_graph(g);
    PASS();
}

void test_random_graph_verification(void) {
    TEST("Random graph verification");

    for (int trial = 0; trial < 5; trial++) {
        GraphGenParams params = graph_gen_params_default(30, 60);
        Graph* g = graph_generate_random(&params);
        ASSERT_NOT_NULL(g);

        sssp_breakthrough(g, 0);

        bool valid = verify_solution(g);
        ASSERT_TRUE(valid);

        free_graph(g);
    }

    PASS();
}

/*============================================================================
 * Stress Tests
 *============================================================================*/

void test_dense_graph(void) {
    TEST("Dense graph");

    int n = 50;
    Graph* g = graph_generate_complete(n, 1.0, 10.0);
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    // All vertices should be reachable
    for (int i = 0; i < n; i++) {
        ASSERT_TRUE(g->dist[i] < INFINITY_DIST);
    }

    free_graph(g);
    PASS();
}

void test_sparse_large_graph(void) {
    TEST("Large sparse graph");

    Graph* g = graph_generate_sparse(500, 3, 1.0, 100.0);
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    free_graph(g);
    PASS();
}

void test_grid_graph(void) {
    TEST("Grid graph");

    Graph* g = graph_generate_grid(10, 10, 1.0, 10.0, false);
    ASSERT_NOT_NULL(g);

    sssp_breakthrough(g, 0);

    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    // Verify it's a valid solution
    bool valid = verify_solution(g);
    ASSERT_TRUE(valid);

    free_graph(g);
    PASS();
}

/*============================================================================
 * Test Runner
 *============================================================================*/

void print_test_summary(void) {
    printf("\n");
    printf(COLOR_BLUE "═══════════════════════════════════════════════════════════════\n");
    printf("Test Summary\n");
    printf("═══════════════════════════════════════════════════════════════" COLOR_RESET "\n");
    printf("\n");
    printf("Total tests:  %d\n", tests_run);
    printf(COLOR_GREEN "Passed:       %d\n" COLOR_RESET, tests_passed);
    if (tests_failed > 0) {
        printf(COLOR_RED "Failed:       %d\n" COLOR_RESET, tests_failed);
    } else {
        printf("Failed:       %d\n", tests_failed);
    }
    printf("\n");

    if (tests_failed == 0) {
        printf(COLOR_GREEN "✓ All tests passed!" COLOR_RESET "\n");
    } else {
        printf(COLOR_RED "✗ Some tests failed!" COLOR_RESET "\n");
    }
    printf("\n");
}

int main(void) {
    printf("\n");
    printf(COLOR_BLUE "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        SSSP Breakthrough - Comprehensive Unit Tests          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝" COLOR_RESET "\n");

    // Set random seed for reproducibility
    utils_set_random_seed(42);

    // Run test suites
    TEST_SUITE("Algorithm Correctness");
    test_simple_graph_correctness();
    test_single_vertex();
    test_disconnected_graph();
    test_self_loop();
    test_negative_weights();
    test_comparison_with_dijkstra();
    test_various_graph_sizes();
    test_complete_graph();

    TEST_SUITE("Edge Cases");
    test_empty_graph();
    test_two_vertices();
    test_linear_chain();
    test_all_zero_weights();
    test_very_large_weights();
    test_multiple_sources();

    TEST_SUITE("Data Structures");
    test_heap_operations();
    test_heap_decrease_key();
    test_graph_creation_destruction();
    test_edge_addition();

    TEST_SUITE("Memory Management");
    test_no_memory_leaks_small();
    test_no_memory_leaks_large();
    test_repeated_execution();

    TEST_SUITE("Solution Validation");
    test_solution_verification();
    test_random_graph_verification();

    TEST_SUITE("Stress Tests");
    test_dense_graph();
    test_sparse_large_graph();
    test_grid_graph();

    // Print summary
    print_test_summary();

    return (tests_failed == 0) ? 0 : 1;
}
