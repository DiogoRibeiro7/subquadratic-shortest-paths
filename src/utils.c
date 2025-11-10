/**
 * @file utils.c
 * @brief Implementation of utility functions for SSSP breakthrough algorithm
 */

#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

/* Platform-specific includes for timing */
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

/*============================================================================
 * Global variables for memory tracking
 *============================================================================*/

static size_t total_allocated = 0;
static size_t peak_allocated = 0;
static unsigned int random_seed_state = 0;

/*============================================================================
 * Graph File I/O Implementation
 *============================================================================*/

Graph* graph_read_edge_list(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Error: NULL filename\n");
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    int n, m;
    if (fscanf(fp, "%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Invalid edge list format (expected: n m)\n");
        fclose(fp);
        return NULL;
    }

    if (n <= 0 || n > MAX_VERTICES || m < 0 || m > MAX_EDGES) {
        fprintf(stderr, "Error: Invalid graph size (n=%d, m=%d)\n", n, m);
        fclose(fp);
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        fclose(fp);
        return NULL;
    }

    // Read edges
    for (int i = 0; i < m; i++) {
        int u, v;
        double weight;

        if (fscanf(fp, "%d %d %lf", &u, &v, &weight) != 3) {
            fprintf(stderr, "Error: Invalid edge format at line %d\n", i + 2);
            free_graph(graph);
            fclose(fp);
            return NULL;
        }

        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Error: Invalid vertex ID at line %d (u=%d, v=%d, n=%d)\n",
                    i + 2, u, v, n);
            free_graph(graph);
            fclose(fp);
            return NULL;
        }

        add_edge(graph, u, v, weight);
    }

    fclose(fp);
    return graph;
}

bool graph_write_edge_list(const Graph* graph, const char* filename) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot write to file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    // Write header: n m
    fprintf(fp, "%d %d\n", graph->n, graph->m);

    // Write edges
    for (int u = 0; u < graph->n; u++) {
        Edge* edge = graph->adj[u];
        while (edge) {
            fprintf(fp, "%d %d %.10f\n", u, edge->target, edge->weight);
            edge = edge->next;
        }
    }

    fclose(fp);
    return true;
}

Graph* graph_read_dimacs(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Error: NULL filename\n");
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    Graph* graph = NULL;
    int n = 0, m = 0;
    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
        char type;
        if (sscanf(line, " %c", &type) != 1) {
            continue;
        }

        switch (type) {
            case 'c': // Comment
                continue;

            case 'p': { // Problem line: p sp n m
                char format[32];
                if (sscanf(line, "p %s %d %d", format, &n, &m) != 3) {
                    fprintf(stderr, "Error: Invalid DIMACS problem line\n");
                    fclose(fp);
                    return NULL;
                }

                if (strcmp(format, "sp") != 0 && strcmp(format, "shortest") != 0) {
                    fprintf(stderr, "Warning: Expected 'sp' format, got '%s'\n", format);
                }

                graph = create_graph(n);
                if (!graph) {
                    fclose(fp);
                    return NULL;
                }
                break;
            }

            case 'a': { // Arc: a u v weight
                if (!graph) {
                    fprintf(stderr, "Error: Arc before problem line\n");
                    fclose(fp);
                    return NULL;
                }

                int u, v;
                double weight;
                if (sscanf(line, "a %d %d %lf", &u, &v, &weight) != 3) {
                    fprintf(stderr, "Error: Invalid arc format\n");
                    free_graph(graph);
                    fclose(fp);
                    return NULL;
                }

                // DIMACS uses 1-based indexing
                u--;
                v--;

                if (u < 0 || u >= n || v < 0 || v >= n) {
                    fprintf(stderr, "Error: Invalid vertex ID (u=%d, v=%d)\n", u, v);
                    free_graph(graph);
                    fclose(fp);
                    return NULL;
                }

                add_edge(graph, u, v, weight);
                break;
            }

            default:
                // Ignore unknown line types
                break;
        }
    }

    fclose(fp);

    if (!graph) {
        fprintf(stderr, "Error: No problem line found in DIMACS file\n");
        return NULL;
    }

    return graph;
}

