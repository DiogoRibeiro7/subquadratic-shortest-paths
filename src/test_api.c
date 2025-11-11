/**
 * @file test_api.c
 * @brief Comprehensive test suite for SSSP Public API
 *
 * Tests all functions in the API including error handling, path reconstruction,
 * graph validation, and I/O operations.
 */

#include "../include/sssp.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/* ANSI color codes for test output */
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_START(name) \
    printf("\n" COLOR_BLUE "TEST: %s" COLOR_RESET "\n", name);

#define TEST_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            printf("  " COLOR_GREEN "✓" COLOR_RESET " %s\n", msg); \
            tests_passed++; \
        } else { \
            printf("  " COLOR_RED "✗" COLOR_RESET " %s\n", msg); \
            tests_failed++; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(a, b, msg) \
    TEST_ASSERT((a) == (b), msg)

#define TEST_ASSERT_NEAR(a, b, eps, msg) \
    TEST_ASSERT(fabs((a) - (b)) < (eps), msg)

/* ============================================================================
 * TEST CASES
 * ============================================================================ */

/**
 * Test basic graph creation and destruction
 */
void test_graph_creation() {
    TEST_START("Graph Creation and Destruction");

    /* Test valid creation */
    sssp_graph_t* graph = sssp_graph_create(10);
    TEST_ASSERT(graph != NULL, "Create graph with 10 vertices");
    TEST_ASSERT_EQ(sssp_graph_get_num_vertices(graph), 10, "Correct vertex count");
    TEST_ASSERT_EQ(sssp_graph_get_num_edges(graph), 0, "Initially no edges");

    /* Test edge addition */
    sssp_status_t status = sssp_graph_add_edge(graph, 0, 1, 5.0);
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Add valid edge");
    TEST_ASSERT_EQ(sssp_graph_get_num_edges(graph), 1, "Edge count updated");

    /* Test invalid vertex IDs */
    status = sssp_graph_add_edge(graph, -1, 1, 5.0);
    TEST_ASSERT_EQ(status, SSSP_ERROR_INVALID_VERTEX, "Reject negative vertex ID");

    status = sssp_graph_add_edge(graph, 0, 100, 5.0);
    TEST_ASSERT_EQ(status, SSSP_ERROR_INVALID_VERTEX, "Reject out-of-range vertex ID");

    /* Test null pointer */
    status = sssp_graph_add_edge(NULL, 0, 1, 5.0);
    TEST_ASSERT_EQ(status, SSSP_ERROR_NULL_POINTER, "Reject NULL graph");

    /* Clean up */
    sssp_graph_destroy(graph);
    sssp_graph_destroy(NULL);  /* Should not crash */

    /* Test invalid creation */
    graph = sssp_graph_create(0);
    TEST_ASSERT(graph == NULL, "Reject zero vertices");

    graph = sssp_graph_create(-10);
    TEST_ASSERT(graph == NULL, "Reject negative vertices");

    graph = sssp_graph_create(SSSP_MAX_VERTICES + 1);
    TEST_ASSERT(graph == NULL, "Reject too many vertices");
}

/**
 * Test basic SSSP solving
 */
void test_basic_sssp() {
    TEST_START("Basic SSSP Solving");

    /* Create simple graph:
     *     1
     *   0 → 1 → 2
     *   ↓  2   ↓ 1
     *   3 ←--→ 4
     *      2
     */
    sssp_graph_t* graph = sssp_graph_create(5);
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 2.0);
    sssp_graph_add_edge(graph, 0, 3, 3.0);
    sssp_graph_add_edge(graph, 2, 4, 1.0);
    sssp_graph_add_edge(graph, 3, 4, 2.0);
    sssp_graph_add_edge(graph, 4, 3, 2.0);

    /* Solve SSSP from vertex 0 */
    sssp_result_t* result = sssp_solve(graph, 0);
    TEST_ASSERT(result != NULL, "SSSP solve succeeds");

    /* Check status */
    TEST_ASSERT_EQ(sssp_result_get_status(result), SSSP_SUCCESS, "Result status is success");
    TEST_ASSERT_EQ(sssp_result_get_source(result), 0, "Correct source vertex");

    /* Check distances */
    TEST_ASSERT_NEAR(sssp_result_get_distance(result, 0), 0.0, 1e-9, "Distance to source is 0");
    TEST_ASSERT_NEAR(sssp_result_get_distance(result, 1), 1.0, 1e-9, "Distance to vertex 1");
    TEST_ASSERT_NEAR(sssp_result_get_distance(result, 2), 3.0, 1e-9, "Distance to vertex 2");
    TEST_ASSERT_NEAR(sssp_result_get_distance(result, 3), 3.0, 1e-9, "Distance to vertex 3");
    TEST_ASSERT_NEAR(sssp_result_get_distance(result, 4), 4.0, 1e-9, "Distance to vertex 4");

    /* Check reachability */
    TEST_ASSERT(sssp_result_is_reachable(result, 0), "Vertex 0 is reachable");
    TEST_ASSERT(sssp_result_is_reachable(result, 4), "Vertex 4 is reachable");

    /* Check predecessors */
    TEST_ASSERT_EQ(sssp_result_get_predecessor(result, 0), SSSP_INVALID_VERTEX, "Source has no predecessor");
    TEST_ASSERT_EQ(sssp_result_get_predecessor(result, 1), 0, "Vertex 1's predecessor");

    /* Verify solution */
    TEST_ASSERT(sssp_result_verify(result), "Solution passes verification");

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);
}

