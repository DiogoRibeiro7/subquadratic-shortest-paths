/**
 * @file compare_dijkstra.c
 * @brief Comprehensive comparison between Breakthrough and Dijkstra algorithms
 *
 * Features:
 * 1. Multiple Dijkstra implementations (binary heap, Fibonacci heap)
 * 2. Side-by-side comparison on identical graphs
 * 3. Statistical analysis with confidence intervals
 * 4. CSV output for plotting
 * 5. Theoretical vs actual performance analysis
 *
 * Usage:
 *   ./compare_dijkstra [options]
 *
 * Output:
 *   - comparison_results.csv: Raw timing data
 *   - comparison_summary.txt: Statistical analysis
 *   - theoretical_analysis.txt: Complexity comparison
 */

#include "../src/sssp_breakthrough.h"
#include "../src/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

/*============================================================================
 * Configuration
 *===========================================================================*/

#define DEFAULT_MIN_N 100
#define DEFAULT_MAX_N 5000
#define DEFAULT_TRIALS 20
#define CONFIDENCE_LEVEL 0.95
#define Z_SCORE_95 1.96  /* For 95% confidence interval */

/*============================================================================
 * Fibonacci Heap Implementation
 *===========================================================================*/

typedef struct FibNode {
    int vertex;
    double key;
    int degree;
    bool marked;
    struct FibNode* parent;
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
} FibNode;

typedef struct {
    FibNode* min;
    int n;
    FibNode** vertex_map;  /* Map from vertex to node */
} FibHeap;

/**
 * @brief Create Fibonacci heap
 */
FibHeap* fib_heap_create(int capacity) {
    FibHeap* heap = (FibHeap*)malloc(sizeof(FibHeap));
    if (!heap) return NULL;

    heap->min = NULL;
    heap->n = 0;
    heap->vertex_map = (FibNode**)calloc(capacity, sizeof(FibNode*));
    if (!heap->vertex_map) {
        free(heap);
        return NULL;
    }

    return heap;
}

/**
 * @brief Create new Fibonacci heap node
 */
FibNode* fib_node_create(int vertex, double key) {
    FibNode* node = (FibNode*)malloc(sizeof(FibNode));
    if (!node) return NULL;

    node->vertex = vertex;
    node->key = key;
    node->degree = 0;
    node->marked = false;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;

    return node;
}

/**
 * @brief Link two nodes (make y child of x)
 */
void fib_heap_link(FibHeap* heap, FibNode* y, FibNode* x) {
    /* Remove y from root list */
    y->left->right = y->right;
    y->right->left = y->left;

    /* Make y child of x */
    y->parent = x;

    if (x->child == NULL) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }

    x->degree++;
    y->marked = false;
}

/**
 * @brief Consolidate heap after extract-min
 */
void fib_heap_consolidate(FibHeap* heap) {
    if (!heap->min) return;

    int max_degree = (int)(log((double)heap->n) / log(2.0)) + 1;
    FibNode** degree_array = (FibNode**)calloc(max_degree + 1, sizeof(FibNode*));

    /* Create list of root nodes */
    int num_roots = 0;
    FibNode* start = heap->min;
    FibNode* current = start;
    FibNode* roots[1000];  /* Temporary array */

    do {
        roots[num_roots++] = current;
        current = current->right;
    } while (current != start && num_roots < 1000);

    /* Consolidate */
    for (int i = 0; i < num_roots; i++) {
        FibNode* x = roots[i];
        int d = x->degree;

        while (degree_array[d] != NULL) {
            FibNode* y = degree_array[d];

            if (x->key > y->key) {
                FibNode* temp = x;
                x = y;
                y = temp;
            }

            fib_heap_link(heap, y, x);
            degree_array[d] = NULL;
            d++;
        }

        degree_array[d] = x;
    }

    /* Rebuild root list and find new min */
    heap->min = NULL;
    for (int i = 0; i <= max_degree; i++) {
        if (degree_array[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = degree_array[i];
                heap->min->left = heap->min;
                heap->min->right = heap->min;
            } else {
                /* Add to root list */
                degree_array[i]->left = heap->min;
                degree_array[i]->right = heap->min->right;
                heap->min->right->left = degree_array[i];
                heap->min->right = degree_array[i];

                if (degree_array[i]->key < heap->min->key) {
                    heap->min = degree_array[i];
                }
            }
        }
    }

    free(degree_array);
}