bool graph_write_dimacs(const Graph* graph, const char* filename, int source) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot write to file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    // Write comments
    fprintf(fp, "c DIMACS shortest path format\n");
    fprintf(fp, "c Generated by SSSP Breakthrough\n");

    // Write problem line (DIMACS uses 1-based indexing)
    fprintf(fp, "p sp %d %d\n", graph->n, graph->m);

    // Write source
    fprintf(fp, "s %d\n", source + 1);

    // Write arcs
    for (int u = 0; u < graph->n; u++) {
        Edge* edge = graph->adj[u];
        while (edge) {
            fprintf(fp, "a %d %d %.0f\n", u + 1, edge->target + 1, edge->weight);
            edge = edge->next;
        }
    }

    fclose(fp);
    return true;
}

bool graph_write_dot(const Graph* graph, const char* filename, const double* distances) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot write to file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "  rankdir=LR;\n");
    fprintf(fp, "  node [shape=circle];\n\n");

    // Write vertices with colors based on distances
    for (int v = 0; v < graph->n; v++) {
        if (distances) {
            if (distances[v] == INFINITY_DIST) {
                fprintf(fp, "  %d [label=\"%d\\n∞\", style=filled, fillcolor=lightgray];\n", v, v);
            } else {
                fprintf(fp, "  %d [label=\"%d\\n%.2f\", style=filled, fillcolor=lightblue];\n",
                        v, v, distances[v]);
            }
        } else {
            fprintf(fp, "  %d;\n", v);
        }
    }

    fprintf(fp, "\n");

    // Write edges
    for (int u = 0; u < graph->n; u++) {
        Edge* edge = graph->adj[u];
        while (edge) {
            fprintf(fp, "  %d -> %d [label=\"%.2f\"];\n", u, edge->target, edge->weight);
            edge = edge->next;
        }
    }

    fprintf(fp, "}\n");
    fclose(fp);
    return true;
}

Graph* graph_read_file(const char* filename, GraphFileFormat format, int* n) {
    Graph* graph = NULL;

    switch (format) {
        case GRAPH_FORMAT_EDGE_LIST:
            graph = graph_read_edge_list(filename);
            break;

        case GRAPH_FORMAT_DIMACS:
            graph = graph_read_dimacs(filename);
            break;

        default:
            fprintf(stderr, "Error: Unsupported graph file format\n");
            return NULL;
    }

    if (graph && n) {
        *n = graph->n;
    }

    return graph;
}

bool graph_write_file(const Graph* graph, const char* filename, GraphFileFormat format) {
    switch (format) {
        case GRAPH_FORMAT_EDGE_LIST:
            return graph_write_edge_list(graph, filename);

        case GRAPH_FORMAT_DIMACS:
            return graph_write_dimacs(graph, filename, 0);

        case GRAPH_FORMAT_DOT:
            return graph_write_dot(graph, filename, NULL);

        default:
            fprintf(stderr, "Error: Unsupported graph file format\n");
            return false;
    }
}

/*============================================================================
 * Memory Management Implementation
 *============================================================================*/

void* safe_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Error: malloc(%zu) failed\n", size);
        return NULL;
    }

    total_allocated += size;
    if (total_allocated > peak_allocated) {
        peak_allocated = total_allocated;
    }

    return ptr;
}

void* safe_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        return NULL;
    }

    void* ptr = calloc(nmemb, size);
    if (!ptr) {
        fprintf(stderr, "Error: calloc(%zu, %zu) failed\n", nmemb, size);
        return NULL;
    }

    size_t total_size = nmemb * size;
    total_allocated += total_size;
    if (total_allocated > peak_allocated) {
        peak_allocated = total_allocated;
    }

    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "Error: realloc(%p, %zu) failed\n", ptr, size);
        return NULL;
    }

    return new_ptr;
}

void safe_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void memory_get_stats(size_t* allocated, size_t* peak) {
    if (allocated) {
        *allocated = total_allocated;
    }
    if (peak) {
        *peak = peak_allocated;
    }
}

void memory_reset_stats(void) {
    total_allocated = 0;
    peak_allocated = 0;
}

void memory_print_stats(void) {
    fprintf(stderr, "Memory Statistics:\n");
    fprintf(stderr, "  Current allocated: %zu bytes (%.2f MB)\n",
            total_allocated, total_allocated / (1024.0 * 1024.0));
    fprintf(stderr, "  Peak allocated: %zu bytes (%.2f MB)\n",
            peak_allocated, peak_allocated / (1024.0 * 1024.0));
}

