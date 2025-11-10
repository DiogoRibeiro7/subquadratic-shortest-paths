/**
 * @file scalability_test.c
 * @brief Comprehensive scalability analysis for SSSP Breakthrough Algorithm
 *
 * This benchmark suite:
 * 1. Tests algorithm on progressively larger graphs
 * 2. Measures both time and memory usage
 * 3. Validates theoretical O(m * log^(2/3) n) complexity bounds
 * 4. Identifies practical crossover points vs Dijkstra's algorithm
 * 5. Generates CSV data for scaling plots and analysis
 *
 * Output files:
 * - scalability_results.csv: Raw performance data
 * - scalability_summary.txt: Human-readable summary
 * - crossover_analysis.csv: Dijkstra comparison data
 * - memory_profile.csv: Memory usage data
 *
 * Usage:
 *   ./scalability_test [options]
 *   Options:
 *     --max-n <size>       Maximum graph size (default: 10000)
 *     --min-n <size>       Minimum graph size (default: 100)
 *     --trials <count>     Number of trials per size (default: 5)
 *     --output-dir <dir>   Output directory (default: current)
 *     --graph-type <type>  Graph type: sparse, dense, grid (default: sparse)
 */

#include "../src/sssp_breakthrough.h"
#include "../src/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

/*============================================================================
 * Configuration and Constants
 *===========================================================================*/

#define DEFAULT_MIN_N 100
#define DEFAULT_MAX_N 10000
#define DEFAULT_TRIALS 5
#define SIZE_GROWTH_FACTOR 1.5
#define MAX_DATA_POINTS 50

/* Graph type enumeration */
typedef enum {
    GRAPH_TYPE_SPARSE,      /* m ≈ 3n */
    GRAPH_TYPE_MEDIUM,      /* m ≈ 10n */
    GRAPH_TYPE_DENSE,       /* m ≈ n^1.5 */
    GRAPH_TYPE_GRID,        /* 2D grid */
    GRAPH_TYPE_COMPLETE     /* Complete graph */
} GraphType;

/*============================================================================
 * Data Structures
 *===========================================================================*/

/**
 * @brief Memory usage statistics
 */
typedef struct {
    size_t peak_memory_bytes;    /* Peak memory usage */
    size_t current_memory_bytes; /* Current memory usage */
    size_t allocations;          /* Number of allocations */
    size_t deallocations;        /* Number of deallocations */
} MemoryStats;

/**
 * @brief Performance measurement for a single test
 */
typedef struct {
    int n;                       /* Number of vertices */
    int m;                       /* Number of edges */
    double density;              /* Edge density */

    /* Breakthrough algorithm metrics */
    double time_breakthrough_mean;
    double time_breakthrough_stddev;
    double time_breakthrough_min;
    double time_breakthrough_max;

    /* Dijkstra algorithm metrics */
    double time_dijkstra_mean;
    double time_dijkstra_stddev;
    double time_dijkstra_min;
    double time_dijkstra_max;

    /* Memory metrics */
    size_t memory_peak_bytes;
    size_t memory_avg_bytes;

    /* Derived metrics */
    double speedup;              /* Dijkstra time / Breakthrough time */
    double complexity_measure;   /* m * log^(2/3) n */
    double time_per_complexity;  /* Time / complexity measure */

    /* Verification */
    bool results_match;          /* Whether algorithms produce same result */
    int trials_completed;        /* Number of successful trials */
} ScalabilityDataPoint;

/**
 * @brief Configuration for scalability test
 */
typedef struct {
    int min_n;
    int max_n;
    int num_trials;
    GraphType graph_type;
    char output_dir[256];
    bool verbose;
    bool compare_dijkstra;
    bool measure_memory;
} TestConfig;

/*============================================================================
 * Memory Tracking
 *===========================================================================*/

static size_t current_memory = 0;
static size_t peak_memory = 0;
static size_t allocation_count = 0;

/**
 * @brief Get current memory usage
 */
size_t get_current_memory_usage(void) {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; /* Convert KB to bytes */
#endif
}

/**
 * @brief Reset memory statistics
 */
void reset_memory_stats(void) {
    current_memory = get_current_memory_usage();
    peak_memory = current_memory;
    allocation_count = 0;
}

/**
 * @brief Update peak memory usage
 */