/**
 * Test path reconstruction
 */
void test_path_reconstruction() {
    TEST_START("Path Reconstruction");

    /* Create graph with known paths */
    sssp_graph_t* graph = sssp_graph_create(5);
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 1.0);
    sssp_graph_add_edge(graph, 2, 3, 1.0);
    sssp_graph_add_edge(graph, 3, 4, 1.0);
    sssp_graph_add_edge(graph, 0, 4, 10.0);  /* Longer direct path */

    sssp_result_t* result = sssp_solve(graph, 0);

    /* Test path to vertex 4 */
    int path[10];
    int path_length;
    sssp_status_t status = sssp_result_get_path(result, 4, path, &path_length);

    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Path reconstruction succeeds");
    TEST_ASSERT_EQ(path_length, 5, "Path has correct length");

    if (path_length == 5) {
        TEST_ASSERT_EQ(path[0], 0, "Path starts at source");
        TEST_ASSERT_EQ(path[1], 1, "Path goes through vertex 1");
        TEST_ASSERT_EQ(path[2], 2, "Path goes through vertex 2");
        TEST_ASSERT_EQ(path[3], 3, "Path goes through vertex 3");
        TEST_ASSERT_EQ(path[4], 4, "Path ends at target");
    }

    /* Test path to source */
    status = sssp_result_get_path(result, 0, path, &path_length);
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Path to source succeeds");
    TEST_ASSERT_EQ(path_length, 1, "Path to source has length 1");
    if (path_length == 1) {
        TEST_ASSERT_EQ(path[0], 0, "Path to source is just source");
    }

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);
}

/**
 * Test unreachable vertices
 */
void test_unreachable_vertices() {
    TEST_START("Unreachable Vertices");

    /* Create disconnected graph */
    sssp_graph_t* graph = sssp_graph_create(5);
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 1.0);
    /* Vertices 3 and 4 are unreachable from 0 */

    sssp_result_t* result = sssp_solve(graph, 0);

    TEST_ASSERT(sssp_result_is_reachable(result, 0), "Source is reachable");
    TEST_ASSERT(sssp_result_is_reachable(result, 1), "Vertex 1 is reachable");
    TEST_ASSERT(sssp_result_is_reachable(result, 2), "Vertex 2 is reachable");
    TEST_ASSERT(!sssp_result_is_reachable(result, 3), "Vertex 3 is unreachable");
    TEST_ASSERT(!sssp_result_is_reachable(result, 4), "Vertex 4 is unreachable");

    TEST_ASSERT_EQ(sssp_result_get_distance(result, 3), SSSP_INFINITY, "Unreachable distance is infinity");

    /* Test path to unreachable vertex */
    int path[10];
    int path_length;
    sssp_status_t status = sssp_result_get_path(result, 3, path, &path_length);
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Path query succeeds for unreachable vertex");
    TEST_ASSERT_EQ(path_length, 0, "Path length is 0 for unreachable");

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);
}