/**
 * @brief Insert node into Fibonacci heap
 */
void fib_heap_insert(FibHeap* heap, int vertex, double key) {
    FibNode* node = fib_node_create(vertex, key);
    if (!node) return;

    heap->vertex_map[vertex] = node;

    if (heap->min == NULL) {
        heap->min = node;
    } else {
        /* Add to root list */
        node->left = heap->min;
        node->right = heap->min->right;
        heap->min->right->left = node;
        heap->min->right = node;

        if (key < heap->min->key) {
            heap->min = node;
        }
    }

    heap->n++;
}

/**
 * @brief Extract minimum from Fibonacci heap
 */
FibNode* fib_heap_extract_min(FibHeap* heap) {
    FibNode* z = heap->min;
    if (z == NULL) return NULL;

    /* Add children to root list */
    if (z->child != NULL) {
        FibNode* child = z->child;
        FibNode* start = child;

        do {
            FibNode* next = child->right;
            child->left = heap->min;
            child->right = heap->min->right;
            heap->min->right->left = child;
            heap->min->right = child;
            child->parent = NULL;
            child = next;
        } while (child != start);
    }

    /* Remove z from root list */
    z->left->right = z->right;
    z->right->left = z->left;

    if (z == z->right) {
        heap->min = NULL;
    } else {
        heap->min = z->right;
        fib_heap_consolidate(heap);
    }

    heap->n--;
    heap->vertex_map[z->vertex] = NULL;

    return z;
}

/**
 * @brief Cut node from parent in Fibonacci heap
 */
void fib_heap_cut(FibHeap* heap, FibNode* x, FibNode* y) {
    /* Remove x from child list of y */
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }

    y->degree--;

    /* Add x to root list */
    x->left = heap->min;
    x->right = heap->min->right;
    heap->min->right->left = x;
    heap->min->right = x;

    x->parent = NULL;
    x->marked = false;
}

/**
 * @brief Cascading cut in Fibonacci heap
 */
void fib_heap_cascading_cut(FibHeap* heap, FibNode* y) {
    FibNode* z = y->parent;

    if (z != NULL) {
        if (!y->marked) {
            y->marked = true;
        } else {
            fib_heap_cut(heap, y, z);
            fib_heap_cascading_cut(heap, z);
        }
    }
}

/**
 * @brief Decrease key in Fibonacci heap
 */
void fib_heap_decrease_key(FibHeap* heap, int vertex, double new_key) {
    FibNode* x = heap->vertex_map[vertex];
    if (!x || new_key > x->key) return;

    x->key = new_key;
    FibNode* y = x->parent;

    if (y != NULL && x->key < y->key) {
        fib_heap_cut(heap, x, y);
        fib_heap_cascading_cut(heap, y);
    }

    if (x->key < heap->min->key) {
        heap->min = x;
    }
}

/**
 * @brief Free Fibonacci heap
 */
void fib_heap_free(FibHeap* heap) {
    if (!heap) return;

    /* Free all nodes (simple approach - free as we encounter them) */
    if (heap->min) {
        FibNode* start = heap->min;
        FibNode* current = start;

        do {
            FibNode* next = current->right;
            /* Note: This is simplified; a complete implementation would
             * recursively free children */
            free(current);
            current = next;
        } while (current != start);
    }

    free(heap->vertex_map);
    free(heap);
}