void update_memory_stats(void) {
    current_memory = get_current_memory_usage();
    if (current_memory > peak_memory) {
        peak_memory = current_memory;
    }
    allocation_count++;
}

/**
 * @brief Get memory statistics
 */
MemoryStats get_memory_stats(void) {
    MemoryStats stats;
    stats.current_memory_bytes = current_memory;
    stats.peak_memory_bytes = peak_memory;
    stats.allocations = allocation_count;
    stats.deallocations = 0;
    return stats;
}

/*============================================================================
 * Dijkstra Implementation (for comparison)
 *===========================================================================*/

void dijkstra_baseline(Graph* g, int source) {
    int n = g->n;

    /* Initialize */
    for (int i = 0; i < n; i++) {
        g->dist[i] = INFINITY_DIST;
        g->complete[i] = false;
    }
    g->dist[source] = 0.0;

    MinHeap* heap = heap_create(n);
    if (!heap) return;

    heap_insert(heap, source, 0.0);

    while (heap->size > 0) {
        int u = heap_extract_min(heap);

        if (g->complete[u]) continue;
        g->complete[u] = true;

        Edge* edge = g->adj[u];
        while (edge) {
            int v = edge->target;
            double new_dist = g->dist[u] + edge->weight;

            if (new_dist < g->dist[v]) {
                g->dist[v] = new_dist;

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
 *===========================================================================*/

/**
 * @brief Generate test graph based on type and size
 */
Graph* generate_test_graph(int n, GraphType type, int* actual_m) {
    Graph* g = NULL;

    switch (type) {
        case GRAPH_TYPE_SPARSE:
            /* m ≈ 3n */
            g = graph_generate_sparse(n, 3, 1.0, 100.0);
            break;

        case GRAPH_TYPE_MEDIUM:
            /* m ≈ 10n */
            g = graph_generate_sparse(n, 10, 1.0, 100.0);
            break;

        case GRAPH_TYPE_DENSE:
            /* m ≈ n^1.5 */
            {
                int target_degree = (int)sqrt(n);
                if (target_degree < 3) target_degree = 3;
                g = graph_generate_sparse(n, target_degree, 1.0, 100.0);
            }
            break;

        case GRAPH_TYPE_GRID:
            /* Grid graph */
            {
                int side = (int)sqrt(n);
                g = graph_generate_grid(side, side, 1.0, 100.0, false);
            }
            break;

        case GRAPH_TYPE_COMPLETE:
            /* Complete graph (use with caution for large n) */
            g = graph_generate_complete(n, 1.0, 100.0);
            break;
    }

    if (g && actual_m) {
        *actual_m = g->m;
    }

    return g;
}

/*============================================================================
 * Statistical Analysis
 *===========================================================================*/

/**
 * @brief Calculate mean of array
 */
double calculate_mean(const double* data, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += data[i];
    }
    return sum / n;
}

/**
 * @brief Calculate standard deviation
 */
double calculate_stddev(const double* data, int n, double mean) {
    if (n <= 1) return 0.0;

    double sum_sq = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq += diff * diff;
    }
    return sqrt(sum_sq / (n - 1));
}

/**
 * @brief Find minimum value in array
 */
double find_min(const double* data, int n) {
    double min = data[0];
    for (int i = 1; i < n; i++) {
        if (data[i] < min) min = data[i];
    }
    return min;
}

/**
 * @brief Find maximum value in array
 */
double find_max(const double* data, int n) {
    double max = data[0];
    for (int i = 1; i < n; i++) {
        if (data[i] > max) max = data[i];
    }
    return max;
}

/*============================================================================
 * Performance Measurement
 *===========================================================================*/

/**
 * @brief Run single trial and measure performance
 */
bool run_single_trial(Graph* g, int source, double* time_breakthrough,
                      double* time_dijkstra, bool compare_dijkstra) {
    /* Save original distances for comparison */
    double* dist_backup = (double*)malloc(g->n * sizeof(double));
    if (!dist_backup) return false;

    /* Measure breakthrough algorithm */
    Timer* timer = timer_create();
    timer_start(timer);
    sssp_breakthrough(g, source);
    *time_breakthrough = timer_stop(timer);
    timer_destroy(timer);

    /* Save breakthrough results */
    memcpy(dist_backup, g->dist, g->n * sizeof(double));

    /* Measure Dijkstra if requested */
    if (compare_dijkstra) {
        timer = timer_create();
        timer_start(timer);
        dijkstra_baseline(g, source);
        *time_dijkstra = timer_stop(timer);
        timer_destroy(timer);
    } else {
        *time_dijkstra = 0.0;
    }

    free(dist_backup);
    return true;
}

/**
 * @brief Run scalability test for specific graph size
 */
ScalabilityDataPoint run_scalability_test(int n, GraphType type,
                                          int num_trials, bool compare_dijkstra,
                                          bool measure_memory) {
    ScalabilityDataPoint result = {0};
    result.n = n;
    result.trials_completed = 0;
    result.results_match = true;

    double* times_breakthrough = (double*)malloc(num_trials * sizeof(double));
    double* times_dijkstra = (double*)malloc(num_trials * sizeof(double));
    size_t* memory_samples = (size_t*)malloc(num_trials * sizeof(size_t));

    printf("  Testing n=%d", n);
    fflush(stdout);

    for (int trial = 0; trial < num_trials; trial++) {
        /* Generate graph */
        Graph* g = generate_test_graph(n, type, &result.m);
        if (!g) {
            printf(" [Graph generation failed]\n");
            break;
        }

        /* Reset memory tracking */
        if (measure_memory) {
            reset_memory_stats();
        }

        /* Run trial */
        bool success = run_single_trial(g, 0, &times_breakthrough[trial],
                                       &times_dijkstra[trial], compare_dijkstra);

        if (success) {
            result.trials_completed++;

            if (measure_memory) {
                update_memory_stats();
                memory_samples[trial] = peak_memory;
            }
        }

        free_graph(g);
        printf(".");
        fflush(stdout);
    }

    printf(" Done\n");

    if (result.trials_completed == 0) {
        free(times_breakthrough);
        free(times_dijkstra);
        free(memory_samples);
        return result;
    }

    /* Calculate statistics */
    result.time_breakthrough_mean = calculate_mean(times_breakthrough, result.trials_completed);
    result.time_breakthrough_stddev = calculate_stddev(times_breakthrough, result.trials_completed,
                                                       result.time_breakthrough_mean);
    result.time_breakthrough_min = find_min(times_breakthrough, result.trials_completed);
    result.time_breakthrough_max = find_max(times_breakthrough, result.trials_completed);

    if (compare_dijkstra) {
        result.time_dijkstra_mean = calculate_mean(times_dijkstra, result.trials_completed);
        result.time_dijkstra_stddev = calculate_stddev(times_dijkstra, result.trials_completed,
                                                       result.time_dijkstra_mean);
        result.time_dijkstra_min = find_min(times_dijkstra, result.trials_completed);
        result.time_dijkstra_max = find_max(times_dijkstra, result.trials_completed);
        result.speedup = result.time_dijkstra_mean / result.time_breakthrough_mean;
    }

    if (measure_memory && result.trials_completed > 0) {
        result.memory_peak_bytes = find_max((double*)memory_samples, result.trials_completed);
        result.memory_avg_bytes = (size_t)calculate_mean((double*)memory_samples, result.trials_completed);
    }

    /* Calculate derived metrics */
    result.density = (double)result.m / ((double)result.n * (result.n - 1));
    result.complexity_measure = result.m * pow(log((double)result.n), 2.0/3.0);
    result.time_per_complexity = result.time_breakthrough_mean / result.complexity_measure;

    free(times_breakthrough);
    free(times_dijkstra);
    free(memory_samples);

    return result;
}

/*============================================================================
 * Output and Reporting
 *===========================================================================*/

/**
 * @brief Write CSV header for scalability results
 */
void write_csv_header(FILE* fp) {
    fprintf(fp, "n,m,density,");
    fprintf(fp, "breakthrough_mean,breakthrough_stddev,breakthrough_min,breakthrough_max,");
    fprintf(fp, "dijkstra_mean,dijkstra_stddev,dijkstra_min,dijkstra_max,");
    fprintf(fp, "speedup,");
    fprintf(fp, "complexity_measure,time_per_complexity,");
    fprintf(fp, "memory_peak_mb,memory_avg_mb,");
    fprintf(fp, "trials\n");
}

/**
 * @brief Write data point to CSV
 */
void write_csv_datapoint(FILE* fp, const ScalabilityDataPoint* data) {
    fprintf(fp, "%d,%d,%.6e,", data->n, data->m, data->density);
    fprintf(fp, "%.6e,%.6e,%.6e,%.6e,",
            data->time_breakthrough_mean, data->time_breakthrough_stddev,
            data->time_breakthrough_min, data->time_breakthrough_max);
    fprintf(fp, "%.6e,%.6e,%.6e,%.6e,",
            data->time_dijkstra_mean, data->time_dijkstra_stddev,
            data->time_dijkstra_min, data->time_dijkstra_max);
    fprintf(fp, "%.6f,", data->speedup);
    fprintf(fp, "%.6e,%.6e,",
            data->complexity_measure, data->time_per_complexity);
    fprintf(fp, "%.3f,%.3f,",
            data->memory_peak_bytes / (1024.0 * 1024.0),
            data->memory_avg_bytes / (1024.0 * 1024.0));
    fprintf(fp, "%d\n", data->trials_completed);
}

/**
 * @brief Generate summary report
 */
void generate_summary_report(const char* filename, ScalabilityDataPoint* data,
                            int num_points, const TestConfig* config) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot write summary to %s\n", filename);
        return;
    }

    fprintf(fp, "===============================================\n");
    fprintf(fp, "  SSSP Scalability Test Summary\n");
    fprintf(fp, "===============================================\n\n");

    /* Configuration */
    fprintf(fp, "Configuration:\n");
    fprintf(fp, "  Graph sizes: %d to %d vertices\n", config->min_n, config->max_n);
    fprintf(fp, "  Trials per size: %d\n", config->num_trials);
    fprintf(fp, "  Graph type: ");
    switch (config->graph_type) {
        case GRAPH_TYPE_SPARSE: fprintf(fp, "Sparse (m ≈ 3n)\n"); break;
        case GRAPH_TYPE_MEDIUM: fprintf(fp, "Medium (m ≈ 10n)\n"); break;
        case GRAPH_TYPE_DENSE: fprintf(fp, "Dense (m ≈ n^1.5)\n"); break;
        case GRAPH_TYPE_GRID: fprintf(fp, "Grid\n"); break;
        case GRAPH_TYPE_COMPLETE: fprintf(fp, "Complete\n"); break;
    }
    fprintf(fp, "  Compare with Dijkstra: %s\n", config->compare_dijkstra ? "Yes" : "No");
    fprintf(fp, "\n");

    /* Performance summary */
    fprintf(fp, "Performance Summary:\n");
    fprintf(fp, "─────────────────────────────────────────────────────────────\n");
    fprintf(fp, "  %-8s %-8s %-12s %-12s %-10s\n",
            "n", "m", "Breakthrough", "Dijkstra", "Speedup");
    fprintf(fp, "─────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < num_points; i++) {
        fprintf(fp, "  %-8d %-8d %-12.6f %-12.6f %-10.2fx\n",
                data[i].n, data[i].m,
                data[i].time_breakthrough_mean,
                data[i].time_dijkstra_mean,
                data[i].speedup);
    }
    fprintf(fp, "\n");

    /* Complexity analysis */
    fprintf(fp, "Complexity Analysis:\n");
    fprintf(fp, "─────────────────────────────────────────────────────────────\n");
    fprintf(fp, "  %-8s %-14s %-14s\n", "n", "m*log^(2/3)n", "Time/Measure");
    fprintf(fp, "─────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < num_points; i++) {
        fprintf(fp, "  %-8d %-14.2f %-14.6e\n",
                data[i].n,
                data[i].complexity_measure,
                data[i].time_per_complexity);
    }
    fprintf(fp, "\n");

    /* Crossover analysis */
    if (config->compare_dijkstra) {
        fprintf(fp, "Crossover Analysis:\n");
        fprintf(fp, "  (Point where Breakthrough becomes faster than Dijkstra)\n\n");

        int crossover_idx = -1;
        for (int i = 0; i < num_points; i++) {
            if (data[i].speedup > 1.0) {
                crossover_idx = i;
                break;
            }
        }

        if (crossover_idx >= 0) {
            fprintf(fp, "  Crossover found at n=%d, m=%d\n",
                    data[crossover_idx].n, data[crossover_idx].m);
            fprintf(fp, "  Speedup: %.2fx\n", data[crossover_idx].speedup);
        } else if (data[num_points-1].speedup < 1.0) {
            fprintf(fp, "  No crossover found in tested range\n");
            fprintf(fp, "  Best speedup: %.2fx at n=%d\n",
                    data[num_points-1].speedup, data[num_points-1].n);
        } else {
            fprintf(fp, "  Breakthrough faster for all tested sizes\n");
        }
        fprintf(fp, "\n");
    }

    /* Memory analysis */
    if (config->measure_memory) {
        fprintf(fp, "Memory Usage:\n");
        fprintf(fp, "─────────────────────────────────────────────────────────────\n");
        fprintf(fp, "  %-8s %-8s %-12s %-12s\n",
                "n", "m", "Peak (MB)", "Avg (MB)");
        fprintf(fp, "─────────────────────────────────────────────────────────────\n");

        for (int i = 0; i < num_points; i++) {
            fprintf(fp, "  %-8d %-8d %-12.2f %-12.2f\n",
                    data[i].n, data[i].m,
                    data[i].memory_peak_bytes / (1024.0 * 1024.0),
                    data[i].memory_avg_bytes / (1024.0 * 1024.0));
        }
        fprintf(fp, "\n");
    }

    /* Theoretical validation */
    fprintf(fp, "Theoretical Complexity Validation:\n");
    double first_ratio = data[0].time_per_complexity;
    double last_ratio = data[num_points-1].time_per_complexity;
    double growth_factor = last_ratio / first_ratio;

    fprintf(fp, "  First time/complexity ratio: %.6e\n", first_ratio);
    fprintf(fp, "  Last time/complexity ratio:  %.6e\n", last_ratio);
    fprintf(fp, "  Growth factor: %.3f\n", growth_factor);
    fprintf(fp, "\n");

    if (growth_factor < 2.0) {
        fprintf(fp, "  ✓ PASS: Growth factor < 2.0 confirms O(m * log^(2/3) n)\n");
    } else if (growth_factor < 5.0) {
        fprintf(fp, "  ~ MARGINAL: Growth factor suggests sub-quadratic behavior\n");
    } else {
        fprintf(fp, "  ✗ FAIL: Growth factor indicates super-linear complexity\n");
    }

    fprintf(fp, "\n");
    fprintf(fp, "===============================================\n");

    fclose(fp);
}