/*============================================================================
 * Performance Timing Implementation
 *============================================================================*/

Timer* timer_create(void) {
    Timer* timer = (Timer*)safe_malloc(sizeof(Timer));
    if (!timer) {
        return NULL;
    }

    timer->elapsed_seconds = 0.0;
    timer->running = false;
    timer_start(timer);

    return timer;
}

void timer_start(Timer* timer) {
    if (!timer) {
        return;
    }

#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    timer->start_time.tv_sec = counter.QuadPart / freq.QuadPart;
    timer->start_time.tv_nsec = ((counter.QuadPart % freq.QuadPart) * 1000000000) / freq.QuadPart;
#else
    clock_gettime(CLOCK_MONOTONIC, &timer->start_time);
#endif

    timer->running = true;
}

double timer_stop(Timer* timer) {
    if (!timer || !timer->running) {
        return 0.0;
    }

#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    timer->end_time.tv_sec = counter.QuadPart / freq.QuadPart;
    timer->end_time.tv_nsec = ((counter.QuadPart % freq.QuadPart) * 1000000000) / freq.QuadPart;
#else
    clock_gettime(CLOCK_MONOTONIC, &timer->end_time);
#endif

    timer->elapsed_seconds = (timer->end_time.tv_sec - timer->start_time.tv_sec) +
                             (timer->end_time.tv_nsec - timer->start_time.tv_nsec) / 1e9;
    timer->running = false;

    return timer->elapsed_seconds;
}

double timer_elapsed(const Timer* timer) {
    if (!timer) {
        return 0.0;
    }

    if (!timer->running) {
        return timer->elapsed_seconds;
    }

    struct timespec current_time;

#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    current_time.tv_sec = counter.QuadPart / freq.QuadPart;
    current_time.tv_nsec = ((counter.QuadPart % freq.QuadPart) * 1000000000) / freq.QuadPart;
#else
    clock_gettime(CLOCK_MONOTONIC, &current_time);
#endif

    return (current_time.tv_sec - timer->start_time.tv_sec) +
           (current_time.tv_nsec - timer->start_time.tv_nsec) / 1e9;
}

void timer_destroy(Timer* timer) {
    safe_free(timer);
}

long long get_timestamp_us(void) {
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000) / freq.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
#endif
}

void format_time(double seconds, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return;
    }

    if (seconds < 1e-6) {
        snprintf(buffer, buffer_size, "%.3f ns", seconds * 1e9);
    } else if (seconds < 1e-3) {
        snprintf(buffer, buffer_size, "%.3f μs", seconds * 1e6);
    } else if (seconds < 1.0) {
        snprintf(buffer, buffer_size, "%.3f ms", seconds * 1e3);
    } else if (seconds < 60.0) {
        snprintf(buffer, buffer_size, "%.3f s", seconds);
    } else if (seconds < 3600.0) {
        int minutes = (int)(seconds / 60);
        double secs = seconds - minutes * 60;
        snprintf(buffer, buffer_size, "%dm %.3fs", minutes, secs);
    } else {
        int hours = (int)(seconds / 3600);
        int minutes = (int)((seconds - hours * 3600) / 60);
        double secs = seconds - hours * 3600 - minutes * 60;
        snprintf(buffer, buffer_size, "%dh %dm %.3fs", hours, minutes, secs);
    }
}

/*============================================================================
 * Graph Validation Implementation
 *============================================================================*/

GraphProperties graph_get_properties(const Graph* graph) {
    GraphProperties props;
    memset(&props, 0, sizeof(GraphProperties));

    if (!graph) {
        return props;
    }

    props.num_vertices = graph->n;
    props.num_edges = graph->m;

    // Calculate weight statistics and degree info
    props.min_weight = DBL_MAX;
    props.max_weight = -DBL_MAX;
    props.max_degree = 0;
    props.has_negative_weights = false;

    int total_degree = 0;

    for (int u = 0; u < graph->n; u++) {
        int degree = 0;
        Edge* edge = graph->adj[u];

        while (edge) {
            degree++;
            total_degree++;

            if (edge->weight < props.min_weight) {
                props.min_weight = edge->weight;
            }
            if (edge->weight > props.max_weight) {
                props.max_weight = edge->weight;
            }
            if (edge->weight < 0) {
                props.has_negative_weights = true;
            }

            edge = edge->next;
        }

        if (degree > props.max_degree) {
            props.max_degree = degree;
        }
    }

    props.avg_degree = (graph->n > 0) ? (double)total_degree / graph->n : 0.0;

    // Check connectivity (simplified - checks weak connectivity)
    props.is_connected = graph_is_connected(graph);

    // Check for DAG
    props.is_dag = graph_is_dag(graph);

    // Check for negative cycles (expensive, so only if needed)
    if (props.has_negative_weights) {
        props.has_negative_cycle = graph_has_negative_cycle(graph, 0);
    }

    return props;
}