/*============================================================================
 * Dijkstra Implementations
 *===========================================================================*/

/**
 * @brief Dijkstra with binary heap (already in sssp_breakthrough.h)
 */
void dijkstra_binary_heap(Graph* g, int source) {
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

/**
 * @brief Dijkstra with Fibonacci heap
 */
void dijkstra_fibonacci_heap(Graph* g, int source) {
    int n = g->n;

    /* Initialize */
    for (int i = 0; i < n; i++) {
        g->dist[i] = INFINITY_DIST;
        g->complete[i] = false;
    }
    g->dist[source] = 0.0;

    FibHeap* heap = fib_heap_create(n);
    if (!heap) return;

    fib_heap_insert(heap, source, 0.0);

    while (heap->n > 0) {
        FibNode* min_node = fib_heap_extract_min(heap);
        if (!min_node) break;

        int u = min_node->vertex;
        free(min_node);

        if (g->complete[u]) continue;
        g->complete[u] = true;

        Edge* edge = g->adj[u];
        while (edge) {
            int v = edge->target;
            double new_dist = g->dist[u] + edge->weight;

            if (new_dist < g->dist[v]) {
                if (g->dist[v] == INFINITY_DIST) {
                    g->dist[v] = new_dist;
                    fib_heap_insert(heap, v, new_dist);
                } else {
                    g->dist[v] = new_dist;
                    fib_heap_decrease_key(heap, v, new_dist);
                }
            }

            edge = edge->next;
        }
    }

    fib_heap_free(heap);
}

/*============================================================================
 * Data Structures for Comparison
 *===========================================================================*/

typedef struct {
    int n;
    int m;
    double density;

    /* Timing results (multiple trials) */
    double* time_breakthrough;
    double* time_binary_heap;
    double* time_fibonacci_heap;

    int num_trials;

    /* Statistical measures */
    double breakthrough_mean;
    double breakthrough_stddev;
    double breakthrough_ci_low;
    double breakthrough_ci_high;

    double binary_mean;
    double binary_stddev;
    double binary_ci_low;
    double binary_ci_high;

    double fibonacci_mean;
    double fibonacci_stddev;
    double fibonacci_ci_low;
    double fibonacci_ci_high;

    /* Speedup metrics */
    double speedup_vs_binary;
    double speedup_vs_fibonacci;

    /* Theoretical complexity */
    double complexity_breakthrough;  /* m * log^(2/3) n */
    double complexity_binary;        /* (m + n) * log n */
    double complexity_fibonacci;     /* m + n * log n */

    /* Verification */
    bool results_match;
} ComparisonResult;

typedef struct {
    int min_n;
    int max_n;
    int num_trials;
    char graph_type[32];
    char output_dir[256];
    bool verbose;
} ComparisonConfig;

/*============================================================================
 * Statistical Functions
 *===========================================================================*/

double calc_mean(const double* data, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += data[i];
    }
    return sum / n;
}

double calc_stddev(const double* data, int n, double mean) {
    if (n <= 1) return 0.0;

    double sum_sq = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq += diff * diff;
    }
    return sqrt(sum_sq / (n - 1));
}

void calc_confidence_interval(double mean, double stddev, int n,
                              double* ci_low, double* ci_high) {
    double margin = Z_SCORE_95 * (stddev / sqrt(n));
    *ci_low = mean - margin;
    *ci_high = mean + margin;
}

/**
 * @brief Perform t-test to determine if two means are significantly different
 * @return t-statistic
 */
double perform_t_test(const double* data1, int n1,
                      const double* data2, int n2) {
    double mean1 = calc_mean(data1, n1);
    double mean2 = calc_mean(data2, n2);

    double var1 = 0.0, var2 = 0.0;
    for (int i = 0; i < n1; i++) {
        double diff = data1[i] - mean1;
        var1 += diff * diff;
    }
    var1 /= (n1 - 1);

    for (int i = 0; i < n2; i++) {
        double diff = data2[i] - mean2;
        var2 += diff * diff;
    }
    var2 /= (n2 - 1);

    double pooled_std = sqrt((var1 / n1) + (var2 / n2));
    if (pooled_std == 0.0) return 0.0;

    return (mean1 - mean2) / pooled_std;
}