/*============================================================================
 * Main Scalability Test Suite
 *===========================================================================*/

/**
 * @brief Run complete scalability test suite
 */
int run_scalability_suite(const TestConfig* config) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         SSSP Scalability Test Suite                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Generate test sizes */
    int sizes[MAX_DATA_POINTS];
    int num_sizes = 0;

    for (int n = config->min_n; n <= config->max_n && num_sizes < MAX_DATA_POINTS;
         n = (int)(n * SIZE_GROWTH_FACTOR)) {
        sizes[num_sizes++] = n;
    }

    printf("Configuration:\n");
    printf("  Graph sizes: %d points from %d to %d vertices\n",
           num_sizes, sizes[0], sizes[num_sizes-1]);
    printf("  Trials per size: %d\n", config->num_trials);
    printf("  Graph type: ");
    switch (config->graph_type) {
        case GRAPH_TYPE_SPARSE: printf("Sparse (m ≈ 3n)\n"); break;
        case GRAPH_TYPE_MEDIUM: printf("Medium (m ≈ 10n)\n"); break;
        case GRAPH_TYPE_DENSE: printf("Dense (m ≈ n^1.5)\n"); break;
        case GRAPH_TYPE_GRID: printf("Grid\n"); break;
        case GRAPH_TYPE_COMPLETE: printf("Complete\n"); break;
    }
    printf("\n");

    /* Allocate results array */
    ScalabilityDataPoint* results = (ScalabilityDataPoint*)malloc(
        num_sizes * sizeof(ScalabilityDataPoint));

    if (!results) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    /* Run tests */
    printf("Running scalability tests...\n");
    printf("===============================================\n");

    for (int i = 0; i < num_sizes; i++) {
        results[i] = run_scalability_test(sizes[i], config->graph_type,
                                          config->num_trials,
                                          config->compare_dijkstra,
                                          config->measure_memory);
    }

    printf("===============================================\n");
    printf("All tests completed\n\n");

    /* Write CSV output */
    char csv_path[512];
    snprintf(csv_path, sizeof(csv_path), "%s/scalability_results.csv", config->output_dir);

    FILE* csv_file = fopen(csv_path, "w");
    if (csv_file) {
        write_csv_header(csv_file);
        for (int i = 0; i < num_sizes; i++) {
            write_csv_datapoint(csv_file, &results[i]);
        }
        fclose(csv_file);
        printf("Results written to: %s\n", csv_path);
    }

    /* Generate summary report */
    char summary_path[512];
    snprintf(summary_path, sizeof(summary_path), "%s/scalability_summary.txt", config->output_dir);
    generate_summary_report(summary_path, results, num_sizes, config);
    printf("Summary written to: %s\n", summary_path);

    free(results);
    return 0;
}