/**
 * Test negative cycle detection
 */
void test_negative_cycle_detection() {
    TEST_START("Negative Cycle Detection");

    /* Graph without negative cycle */
    sssp_graph_t* graph1 = sssp_graph_create(3);
    sssp_graph_add_edge(graph1, 0, 1, -1.0);
    sssp_graph_add_edge(graph1, 1, 2, -1.0);
    sssp_graph_add_edge(graph1, 2, 0, 5.0);

    TEST_ASSERT(!sssp_graph_has_negative_cycle(graph1, NULL, NULL), "No negative cycle detected");

    char error_msg[256];
    sssp_status_t status = sssp_graph_validate(graph1, error_msg, sizeof(error_msg));
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Graph without negative cycle is valid");

    /* Graph with negative cycle */
    sssp_graph_t* graph2 = sssp_graph_create(3);
    sssp_graph_add_edge(graph2, 0, 1, 1.0);
    sssp_graph_add_edge(graph2, 1, 2, -2.0);
    sssp_graph_add_edge(graph2, 2, 0, -2.0);

    int cycle[10];
    int cycle_length;
    TEST_ASSERT(sssp_graph_has_negative_cycle(graph2, cycle, &cycle_length), "Negative cycle detected");
    TEST_ASSERT(cycle_length > 0, "Cycle has non-zero length");

    status = sssp_graph_validate(graph2, error_msg, sizeof(error_msg));
    TEST_ASSERT_EQ(status, SSSP_ERROR_NEGATIVE_CYCLE, "Graph validation detects negative cycle");

    sssp_graph_destroy(graph1);
    sssp_graph_destroy(graph2);
}

/**
 * Test graph I/O operations
 */
void test_graph_io() {
    TEST_START("Graph I/O Operations");

    /* Create a test graph */
    sssp_graph_t* graph = sssp_graph_create(4);
    sssp_graph_add_edge(graph, 0, 1, 2.5);
    sssp_graph_add_edge(graph, 1, 2, 3.7);
    sssp_graph_add_edge(graph, 2, 3, 1.2);
    sssp_graph_add_edge(graph, 0, 3, 8.9);

    /* Save to file */
    sssp_status_t status = sssp_graph_save_edge_list(graph, "test_graph.txt");
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Save graph to file");

    /* Load from file */
    char error_msg[256];
    sssp_graph_t* loaded_graph = sssp_graph_load_edge_list("test_graph.txt", error_msg, sizeof(error_msg));
    TEST_ASSERT(loaded_graph != NULL, "Load graph from file");

    if (loaded_graph != NULL) {
        TEST_ASSERT_EQ(sssp_graph_get_num_vertices(loaded_graph), 4, "Loaded graph has correct vertices");
        TEST_ASSERT_EQ(sssp_graph_get_num_edges(loaded_graph), 4, "Loaded graph has correct edges");
    }

    sssp_graph_destroy(loaded_graph);
    sssp_graph_destroy(graph);

    /* Clean up test file */
    remove("test_graph.txt");
}

/**
 * Test result export
 */
void test_result_export() {
    TEST_START("Result Export");

    sssp_graph_t* graph = sssp_graph_create(3);
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 2.0);

    sssp_result_t* result = sssp_solve(graph, 0);

    sssp_status_t status = sssp_result_export(result, "test_result.txt");
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Export result to file");

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    /* Clean up */
    remove("test_result.txt");
}

/**
 * Test undirected edge addition
 */