void graph_print_properties(const GraphProperties* props) {
    if (!props) {
        return;
    }

    printf("Graph Properties:\n");
    printf("  Vertices: %d\n", props->num_vertices);
    printf("  Edges: %d\n", props->num_edges);
    printf("  Average degree: %.2f\n", props->avg_degree);
    printf("  Maximum degree: %d\n", props->max_degree);
    printf("  Weight range: [%.3f, %.3f]\n", props->min_weight, props->max_weight);
    printf("  Has negative weights: %s\n", props->has_negative_weights ? "yes" : "no");
    printf("  Has negative cycle: %s\n", props->has_negative_cycle ? "yes" : "no");
    printf("  Is connected: %s\n", props->is_connected ? "yes" : "no");
    printf("  Is DAG: %s\n", props->is_dag ? "yes" : "no");
}

bool graph_has_negative_cycle(const Graph* graph, int source) {
    if (!graph || source < 0 || source >= graph->n) {
        return false;
    }

    // Bellman-Ford algorithm
    double* dist = (double*)safe_calloc(graph->n, sizeof(double));
    if (!dist) {
        return false;
    }

    // Initialize distances
    for (int i = 0; i < graph->n; i++) {
        dist[i] = INFINITY_DIST;
    }
    dist[source] = 0.0;

    // Relax edges V-1 times
    for (int i = 0; i < graph->n - 1; i++) {
        bool changed = false;
        for (int u = 0; u < graph->n; u++) {
            if (dist[u] == INFINITY_DIST) {
                continue;
            }

            Edge* edge = graph->adj[u];
            while (edge) {
                int v = edge->target;
                double new_dist = dist[u] + edge->weight;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    changed = true;
                }
                edge = edge->next;
            }
        }

        if (!changed) {
            break;
        }
    }

    // Check for negative cycle
    bool has_cycle = false;
    for (int u = 0; u < graph->n; u++) {
        if (dist[u] == INFINITY_DIST) {
            continue;
        }

        Edge* edge = graph->adj[u];
        while (edge) {
            int v = edge->target;
            if (dist[u] + edge->weight < dist[v]) {
                has_cycle = true;
                break;
            }
            edge = edge->next;
        }

        if (has_cycle) {
            break;
        }
    }

    safe_free(dist);
    return has_cycle;
}

bool graph_is_connected(const Graph* graph) {
    if (!graph || graph->n == 0) {
        return false;
    }

    // BFS to check connectivity (treats as undirected)
    bool* visited = (bool*)safe_calloc(graph->n, sizeof(bool));
    if (!visited) {
        return false;
    }

    int* queue = (int*)safe_malloc(graph->n * sizeof(int));
    if (!queue) {
        safe_free(visited);
        return false;
    }

    int head = 0, tail = 0;
    queue[tail++] = 0;
    visited[0] = true;
    int count = 1;

    while (head < tail) {
        int u = queue[head++];

        Edge* edge = graph->adj[u];
        while (edge) {
            int v = edge->target;
            if (!visited[v]) {
                visited[v] = true;
                queue[tail++] = v;
                count++;
            }
            edge = edge->next;
        }
    }

    safe_free(visited);
    safe_free(queue);

    return count == graph->n;
}

