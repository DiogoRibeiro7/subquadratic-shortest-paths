/**
 * @file performance_test.c
 * @brief Comprehensive performance testing for SSSP Breakthrough Algorithm
 *
 * This program:
 * - Generates graphs of various sizes (1K to 100K vertices)
 * - Times both Dijkstra and SSSP Breakthrough algorithms
 * - Outputs performance comparison tables
 * - Creates CSV data suitable for plotting
 * - Analyzes memory usage patterns
 * - Tests different graph types (sparse, dense, grid, random)
 *
 * Compile and run:
 *   mkdir build && cd build
 *   cmake ..
 *   make
 *   ./bin/examples/performance_test
 *
 * Output files:
 *   performance_results.csv - Data for plotting
 *   performance_results.txt - Detailed results
 */

#include "sssp_breakthrough.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*============================================================================
 * Configuration
 *============================================================================*/

// Test configuration
typedef struct {
    int n;                    // Number of vertices
    int m;                    // Number of edges (or -1 for auto)
    const char* graph_type;   // "sparse", "medium", "dense", "grid"
    int num_trials;           // Number of trials to average
} TestConfig;

// Performance results for one test
typedef struct {
    int n;                       // Number of vertices
    int m;                       // Number of edges
    const char* graph_type;      // Graph type
    double time_breakthrough;    // Breakthrough time (seconds)
    double time_dijkstra;        // Dijkstra time (seconds)
    double speedup;              // Dijkstra / Breakthrough
    size_t memory_used;          // Peak memory usage (bytes)
    bool results_match;          // Do results match?
    int trials_run;              // Number of trials completed
} TestResult;

/*============================================================================
 * Dijkstra Implementation (for comparison)
 *============================================================================*/

/**
 * @brief Standard Dijkstra's algorithm using min-heap
 */