void test_undirected_edges() {
    TEST_START("Undirected Edge Addition");

    sssp_graph_t* graph = sssp_graph_create(3);
    sssp_status_t status = sssp_graph_add_undirected_edge(graph, 0, 1, 5.0);
    TEST_ASSERT_EQ(status, SSSP_SUCCESS, "Add undirected edge");
    TEST_ASSERT_EQ(sssp_graph_get_num_edges(graph), 2, "Undirected edge adds two directed edges");

    sssp_result_t* result1 = sssp_solve(graph, 0);
    TEST_ASSERT_NEAR(sssp_result_get_distance(result1, 1), 5.0, 1e-9, "Forward direction works");

    sssp_result_t* result2 = sssp_solve(graph, 1);
    TEST_ASSERT_NEAR(sssp_result_get_distance(result2, 0), 5.0, 1e-9, "Backward direction works");

    sssp_result_destroy(result1);
    sssp_result_destroy(result2);
    sssp_graph_destroy(graph);
}

/**
 * Test utility functions
 */
void test_utility_functions() {
    TEST_START("Utility Functions");

    const char* version = sssp_get_version();
    TEST_ASSERT(version != NULL, "Get version string");
    TEST_ASSERT(strlen(version) > 0, "Version string is non-empty");

    const char* info = sssp_get_info();
    TEST_ASSERT(info != NULL, "Get info string");
    TEST_ASSERT(strlen(info) > 0, "Info string is non-empty");

    const char* msg = sssp_status_to_string(SSSP_SUCCESS);
    TEST_ASSERT(strcmp(msg, "Success") == 0, "Status to string for SUCCESS");

    msg = sssp_status_to_string(SSSP_ERROR_NULL_POINTER);
    TEST_ASSERT(strstr(msg, "Null") != NULL || strstr(msg, "NULL") != NULL,
                "Status to string for NULL_POINTER");
}

/**
 * Test error handling
 */
void test_error_handling() {
    TEST_START("Error Handling");

    /* Test NULL pointers */
    TEST_ASSERT_EQ(sssp_result_get_status(NULL), SSSP_ERROR_NULL_POINTER, "NULL result returns error");
    TEST_ASSERT_EQ(sssp_result_get_distance(NULL, 0), SSSP_INFINITY, "NULL result distance is infinity");
    TEST_ASSERT_EQ(sssp_result_get_predecessor(NULL, 0), SSSP_INVALID_VERTEX, "NULL result predecessor is invalid");
    TEST_ASSERT(!sssp_result_is_reachable(NULL, 0), "NULL result is not reachable");

    /* Test invalid solve */
    sssp_result_t* result = sssp_solve(NULL, 0);
    TEST_ASSERT(result == NULL, "Solve with NULL graph returns NULL");

    sssp_graph_t* graph = sssp_graph_create(5);
    result = sssp_solve(graph, -1);
    TEST_ASSERT(result == NULL, "Solve with invalid source returns NULL");

    result = sssp_solve(graph, 100);
    TEST_ASSERT(result == NULL, "Solve with out-of-range source returns NULL");

    sssp_graph_destroy(graph);
}

/* ============================================================================
 * MAIN TEST RUNNER
 * ============================================================================ */

int main() {
    printf("=================================================================\n");
    printf("   SSSP PUBLIC API - COMPREHENSIVE TEST SUITE\n");
    printf("=================================================================\n");

    printf("\nVersion: %s\n", sssp_get_version());

    test_graph_creation();
    test_basic_sssp();
    test_path_reconstruction();
    test_unreachable_vertices();
    test_negative_cycle_detection();
    test_graph_io();
    test_result_export();
    test_undirected_edges();
    test_utility_functions();
    test_error_handling();

    printf("\n=================================================================\n");
    printf("   TEST SUMMARY\n");
    printf("=================================================================\n");
    printf(COLOR_GREEN "  Passed: %d" COLOR_RESET "\n", tests_passed);
    if (tests_failed > 0) {
        printf(COLOR_RED "  Failed: %d" COLOR_RESET "\n", tests_failed);
    } else {
        printf("  Failed: 0\n");
    }
    printf("  Total:  %d\n", tests_passed + tests_failed);

    if (tests_failed == 0) {
        printf("\n" COLOR_GREEN "  ✓ ALL TESTS PASSED!" COLOR_RESET "\n");
    } else {
        printf("\n" COLOR_RED "  ✗ SOME TESTS FAILED" COLOR_RESET "\n");
    }

    printf("=================================================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