bool graph_is_dag(const Graph* graph) {
    if (!graph) {
        return false;
    }

    // Use DFS to detect cycles
    enum { WHITE, GRAY, BLACK };
    int* color = (int*)safe_calloc(graph->n, sizeof(int));
    if (!color) {
        return false;
    }

    bool is_dag = true;

    // DFS helper (iterative to avoid stack overflow)
    for (int start = 0; start < graph->n && is_dag; start++) {
        if (color[start] != WHITE) {
            continue;
        }

        // Use stack for DFS
        int* stack = (int*)safe_malloc(graph->n * sizeof(int));
        int* parent = (int*)safe_malloc(graph->n * sizeof(int));
        int top = 0;

        stack[top] = start;
        parent[top] = -1;
        top++;

        while (top > 0 && is_dag) {
            int u = stack[top - 1];

            if (color[u] == WHITE) {
                color[u] = GRAY;
                Edge* edge = graph->adj[u];
                while (edge) {
                    int v = edge->target;
                    if (color[v] == GRAY) {
                        // Back edge found - cycle detected
                        is_dag = false;
                        break;
                    }
                    if (color[v] == WHITE) {
                        stack[top] = v;
                        parent[top] = u;
                        top++;
                    }
                    edge = edge->next;
                }
            } else {
                color[u] = BLACK;
                top--;
            }
        }

        safe_free(stack);
        safe_free(parent);
    }

    safe_free(color);
    return is_dag;
}

bool graph_validate(const Graph* graph) {
    if (!graph) {
        fprintf(stderr, "Validation error: NULL graph\n");
        return false;
    }

    if (graph->n <= 0 || graph->n > MAX_VERTICES) {
        fprintf(stderr, "Validation error: Invalid vertex count %d\n", graph->n);
        return false;
    }

    if (graph->m < 0 || graph->m > MAX_EDGES) {
        fprintf(stderr, "Validation error: Invalid edge count %d\n", graph->m);
        return false;
    }

    if (!graph->adj) {
        fprintf(stderr, "Validation error: NULL adjacency list\n");
        return false;
    }

    // Count edges and check validity
    int edge_count = 0;
    for (int u = 0; u < graph->n; u++) {
        Edge* edge = graph->adj[u];
        while (edge) {
            edge_count++;

            if (edge->target < 0 || edge->target >= graph->n) {
                fprintf(stderr, "Validation error: Invalid edge target %d from %d\n",
                        edge->target, u);
                return false;
            }

            edge = edge->next;
        }
    }

    if (edge_count != graph->m) {
        fprintf(stderr, "Validation error: Edge count mismatch (stored: %d, counted: %d)\n",
                graph->m, edge_count);
        return false;
    }

    return true;
}

/*============================================================================
 * Random Graph Generators Implementation
 *============================================================================*/

GraphGenParams graph_gen_params_default(int n, int m) {
    GraphGenParams params;
    params.n = n;
    params.m = m;
    params.p = (n > 1) ? (double)m / (n * (n - 1)) : 0.0;
    params.min_weight = 1.0;
    params.max_weight = 100.0;
    params.allow_negative = false;
    params.directed = true;
    params.seed = 0;
    return params;
}

Graph* graph_generate_random(const GraphGenParams* params) {
    if (!params || params->n <= 0 || params->n > MAX_VERTICES) {
        return NULL;
    }

    if (params->seed == 0) {
        srand((unsigned int)time(NULL));
    } else {
        srand(params->seed);
    }

    Graph* graph = create_graph(params->n);
    if (!graph) {
        return NULL;
    }

    int target_edges = (params->m > 0) ? params->m : 0;
    int edges_added = 0;

    if (params->p > 0.0) {
        // Erdős-Rényi model with probability p
        for (int u = 0; u < params->n; u++) {
            for (int v = 0; v < params->n; v++) {
                if (u == v && !params->directed) {
                    continue;
                }

                if (random_double(0.0, 1.0) < params->p) {
                    double weight = random_double(params->min_weight, params->max_weight);
                    if (params->allow_negative && random_double(0.0, 1.0) < 0.2) {
                        weight = -weight;
                    }
                    add_edge(graph, u, v, weight);
                    edges_added++;
                }
            }
        }
    } else {
        // Add exactly m random edges
        while (edges_added < target_edges) {
            int u = random_int(0, params->n - 1);
            int v = random_int(0, params->n - 1);

            if (u == v) {
                continue;
            }

            double weight = random_double(params->min_weight, params->max_weight);
            if (params->allow_negative && random_double(0.0, 1.0) < 0.2) {
                weight = -weight;
            }

            add_edge(graph, u, v, weight);
            edges_added++;

            if (!params->directed) {
                add_edge(graph, v, u, weight);
                edges_added++;
            }
        }
    }

    return graph;
}