/*============================================================================
 * Comparison Testing
 *===========================================================================*/

/**
 * @brief Verify that all three algorithms produce same results
 */
bool verify_results(const double* dist1, const double* dist2,
                   const double* dist3, int n) {
    const double tolerance = 1e-9;

    for (int i = 0; i < n; i++) {
        bool inf1 = (dist1[i] >= INFINITY_DIST / 2);
        bool inf2 = (dist2[i] >= INFINITY_DIST / 2);
        bool inf3 = (dist3[i] >= INFINITY_DIST / 2);

        if (inf1 && inf2 && inf3) continue;

        if (inf1 != inf2 || inf2 != inf3) return false;

        if (fabs(dist1[i] - dist2[i]) > tolerance) return false;
        if (fabs(dist2[i] - dist3[i]) > tolerance) return false;
    }

    return true;
}

/**
 * @brief Run comparison test for specific graph size
 */
ComparisonResult run_comparison_test(int n, const char* graph_type,
                                     int num_trials, bool verbose) {
    ComparisonResult result = {0};
    result.n = n;
    result.num_trials = num_trials;

    /* Allocate timing arrays */
    result.time_breakthrough = (double*)malloc(num_trials * sizeof(double));
    result.time_binary_heap = (double*)malloc(num_trials * sizeof(double));
    result.time_fibonacci_heap = (double*)malloc(num_trials * sizeof(double));

    if (verbose) {
        printf("  Testing n=%d", n);
        fflush(stdout);
    }

    /* Allocate distance arrays for verification */
    double* dist_breakthrough = (double*)malloc(n * sizeof(double));
    double* dist_binary = (double*)malloc(n * sizeof(double));
    double* dist_fibonacci = (double*)malloc(n * sizeof(double));

    for (int trial = 0; trial < num_trials; trial++) {
        /* Generate graph */
        Graph* g = NULL;
        if (strcmp(graph_type, "sparse") == 0) {
            g = graph_generate_sparse(n, 3, 1.0, 100.0);
        } else if (strcmp(graph_type, "medium") == 0) {
            g = graph_generate_sparse(n, 10, 1.0, 100.0);
        } else if (strcmp(graph_type, "dense") == 0) {
            int degree = (int)sqrt(n);
            g = graph_generate_sparse(n, degree, 1.0, 100.0);
        } else {
            g = graph_generate_sparse(n, 5, 1.0, 100.0);
        }

        if (!g) break;

        result.m = g->m;
        result.density = (double)g->m / ((double)n * (n - 1));

        /* Test breakthrough algorithm */
        Timer* timer = timer_create();
        timer_start(timer);
        sssp_breakthrough(g, 0);
        result.time_breakthrough[trial] = timer_stop(timer);
        timer_destroy(timer);

        memcpy(dist_breakthrough, g->dist, n * sizeof(double));

        /* Test binary heap Dijkstra */
        timer = timer_create();
        timer_start(timer);
        dijkstra_binary_heap(g, 0);
        result.time_binary_heap[trial] = timer_stop(timer);
        timer_destroy(timer);

        memcpy(dist_binary, g->dist, n * sizeof(double));

        /* Test Fibonacci heap Dijkstra */
        timer = timer_create();
        timer_start(timer);
        dijkstra_fibonacci_heap(g, 0);
        result.time_fibonacci_heap[trial] = timer_stop(timer);
        timer_destroy(timer);

        memcpy(dist_fibonacci, g->dist, n * sizeof(double));

        /* Verify on first trial */
        if (trial == 0) {
            result.results_match = verify_results(dist_breakthrough, dist_binary,
                                                  dist_fibonacci, n);
            if (!result.results_match && verbose) {
                printf(" [MISMATCH!]");
            }
        }

        free_graph(g);

        if (verbose) {
            printf(".");
            fflush(stdout);
        }
    }

    if (verbose) {
        printf(" Done\n");
    }

    /* Calculate statistics */
    result.breakthrough_mean = calc_mean(result.time_breakthrough, num_trials);
    result.breakthrough_stddev = calc_stddev(result.time_breakthrough, num_trials,
                                            result.breakthrough_mean);
    calc_confidence_interval(result.breakthrough_mean, result.breakthrough_stddev,
                           num_trials, &result.breakthrough_ci_low,
                           &result.breakthrough_ci_high);

    result.binary_mean = calc_mean(result.time_binary_heap, num_trials);
    result.binary_stddev = calc_stddev(result.time_binary_heap, num_trials,
                                       result.binary_mean);
    calc_confidence_interval(result.binary_mean, result.binary_stddev,
                           num_trials, &result.binary_ci_low,
                           &result.binary_ci_high);

    result.fibonacci_mean = calc_mean(result.time_fibonacci_heap, num_trials);
    result.fibonacci_stddev = calc_stddev(result.time_fibonacci_heap, num_trials,
                                         result.fibonacci_mean);
    calc_confidence_interval(result.fibonacci_mean, result.fibonacci_stddev,
                           num_trials, &result.fibonacci_ci_low,
                           &result.fibonacci_ci_high);

    /* Calculate speedups */
    result.speedup_vs_binary = result.binary_mean / result.breakthrough_mean;
    result.speedup_vs_fibonacci = result.fibonacci_mean / result.breakthrough_mean;

    /* Calculate theoretical complexities */
    double log_n = log((double)n);
    result.complexity_breakthrough = result.m * pow(log_n, 2.0/3.0);
    result.complexity_binary = (result.m + n) * log_n;
    result.complexity_fibonacci = result.m + n * log_n;

    free(dist_breakthrough);
    free(dist_binary);
    free(dist_fibonacci);

    return result;
}