void dijkstra(Graph* graph, int source) {
    if (!graph || source < 0 || source >= graph->n) {
        return;
    }

    // Initialize
    for (int i = 0; i < graph->n; i++) {
        graph->dist[i] = INFINITY_DIST;
        graph->pred[i] = -1;
        graph->complete[i] = false;
    }
    graph->dist[source] = 0.0;

    // Create min-heap
    MinHeap* heap = heap_create(graph->n);
    if (!heap) {
        return;
    }

    heap_insert(heap, source, 0.0);

    // Main loop
    while (heap->size > 0) {
        int u = heap_extract_min(heap);

        if (graph->complete[u]) {
            continue;
        }

        graph->complete[u] = true;

        // Relax edges
        Edge* edge = graph->adj[u];
        while (edge) {
            int v = edge->target;
            double new_dist = graph->dist[u] + edge->weight;

            if (new_dist < graph->dist[v]) {
                graph->dist[v] = new_dist;
                graph->pred[v] = u;

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
 * Graph Generation
 *============================================================================*/

/**
 * @brief Generate test graph based on configuration
 */
Graph* generate_test_graph(const TestConfig* config) {
    Graph* graph = NULL;

    if (strcmp(config->graph_type, "sparse") == 0) {
        // Sparse: avg degree ~3
        int avg_degree = 3;
        int m = (config->n * avg_degree) / 2;
        GraphGenParams params = graph_gen_params_default(config->n, m);
        params.min_weight = 1.0;
        params.max_weight = 100.0;
        graph = graph_generate_random(&params);

    } else if (strcmp(config->graph_type, "medium") == 0) {
        // Medium: avg degree ~10
        int avg_degree = 10;
        int m = (config->n * avg_degree) / 2;
        GraphGenParams params = graph_gen_params_default(config->n, m);
        params.min_weight = 1.0;
        params.max_weight = 100.0;
        graph = graph_generate_random(&params);

    } else if (strcmp(config->graph_type, "dense") == 0) {
        // Dense: avg degree ~50
        int avg_degree = 50;
        int m = (config->n * avg_degree) / 2;
        GraphGenParams params = graph_gen_params_default(config->n, m);
        params.min_weight = 1.0;
        params.max_weight = 100.0;
        graph = graph_generate_random(&params);

    } else if (strcmp(config->graph_type, "grid") == 0) {
        // Grid graph
        int side = (int)sqrt(config->n);
        graph = graph_generate_grid(side, side, 1.0, 100.0, false);

    } else if (strcmp(config->graph_type, "dag") == 0) {
        // DAG with moderate density
        int m = config->n * 3;
        graph = graph_generate_dag(config->n, m, 1.0, 100.0);

    } else {
        // Default: use specified m or sparse
        int m = (config->m > 0) ? config->m : config->n * 3;
        GraphGenParams params = graph_gen_params_default(config->n, m);
        params.min_weight = 1.0;
        params.max_weight = 100.0;
        graph = graph_generate_random(&params);
    }

    return graph;
}

/*============================================================================
 * Performance Testing
 *============================================================================*/

/**
 * @brief Verify that two distance arrays match
 */
bool verify_distances_match(const double* dist1, const double* dist2, int n) {
    const double EPSILON = 1e-9;

    for (int i = 0; i < n; i++) {
        // Check if both are infinity
        if ((dist1[i] == INFINITY_DIST) != (dist2[i] == INFINITY_DIST)) {
            return false;
        }

        // If both finite, check difference
        if (dist1[i] != INFINITY_DIST) {
            double diff = fabs(dist1[i] - dist2[i]);
            if (diff > EPSILON) {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Run performance test for a single configuration
 */
TestResult run_performance_test(const TestConfig* config) {
    TestResult result = {0};
    result.n = config->n;
    result.graph_type = config->graph_type;
    result.trials_run = 0;

    printf("Testing: n=%d, type=%s, trials=%d\n",
           config->n, config->graph_type, config->num_trials);

    // Accumulate timing over multiple trials
    double total_time_breakthrough = 0.0;
    double total_time_dijkstra = 0.0;
    size_t peak_memory = 0;
    bool all_match = true;

    for (int trial = 0; trial < config->num_trials; trial++) {
        // Generate test graph
        Graph* graph = generate_test_graph(config);
        if (!graph) {
            fprintf(stderr, "  Failed to generate graph (trial %d)\n", trial + 1);
            continue;
        }

        result.m = graph->m;  // Record actual edge count

        // Reset memory stats
        memory_reset_stats();

        // Test 1: SSSP Breakthrough
        Timer* timer = timer_create();
        timer_start(timer);

        sssp_breakthrough(graph, 0);

        double time_breakthrough = timer_stop(timer);
        total_time_breakthrough += time_breakthrough;

        // Save breakthrough results
        double* dist_breakthrough = (double*)malloc(graph->n * sizeof(double));
        if (!dist_breakthrough) {
            fprintf(stderr, "  Memory allocation failed\n");
            timer_destroy(timer);
            free_graph(graph);
            continue;
        }
        memcpy(dist_breakthrough, graph->dist, graph->n * sizeof(double));

        // Get memory usage
        size_t mem_allocated, mem_peak;
        memory_get_stats(&mem_allocated, &mem_peak);
        if (mem_peak > peak_memory) {
            peak_memory = mem_peak;
        }

        // Test 2: Dijkstra
        timer_start(timer);

        dijkstra(graph, 0);

        double time_dijkstra = timer_stop(timer);
        total_time_dijkstra += time_dijkstra;

        // Verify results match
        bool match = verify_distances_match(dist_breakthrough, graph->dist, graph->n);
        if (!match) {
            all_match = false;
            fprintf(stderr, "  WARNING: Results don't match (trial %d)\n", trial + 1);
        }

        // Cleanup
        free(dist_breakthrough);
        timer_destroy(timer);
        free_graph(graph);

        result.trials_run++;

        // Progress indicator
        if ((trial + 1) % 5 == 0 || trial == config->num_trials - 1) {
            printf("  Progress: %d/%d trials completed\r",
                   trial + 1, config->num_trials);
            fflush(stdout);
        }
    }

    printf("\n");

    // Compute averages
    if (result.trials_run > 0) {
        result.time_breakthrough = total_time_breakthrough / result.trials_run;
        result.time_dijkstra = total_time_dijkstra / result.trials_run;
        result.speedup = result.time_dijkstra / result.time_breakthrough;
        result.memory_used = peak_memory;
        result.results_match = all_match;
    }

    return result;
}

/*============================================================================
 * Output and Reporting
 *============================================================================*/

/**
 * @brief Print table header
 */
void print_table_header(void) {
    printf("\n");
    printf("┌────────┬─────────┬──────────┬──────────────┬──────────────┬──────────┬─────────┬────────┐\n");
    printf("│   n    │    m    │   Type   │ Breakthrough │   Dijkstra   │  Speedup │ Memory  │ Match? │\n");
    printf("├────────┼─────────┼──────────┼──────────────┼──────────────┼──────────┼─────────┼────────┤\n");
}

/**
 * @brief Print table footer
 */
void print_table_footer(void) {
    printf("└────────┴─────────┴──────────┴──────────────┴──────────────┴──────────┴─────────┴────────┘\n");
    printf("\n");
}

/**
 * @brief Print a single result row
 */
void print_result_row(const TestResult* result) {
    char time_breakthrough[32], time_dijkstra[32];
    format_time(result->time_breakthrough, time_breakthrough, sizeof(time_breakthrough));
    format_time(result->time_dijkstra, time_dijkstra, sizeof(time_dijkstra));

    printf("│ %6d │ %7d │ %-8s │ %12s │ %12s │ %7.2fx │ %6.1fM │   %s   │\n",
           result->n,
           result->m,
           result->graph_type,
           time_breakthrough,
           time_dijkstra,
           result->speedup,
           result->memory_used / (1024.0 * 1024.0),
           result->results_match ? "✓" : "✗");
}

/**
 * @brief Save results to CSV file for plotting
 */
void save_results_csv(const TestResult* results, int num_results, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", filename);
        return;
    }

    // Write header
    fprintf(fp, "n,m,graph_type,time_breakthrough,time_dijkstra,speedup,memory_mb,match\n");

    // Write data
    for (int i = 0; i < num_results; i++) {
        fprintf(fp, "%d,%d,%s,%.9f,%.9f,%.6f,%.3f,%d\n",
                results[i].n,
                results[i].m,
                results[i].graph_type,
                results[i].time_breakthrough,
                results[i].time_dijkstra,
                results[i].speedup,
                results[i].memory_used / (1024.0 * 1024.0),
                results[i].results_match ? 1 : 0);
    }

    fclose(fp);
    printf("Results saved to: %s\n", filename);
}

/**
 * @brief Save detailed results to text file
 */
void save_results_text(const TestResult* results, int num_results, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", filename);
        return;
    }

    fprintf(fp, "SSSP Breakthrough Algorithm - Performance Test Results\n");
    fprintf(fp, "======================================================\n\n");

    fprintf(fp, "Test Date: %s\n", __DATE__);
    fprintf(fp, "Number of Tests: %d\n\n", num_results);

    for (int i = 0; i < num_results; i++) {
        fprintf(fp, "Test %d:\n", i + 1);
        fprintf(fp, "  Graph: %d vertices, %d edges (%s)\n",
                results[i].n, results[i].m, results[i].graph_type);
        fprintf(fp, "  Breakthrough: %.6f seconds\n", results[i].time_breakthrough);
        fprintf(fp, "  Dijkstra:     %.6f seconds\n", results[i].time_dijkstra);
        fprintf(fp, "  Speedup:      %.2fx\n", results[i].speedup);
        fprintf(fp, "  Memory:       %.2f MB\n", results[i].memory_used / (1024.0 * 1024.0));
        fprintf(fp, "  Results match: %s\n", results[i].results_match ? "Yes" : "No");
        fprintf(fp, "  Trials run:    %d\n\n", results[i].trials_run);
    }

    fclose(fp);
    printf("Detailed results saved to: %s\n", filename);
}

/**
 * @brief Print performance summary statistics
 */
void print_summary(const TestResult* results, int num_results) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Performance Summary\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    double avg_speedup = 0.0;
    double max_speedup = 0.0;
    double min_speedup = 999999.0;
    int breakthrough_wins = 0;
    int dijkstra_wins = 0;

    for (int i = 0; i < num_results; i++) {
        avg_speedup += results[i].speedup;

        if (results[i].speedup > max_speedup) {
            max_speedup = results[i].speedup;
        }
        if (results[i].speedup < min_speedup) {
            min_speedup = results[i].speedup;
        }

        if (results[i].speedup > 1.0) {
            breakthrough_wins++;
        } else {
            dijkstra_wins++;
        }
    }

    avg_speedup /= num_results;

    printf("Speedup Statistics:\n");
    printf("  Average speedup:  %.2fx\n", avg_speedup);
    printf("  Maximum speedup:  %.2fx\n", max_speedup);
    printf("  Minimum speedup:  %.2fx\n", min_speedup);
    printf("\n");

    printf("Algorithm Performance:\n");
    printf("  Breakthrough faster: %d tests (%.1f%%)\n",
           breakthrough_wins, 100.0 * breakthrough_wins / num_results);
    printf("  Dijkstra faster:     %d tests (%.1f%%)\n",
           dijkstra_wins, 100.0 * dijkstra_wins / num_results);
    printf("\n");

    // Correctness check
    int all_correct = 1;
    for (int i = 0; i < num_results; i++) {
        if (!results[i].results_match) {
            all_correct = 0;
            break;
        }
    }

    printf("Correctness: %s\n",
           all_correct ? "All results match ✓" : "Some mismatches found ✗");
    printf("\n");
}

/*============================================================================
 * Main Testing Suite
 *============================================================================*/

int main(int argc, char** argv) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SSSP Breakthrough Algorithm - Performance Test Suite     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    // Initialize random seed
    utils_set_random_seed(42);  // For reproducibility

    // Define test configurations
    // Format: {n, m, graph_type, num_trials}
    TestConfig tests[] = {
        // Small graphs
        {100,    -1, "sparse",  10},
        {100,    -1, "medium",  10},
        {500,    -1, "sparse",  10},
        {500,    -1, "medium",  10},

        // Medium graphs
        {1000,   -1, "sparse",  5},
        {1000,   -1, "medium",  5},
        {2000,   -1, "sparse",  5},
        {5000,   -1, "sparse",  3},
        {5000,   -1, "medium",  3},

        // Large graphs
        {10000,  -1, "sparse",  3},
        {10000,  -1, "medium",  3},
        {20000,  -1, "sparse",  2},
        {50000,  -1, "sparse",  2},

        // Very large graphs (commented out by default - uncomment if needed)
        // {100000, -1, "sparse",  1},

        // Different graph types
        {1000,   -1, "grid",    5},
        {2000,   -1, "dag",     5},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);

    printf("Configuration:\n");
    printf("  Total tests: %d\n", num_tests);
    printf("  Graph sizes: 100 to 50,000 vertices\n");
    printf("  Graph types: sparse, medium, grid, dag\n");
    printf("\n");

    // Allow user to run quick test
    if (argc > 1 && strcmp(argv[1], "--quick") == 0) {
        printf("Running in QUICK mode (reduced test set)\n");
        num_tests = 6;  // Only first 6 tests
    }

    // Run all tests
    TestResult* results = (TestResult*)malloc(num_tests * sizeof(TestResult));
    if (!results) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    printf("Starting performance tests...\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    Timer* total_timer = timer_create();
    timer_start(total_timer);

    for (int i = 0; i < num_tests; i++) {
        printf("[Test %d/%d] ", i + 1, num_tests);
        results[i] = run_performance_test(&tests[i]);
    }

    double total_time = timer_stop(total_timer);

    printf("\n");
    printf("All tests completed in ");
    char time_str[64];
    format_time(total_time, time_str, sizeof(time_str));
    printf("%s\n", time_str);

    // Print results table
    print_table_header();
    for (int i = 0; i < num_tests; i++) {
        print_result_row(&results[i]);
    }
    print_table_footer();

    // Print summary
    print_summary(results, num_tests);

    // Save results
    save_results_csv(results, num_tests, "performance_results.csv");
    save_results_text(results, num_tests, "performance_results.txt");

    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Plotting Instructions:\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    printf("You can plot the results using Python:\n\n");
    printf("  import pandas as pd\n");
    printf("  import matplotlib.pyplot as plt\n");
    printf("  \n");
    printf("  df = pd.read_csv('performance_results.csv')\n");
    printf("  \n");
    printf("  # Plot speedup vs graph size\n");
    printf("  plt.figure(figsize=(10, 6))\n");
    printf("  for graph_type in df['graph_type'].unique():\n");
    printf("      data = df[df['graph_type'] == graph_type]\n");
    printf("      plt.plot(data['n'], data['speedup'], marker='o', label=graph_type)\n");
    printf("  \n");
    printf("  plt.xlabel('Graph Size (vertices)')\n");
    printf("  plt.ylabel('Speedup (Dijkstra / Breakthrough)')\n");
    printf("  plt.title('SSSP Algorithm Performance Comparison')\n");
    printf("  plt.legend()\n");
    printf("  plt.grid(True)\n");
    printf("  plt.axhline(y=1.0, color='r', linestyle='--', label='Equal Performance')\n");
    printf("  plt.savefig('performance_comparison.png')\n\n");

    printf("Or using gnuplot:\n\n");
    printf("  set terminal png size 1200,800\n");
    printf("  set output 'performance_comparison.png'\n");
    printf("  set xlabel 'Graph Size (vertices)'\n");
    printf("  set ylabel 'Speedup'\n");
    printf("  set title 'SSSP Algorithm Performance Comparison'\n");
    printf("  set grid\n");
    printf("  plot 'performance_results.csv' using 1:6 with linespoints title 'Speedup'\n\n");

    // Cleanup
    free(results);
    timer_destroy(total_timer);

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                  Performance Testing Complete                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    return 0;
}