Graph* graph_generate_complete(int n, double min_weight, double max_weight) {
    if (n <= 0 || n > MAX_VERTICES) {
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (u != v) {
                double weight = random_double(min_weight, max_weight);
                add_edge(graph, u, v, weight);
            }
        }
    }

    return graph;
}

Graph* graph_generate_grid(int rows, int cols, double min_weight,
                           double max_weight, bool diagonal) {
    int n = rows * cols;
    if (n <= 0 || n > MAX_VERTICES) {
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;

            // Right neighbor
            if (c + 1 < cols) {
                int v = r * cols + (c + 1);
                double weight = random_double(min_weight, max_weight);
                add_edge(graph, u, v, weight);
                add_edge(graph, v, u, weight);
            }

            // Down neighbor
            if (r + 1 < rows) {
                int v = (r + 1) * cols + c;
                double weight = random_double(min_weight, max_weight);
                add_edge(graph, u, v, weight);
                add_edge(graph, v, u, weight);
            }

            // Diagonal neighbors (if enabled)
            if (diagonal) {
                if (r + 1 < rows && c + 1 < cols) {
                    int v = (r + 1) * cols + (c + 1);
                    double weight = random_double(min_weight, max_weight);
                    add_edge(graph, u, v, weight);
                    add_edge(graph, v, u, weight);
                }
                if (r + 1 < rows && c - 1 >= 0) {
                    int v = (r + 1) * cols + (c - 1);
                    double weight = random_double(min_weight, max_weight);
                    add_edge(graph, u, v, weight);
                    add_edge(graph, v, u, weight);
                }
            }
        }
    }

    return graph;
}

Graph* graph_generate_tree(int n, double min_weight, double max_weight) {
    if (n <= 0 || n > MAX_VERTICES) {
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // Create a random tree by connecting each vertex to a random earlier vertex
    for (int v = 1; v < n; v++) {
        int u = random_int(0, v - 1);
        double weight = random_double(min_weight, max_weight);
        add_edge(graph, u, v, weight);
        add_edge(graph, v, u, weight);
    }

    return graph;
}

Graph* graph_generate_dag(int n, int m, double min_weight, double max_weight) {
    if (n <= 0 || n > MAX_VERTICES || m < 0) {
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    int edges_added = 0;
    while (edges_added < m) {
        int u = random_int(0, n - 1);
        int v = random_int(0, n - 1);

        // Ensure DAG property: u < v
        if (u >= v) {
            continue;
        }

        double weight = random_double(min_weight, max_weight);
        add_edge(graph, u, v, weight);
        edges_added++;
    }

    return graph;
}

Graph* graph_generate_sparse(int n, int avg_degree, double min_weight, double max_weight) {
    if (n <= 0 || n > MAX_VERTICES || avg_degree < 0) {
        return NULL;
    }

    int m = (n * avg_degree) / 2;
    GraphGenParams params = graph_gen_params_default(n, m);
    params.min_weight = min_weight;
    params.max_weight = max_weight;

    return graph_generate_random(&params);
}

Graph* graph_generate_path(int n, double min_weight, double max_weight) {
    if (n <= 0 || n > MAX_VERTICES) {
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    for (int u = 0; u < n - 1; u++) {
        double weight = random_double(min_weight, max_weight);
        add_edge(graph, u, u + 1, weight);
    }

    return graph;
}

/*============================================================================
 * General Utilities Implementation
 *============================================================================*/

void utils_set_random_seed(unsigned int seed) {
    if (seed == 0) {
        random_seed_state = (unsigned int)time(NULL);
    } else {
        random_seed_state = seed;
    }
    srand(random_seed_state);
}

int random_int(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + (rand() % (max - min + 1));
}

double random_double(double min, double max) {
    if (min > max) {
        double temp = min;
        min = max;
        max = temp;
    }
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

bool parse_args(int argc, char** argv, int* n, int* m, int* source) {
    if (argc < 4) {
        return false;
    }

    *n = atoi(argv[1]);
    *m = atoi(argv[2]);
    *source = atoi(argv[3]);

    if (*n <= 0 || *m < 0 || *source < 0) {
        return false;
    }

    return true;
}

void print_usage(const char* program_name) {
    printf("Usage: %s <n> <m> <source> [options]\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  n         Number of vertices\n");
    printf("  m         Number of edges\n");
    printf("  source    Source vertex (0-indexed)\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --verbose  Enable verbose output\n");
    printf("\n");
}