/*============================================================================
 * Output Generation
 *===========================================================================*/

void write_csv_header_comparison(FILE* fp) {
    fprintf(fp, "n,m,density,");
    fprintf(fp, "breakthrough_mean,breakthrough_stddev,breakthrough_ci_low,breakthrough_ci_high,");
    fprintf(fp, "binary_mean,binary_stddev,binary_ci_low,binary_ci_high,");
    fprintf(fp, "fibonacci_mean,fibonacci_stddev,fibonacci_ci_low,fibonacci_ci_high,");
    fprintf(fp, "speedup_vs_binary,speedup_vs_fibonacci,");
    fprintf(fp, "complexity_breakthrough,complexity_binary,complexity_fibonacci,");
    fprintf(fp, "trials,results_match\n");
}

void write_csv_result(FILE* fp, const ComparisonResult* result) {
    fprintf(fp, "%d,%d,%.6e,", result->n, result->m, result->density);
    fprintf(fp, "%.6e,%.6e,%.6e,%.6e,",
            result->breakthrough_mean, result->breakthrough_stddev,
            result->breakthrough_ci_low, result->breakthrough_ci_high);
    fprintf(fp, "%.6e,%.6e,%.6e,%.6e,",
            result->binary_mean, result->binary_stddev,
            result->binary_ci_low, result->binary_ci_high);
    fprintf(fp, "%.6e,%.6e,%.6e,%.6e,",
            result->fibonacci_mean, result->fibonacci_stddev,
            result->fibonacci_ci_low, result->fibonacci_ci_high);
    fprintf(fp, "%.4f,%.4f,",
            result->speedup_vs_binary, result->speedup_vs_fibonacci);
    fprintf(fp, "%.6e,%.6e,%.6e,",
            result->complexity_breakthrough, result->complexity_binary,
            result->complexity_fibonacci);
    fprintf(fp, "%d,%s\n", result->num_trials, result->results_match ? "true" : "false");
}