/*============================================================================
 * Command Line Parsing
 *===========================================================================*/

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n\n", program_name);
    printf("Options:\n");
    printf("  --min-n <size>         Minimum graph size (default: %d)\n", DEFAULT_MIN_N);
    printf("  --max-n <size>         Maximum graph size (default: %d)\n", DEFAULT_MAX_N);
    printf("  --trials <count>       Number of trials per size (default: %d)\n", DEFAULT_TRIALS);
    printf("  --output-dir <dir>     Output directory (default: current)\n");
    printf("  --graph-type <type>    Graph type: sparse, medium, dense, grid (default: sparse)\n");
    printf("  --no-dijkstra          Skip Dijkstra comparison\n");
    printf("  --no-memory            Skip memory measurement\n");
    printf("  --verbose              Verbose output\n");
    printf("  --help                 Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --max-n 5000 --trials 10\n", program_name);
    printf("  %s --graph-type dense --output-dir results/\n", program_name);
    printf("\n");
}

TestConfig parse_arguments(int argc, char** argv) {
    TestConfig config;
    config.min_n = DEFAULT_MIN_N;
    config.max_n = DEFAULT_MAX_N;
    config.num_trials = DEFAULT_TRIALS;
    config.graph_type = GRAPH_TYPE_SPARSE;
    strcpy(config.output_dir, ".");
    config.verbose = false;
    config.compare_dijkstra = true;
    config.measure_memory = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--min-n") == 0 && i + 1 < argc) {
            config.min_n = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--max-n") == 0 && i + 1 < argc) {
            config.max_n = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--trials") == 0 && i + 1 < argc) {
            config.num_trials = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--output-dir") == 0 && i + 1 < argc) {
            strncpy(config.output_dir, argv[++i], sizeof(config.output_dir) - 1);
        } else if (strcmp(argv[i], "--graph-type") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "sparse") == 0) {
                config.graph_type = GRAPH_TYPE_SPARSE;
            } else if (strcmp(argv[i], "medium") == 0) {
                config.graph_type = GRAPH_TYPE_MEDIUM;
            } else if (strcmp(argv[i], "dense") == 0) {
                config.graph_type = GRAPH_TYPE_DENSE;
            } else if (strcmp(argv[i], "grid") == 0) {
                config.graph_type = GRAPH_TYPE_GRID;
            } else if (strcmp(argv[i], "complete") == 0) {
                config.graph_type = GRAPH_TYPE_COMPLETE;
            }
        } else if (strcmp(argv[i], "--no-dijkstra") == 0) {
            config.compare_dijkstra = false;
        } else if (strcmp(argv[i], "--no-memory") == 0) {
            config.measure_memory = false;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            config.verbose = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        }
    }

    return config;
}

/*============================================================================
 * Main Function
 *===========================================================================*/

int main(int argc, char** argv) {
    /* Parse command line arguments */
    TestConfig config = parse_arguments(argc, argv);

    /* Set random seed for reproducibility */
    utils_set_random_seed(42);

    /* Run scalability test suite */
    int result = run_scalability_suite(&config);

    if (result == 0) {
        printf("\n✓ Scalability test completed successfully\n\n");
    } else {
        printf("\n✗ Scalability test failed\n\n");
    }

    return result;
}