void generate_comparison_summary(const char* filename, ComparisonResult* results,
                                int num_results, const ComparisonConfig* config) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot write to %s\n", filename);
        return;
    }

    fprintf(fp, "╔═══════════════════════════════════════════════════════════════╗\n");
    fprintf(fp, "║     SSSP Algorithm Comparison: Breakthrough vs Dijkstra      ║\n");
    fprintf(fp, "╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Configuration */
    fprintf(fp, "Configuration:\n");
    fprintf(fp, "  Graph type: %s\n", config->graph_type);
    fprintf(fp, "  Size range: %d to %d vertices\n", config->min_n, config->max_n);
    fprintf(fp, "  Trials per size: %d\n", config->num_trials);
    fprintf(fp, "  Confidence level: %.0f%%\n\n", CONFIDENCE_LEVEL * 100);

    /* Performance comparison table */
    fprintf(fp, "Performance Comparison:\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "%-6s %-8s | %-12s | %-12s | %-12s | %-8s\n",
            "n", "m", "Breakthrough", "Binary Heap", "Fibonacci", "Speedup");
    fprintf(fp, "       %-8s | %-12s | %-12s | %-12s | (vs Bin)\n",
            "", "(s)", "(s)", "(s)", "");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < num_results; i++) {
        fprintf(fp, "%-6d %-8d | %7.4f±%.4f | %7.4f±%.4f | %7.4f±%.4f | %7.2fx\n",
                results[i].n, results[i].m,
                results[i].breakthrough_mean, results[i].breakthrough_stddev,
                results[i].binary_mean, results[i].binary_stddev,
                results[i].fibonacci_mean, results[i].fibonacci_stddev,
                results[i].speedup_vs_binary);
    }
    fprintf(fp, "\n");

    /* Statistical analysis */
    fprintf(fp, "Statistical Analysis:\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");

    for (int i = 0; i < num_results; i++) {
        fprintf(fp, "\nn=%d, m=%d:\n", results[i].n, results[i].m);

        /* T-test between breakthrough and binary heap */
        double t_stat_binary = perform_t_test(results[i].time_breakthrough,
                                              results[i].num_trials,
                                              results[i].time_binary_heap,
                                              results[i].num_trials);

        fprintf(fp, "  Breakthrough vs Binary Heap:\n");
        fprintf(fp, "    Mean difference: %.6f s\n",
                results[i].binary_mean - results[i].breakthrough_mean);
        fprintf(fp, "    t-statistic: %.3f\n", t_stat_binary);
        fprintf(fp, "    Speedup: %.2fx (CI: [%.2f, %.2f])\n",
                results[i].speedup_vs_binary,
                results[i].binary_ci_low / results[i].breakthrough_ci_high,
                results[i].binary_ci_high / results[i].breakthrough_ci_low);

        if (fabs(t_stat_binary) > 2.0) {
            fprintf(fp, "    Result: Statistically significant difference (p < 0.05)\n");
        } else {
            fprintf(fp, "    Result: No significant difference\n");
        }

        /* T-test between breakthrough and Fibonacci heap */
        double t_stat_fib = perform_t_test(results[i].time_breakthrough,
                                          results[i].num_trials,
                                          results[i].time_fibonacci_heap,
                                          results[i].num_trials);

        fprintf(fp, "  Breakthrough vs Fibonacci Heap:\n");
        fprintf(fp, "    Mean difference: %.6f s\n",
                results[i].fibonacci_mean - results[i].breakthrough_mean);
        fprintf(fp, "    t-statistic: %.3f\n", t_stat_fib);
        fprintf(fp, "    Speedup: %.2fx\n", results[i].speedup_vs_fibonacci);
    }

    fprintf(fp, "\n");

    /* Theoretical vs actual analysis */
    fprintf(fp, "Theoretical vs Actual Performance:\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "%-6s | %-14s | %-14s | %-10s\n",
            "n", "Theoretical", "Actual", "Ratio");
    fprintf(fp, "       | m·log^(2/3)n  | Time (s)      | (Act/Theo)\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < num_results; i++) {
        double ratio = results[i].breakthrough_mean / results[i].complexity_breakthrough;
        fprintf(fp, "%-6d | %-14.2f | %-14.6f | %-10.6e\n",
                results[i].n,
                results[i].complexity_breakthrough,
                results[i].breakthrough_mean,
                ratio);
    }

    fprintf(fp, "\n");

    /* Crossover analysis */
    fprintf(fp, "Crossover Analysis:\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");

    int crossover_binary = -1;
    int crossover_fib = -1;

    for (int i = 0; i < num_results; i++) {
        if (crossover_binary < 0 && results[i].speedup_vs_binary > 1.0) {
            crossover_binary = i;
        }
        if (crossover_fib < 0 && results[i].speedup_vs_fibonacci > 1.0) {
            crossover_fib = i;
        }
    }

    if (crossover_binary >= 0) {
        fprintf(fp, "  Crossover vs Binary Heap: n=%d, m=%d\n",
                results[crossover_binary].n, results[crossover_binary].m);
        fprintf(fp, "    Breakthrough becomes %.2fx faster\n",
                results[crossover_binary].speedup_vs_binary);
    } else {
        fprintf(fp, "  No crossover found vs Binary Heap in tested range\n");
    }

    if (crossover_fib >= 0) {
        fprintf(fp, "  Crossover vs Fibonacci Heap: n=%d, m=%d\n",
                results[crossover_fib].n, results[crossover_fib].m);
        fprintf(fp, "    Breakthrough becomes %.2fx faster\n",
                results[crossover_fib].speedup_vs_fibonacci);
    } else {
        fprintf(fp, "  No crossover found vs Fibonacci Heap in tested range\n");
    }

    fprintf(fp, "\n");

    /* Summary conclusion */
    fprintf(fp, "Summary:\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");

    double avg_speedup_binary = 0.0;
    double avg_speedup_fib = 0.0;
    for (int i = 0; i < num_results; i++) {
        avg_speedup_binary += results[i].speedup_vs_binary;
        avg_speedup_fib += results[i].speedup_vs_fibonacci;
    }
    avg_speedup_binary /= num_results;
    avg_speedup_fib /= num_results;

    fprintf(fp, "  Average speedup vs Binary Heap: %.2fx\n", avg_speedup_binary);
    fprintf(fp, "  Average speedup vs Fibonacci Heap: %.2fx\n", avg_speedup_fib);

    if (avg_speedup_binary > 1.2) {
        fprintf(fp, "\n  ✓ Breakthrough algorithm shows significant improvement\n");
    } else if (avg_speedup_binary > 0.8) {
        fprintf(fp, "\n  ~ Breakthrough algorithm shows competitive performance\n");
    } else {
        fprintf(fp, "\n  ✗ Breakthrough algorithm slower in this test range\n");
    }

    fprintf(fp, "\n═══════════════════════════════════════════════════════════════\n");

    fclose(fp);
}

/*============================================================================
 * Main Comparison Suite
 *===========================================================================*/

int run_comparison_suite(const ComparisonConfig* config) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SSSP Algorithm Comparison Suite                          ║\n");
    printf("║     Breakthrough vs Dijkstra (Binary & Fibonacci Heap)       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Generate test sizes */
    int sizes[50];
    int num_sizes = 0;

    for (int n = config->min_n; n <= config->max_n && num_sizes < 50; n = (int)(n * 1.5)) {
        sizes[num_sizes++] = n;
    }

    printf("Configuration:\n");
    printf("  Graph type: %s\n", config->graph_type);
    printf("  Test points: %d (from %d to %d vertices)\n",
           num_sizes, sizes[0], sizes[num_sizes-1]);
    printf("  Trials per point: %d\n", config->num_trials);
    printf("\n");

    /* Allocate results */
    ComparisonResult* results = (ComparisonResult*)malloc(
        num_sizes * sizeof(ComparisonResult));

    /* Run tests */
    printf("Running comparison tests...\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    for (int i = 0; i < num_sizes; i++) {
        results[i] = run_comparison_test(sizes[i], config->graph_type,
                                        config->num_trials, config->verbose);
    }

    printf("═══════════════════════════════════════════════════════════════\n");
    printf("All tests completed\n\n");

    /* Write CSV */
    char csv_path[512];
    snprintf(csv_path, sizeof(csv_path), "%s/comparison_results.csv", config->output_dir);

    FILE* csv_file = fopen(csv_path, "w");
    if (csv_file) {
        write_csv_header_comparison(csv_file);
        for (int i = 0; i < num_sizes; i++) {
            write_csv_result(csv_file, &results[i]);
        }
        fclose(csv_file);
        printf("Results written to: %s\n", csv_path);
    }

    /* Generate summary */
    char summary_path[512];
    snprintf(summary_path, sizeof(summary_path), "%s/comparison_summary.txt",
             config->output_dir);
    generate_comparison_summary(summary_path, results, num_sizes, config);
    printf("Summary written to: %s\n", summary_path);

    /* Free memory */
    for (int i = 0; i < num_sizes; i++) {
        free(results[i].time_breakthrough);
        free(results[i].time_binary_heap);
        free(results[i].time_fibonacci_heap);
    }
    free(results);

    return 0;
}

/*============================================================================
 * Command Line Parsing
 *===========================================================================*/

void print_usage(const char* prog) {
    printf("Usage: %s [options]\n\n", prog);
    printf("Options:\n");
    printf("  --min-n <size>        Minimum graph size (default: %d)\n", DEFAULT_MIN_N);
    printf("  --max-n <size>        Maximum graph size (default: %d)\n", DEFAULT_MAX_N);
    printf("  --trials <count>      Number of trials (default: %d)\n", DEFAULT_TRIALS);
    printf("  --graph-type <type>   sparse, medium, or dense (default: sparse)\n");
    printf("  --output-dir <dir>    Output directory (default: current)\n");
    printf("  --verbose             Verbose output\n");
    printf("  --help                Show this help\n");
    printf("\n");
}

ComparisonConfig parse_args(int argc, char** argv) {
    ComparisonConfig config;
    config.min_n = DEFAULT_MIN_N;
    config.max_n = DEFAULT_MAX_N;
    config.num_trials = DEFAULT_TRIALS;
    strcpy(config.graph_type, "sparse");
    strcpy(config.output_dir, ".");
    config.verbose = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--min-n") == 0 && i + 1 < argc) {
            config.min_n = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--max-n") == 0 && i + 1 < argc) {
            config.max_n = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--trials") == 0 && i + 1 < argc) {
            config.num_trials = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--graph-type") == 0 && i + 1 < argc) {
            strncpy(config.graph_type, argv[++i], sizeof(config.graph_type) - 1);
        } else if (strcmp(argv[i], "--output-dir") == 0 && i + 1 < argc) {
            strncpy(config.output_dir, argv[++i], sizeof(config.output_dir) - 1);
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
 * Main
 *===========================================================================*/

int main(int argc, char** argv) {
    ComparisonConfig config = parse_args(argc, argv);

    /* Set random seed */
    utils_set_random_seed(42);

    /* Run comparison */
    int result = run_comparison_suite(&config);

    if (result == 0) {
        printf("\n✓ Comparison completed successfully\n\n");
    } else {
        printf("\n✗ Comparison failed\n\n");
    }

    return result;
}
