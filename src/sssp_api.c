/**
 * @file sssp_api.c
 * @brief Production-ready implementation of the SSSP Breakthrough Algorithm Public API
 *
 * This file implements all functions declared in sssp.h with comprehensive
 * error handling, memory management, and thread safety documentation.
 *
 * @version 1.0.0
 * @date 2025
 */

#include "../include/sssp.h"
#include "../src/sssp_breakthrough.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>

/* ============================================================================
 * INTERNAL STRUCTURES
 * ============================================================================ */

/**
 * @brief Internal structure for edge representation
 */
typedef struct sssp_edge {
    int target;
    double weight;
    struct sssp_edge* next;
} sssp_edge_t;

/**
 * @brief Internal structure for graph (matches sssp_graph_t)
 */
struct sssp_graph {
    int num_vertices;
    int num_edges;
    sssp_edge_t** adj_list;     /* Adjacency list */
    int magic;                   /* Magic number for validation */
};

#define GRAPH_MAGIC 0x47524148  /* "GRAH" */
#define RESULT_MAGIC 0x52455354  /* "REST" */

/**
 * @brief Internal structure for SSSP result (matches sssp_result_t)
 */
struct sssp_result {
    int num_vertices;
    int source;
    double* distances;
    int* predecessors;
    sssp_status_t status;
    sssp_graph_t* graph_ref;    /* Reference to original graph for validation */
    int magic;                   /* Magic number for validation */
};

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */

/**
 * @brief Validate graph pointer
 */
static bool is_valid_graph(const sssp_graph_t* graph) {
    return graph != NULL && graph->magic == GRAPH_MAGIC;
}

/**
 * @brief Validate result pointer
 */
static bool is_valid_result(const sssp_result_t* result) {
    return result != NULL && result->magic == RESULT_MAGIC;
}

/**
 * @brief Safe string copy
 */
static void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest != NULL && src != NULL && dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

/* ============================================================================
 * GRAPH MANAGEMENT IMPLEMENTATION
 * ============================================================================ */

sssp_graph_t* sssp_graph_create(int num_vertices) {
    /* Validate input */
    if (num_vertices <= 0) {
        return NULL;
    }
    if (num_vertices > SSSP_MAX_VERTICES) {
        return NULL;
    }

    /* Allocate graph structure */
    sssp_graph_t* graph = (sssp_graph_t*)calloc(1, sizeof(sssp_graph_t));
    if (graph == NULL) {
        return NULL;
    }

    /* Allocate adjacency list */
    graph->adj_list = (sssp_edge_t**)calloc(num_vertices, sizeof(sssp_edge_t*));
    if (graph->adj_list == NULL) {
        free(graph);
        return NULL;
    }

    graph->num_vertices = num_vertices;
    graph->num_edges = 0;
    graph->magic = GRAPH_MAGIC;

    return graph;
}

sssp_status_t sssp_graph_add_edge(sssp_graph_t* graph, int from, int to, double weight) {
    /* Validate inputs */
    if (!is_valid_graph(graph)) {
        return SSSP_ERROR_NULL_POINTER;
    }
    if (from < 0 || from >= graph->num_vertices || to < 0 || to >= graph->num_vertices) {
        return SSSP_ERROR_INVALID_VERTEX;
    }
    if (isnan(weight) || isinf(weight)) {
        return SSSP_ERROR_INVALID_EDGE;
    }
    if (graph->num_edges >= SSSP_MAX_EDGES) {
        return SSSP_ERROR_GRAPH_TOO_LARGE;
    }

    /* Allocate new edge */
    sssp_edge_t* edge = (sssp_edge_t*)malloc(sizeof(sssp_edge_t));
    if (edge == NULL) {
        return SSSP_ERROR_MEMORY_ALLOCATION;
    }

    /* Initialize edge */
    edge->target = to;
    edge->weight = weight;
    edge->next = graph->adj_list[from];
    graph->adj_list[from] = edge;
    graph->num_edges++;

    return SSSP_SUCCESS;
}

sssp_status_t sssp_graph_add_undirected_edge(sssp_graph_t* graph, int u, int v, double weight) {
    sssp_status_t status;

    status = sssp_graph_add_edge(graph, u, v, weight);
    if (status != SSSP_SUCCESS) {
        return status;
    }

    status = sssp_graph_add_edge(graph, v, u, weight);
    return status;
}

int sssp_graph_get_num_vertices(const sssp_graph_t* graph) {
    if (!is_valid_graph(graph)) {
        return -1;
    }
    return graph->num_vertices;
}

int sssp_graph_get_num_edges(const sssp_graph_t* graph) {
    if (!is_valid_graph(graph)) {
        return -1;
    }
    return graph->num_edges;
}

void sssp_graph_destroy(sssp_graph_t* graph) {
    if (!is_valid_graph(graph)) {
        return;
    }

    /* Free adjacency list */
    if (graph->adj_list != NULL) {
        for (int i = 0; i < graph->num_vertices; i++) {
            sssp_edge_t* edge = graph->adj_list[i];
            while (edge != NULL) {
                sssp_edge_t* next = edge->next;
                free(edge);
                edge = next;
            }
        }
        free(graph->adj_list);
    }

    /* Invalidate magic number */
    graph->magic = 0;
    free(graph);
}

/* ============================================================================
 * GRAPH VALIDATION IMPLEMENTATION
 * ============================================================================ */

bool sssp_graph_has_negative_cycle(sssp_graph_t* graph, int* cycle_vertices, int* cycle_length) {
    if (!is_valid_graph(graph)) {
        return false;
    }

    int n = graph->num_vertices;
    double* dist = (double*)malloc(n * sizeof(double));
    int* pred = (int*)malloc(n * sizeof(int));

    if (dist == NULL || pred == NULL) {
        free(dist);
        free(pred);
        return false;
    }

    /* Initialize distances */
    for (int i = 0; i < n; i++) {
        dist[i] = SSSP_INFINITY;
        pred[i] = -1;
    }
    dist[0] = 0.0;

    /* Bellman-Ford: Relax edges n-1 times */
    for (int iter = 0; iter < n - 1; iter++) {
        for (int u = 0; u < n; u++) {
            if (dist[u] == SSSP_INFINITY) continue;

            sssp_edge_t* edge = graph->adj_list[u];
            while (edge != NULL) {
                int v = edge->target;
                double new_dist = dist[u] + edge->weight;

                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    pred[v] = u;
                }
                edge = edge->next;
            }
        }
    }

    /* Check for negative cycle */
    bool has_cycle = false;
    int cycle_start = -1;

    for (int u = 0; u < n; u++) {
        if (dist[u] == SSSP_INFINITY) continue;

        sssp_edge_t* edge = graph->adj_list[u];
        while (edge != NULL) {
            int v = edge->target;
            if (dist[u] + edge->weight < dist[v] - 1e-9) {
                has_cycle = true;
                cycle_start = v;
                break;
            }
            edge = edge->next;
        }
        if (has_cycle) break;
    }

    /* Reconstruct cycle if requested */
    if (has_cycle && cycle_vertices != NULL && cycle_length != NULL) {
        bool* visited = (bool*)calloc(n, sizeof(bool));
        int curr = cycle_start;
        int len = 0;

        /* Walk back to find a vertex in the cycle */
        for (int i = 0; i < n && curr != -1; i++) {
            curr = pred[curr];
        }

        /* Now curr is definitely in a cycle, extract it */
        if (curr != -1) {
            int start = curr;
            do {
                cycle_vertices[len++] = curr;
                visited[curr] = true;
                curr = pred[curr];
            } while (curr != start && len < n);
            cycle_vertices[len++] = start;  /* Close the cycle */
        }

        *cycle_length = len;
        free(visited);
    } else if (cycle_length != NULL) {
        *cycle_length = 0;
    }

    free(dist);
    free(pred);
    return has_cycle;
}

sssp_status_t sssp_graph_validate(sssp_graph_t* graph, char* error_msg, size_t msg_size) {
    if (!is_valid_graph(graph)) {
        safe_strcpy(error_msg, "Invalid graph pointer", msg_size);
        return SSSP_ERROR_NULL_POINTER;
    }

    /* Check for negative self-loops */
    for (int u = 0; u < graph->num_vertices; u++) {
        sssp_edge_t* edge = graph->adj_list[u];
        while (edge != NULL) {
            if (edge->target == u && edge->weight < 0) {
                safe_strcpy(error_msg, "Negative self-loop detected", msg_size);
                return SSSP_ERROR_INVALID_GRAPH;
            }
            edge = edge->next;
        }
    }

    /* Check for negative cycles */
    if (sssp_graph_has_negative_cycle(graph, NULL, NULL)) {
        safe_strcpy(error_msg, "Negative cycle detected", msg_size);
        return SSSP_ERROR_NEGATIVE_CYCLE;
    }

    safe_strcpy(error_msg, "Graph is valid", msg_size);
    return SSSP_SUCCESS;
}

/* ============================================================================
 * BREAKTHROUGH ALGORITHM INTEGRATION
 * ============================================================================ */

/* Note: Graph and Edge types are declared in sssp_breakthrough.h */

/**
 * @brief Convert API graph to breakthrough algorithm graph structure
 *
 * Creates a Graph structure compatible with the breakthrough algorithm
 * by converting from the public API's sssp_graph_t representation.
 *
 * @param api_graph The public API graph
 * @return Graph structure for breakthrough algorithm, or NULL on error
 */
static Graph* convert_to_breakthrough_graph(const sssp_graph_t* api_graph) {
    if (!is_valid_graph(api_graph)) {
        return NULL;
    }

    int n = api_graph->num_vertices;

    /* Allocate Graph structure */
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (g == NULL) {
        return NULL;
    }

    g->n = n;
    g->m = 0;  /* Will count edges during conversion */

    /* Allocate arrays */
    g->adj = (Edge**)calloc(n, sizeof(Edge*));
    g->dist = (double*)malloc(n * sizeof(double));
    g->pred = (int*)malloc(n * sizeof(int));
    g->complete = (bool*)calloc(n, sizeof(bool));

    if (g->adj == NULL || g->dist == NULL || g->pred == NULL || g->complete == NULL) {
        free(g->adj);
        free(g->dist);
        free(g->pred);
        free(g->complete);
        free(g);
        return NULL;
    }

    /* Initialize arrays */
    for (int i = 0; i < n; i++) {
        g->dist[i] = SSSP_INFINITY;
        g->pred[i] = -1;
        g->complete[i] = false;
        g->adj[i] = NULL;
    }

    /* Copy edges from API graph to breakthrough graph */
    for (int u = 0; u < n; u++) {
        sssp_edge_t* api_edge = api_graph->adj_list[u];
        Edge* prev_edge = NULL;

        while (api_edge != NULL) {
            /* Allocate new edge for breakthrough graph */
            Edge* edge = (Edge*)malloc(sizeof(Edge));
            if (edge == NULL) {
                /* Cleanup on error */
                for (int j = 0; j < n; j++) {
                    Edge* e = g->adj[j];
                    while (e != NULL) {
                        Edge* next = e->next;
                        free(e);
                        e = next;
                    }
                }
                free(g->adj);
                free(g->dist);
                free(g->pred);
                free(g->complete);
                free(g);
                return NULL;
            }

            edge->target = api_edge->target;
            edge->weight = api_edge->weight;
            edge->next = NULL;

            /* Link edge into adjacency list */
            if (prev_edge == NULL) {
                g->adj[u] = edge;
            } else {
                prev_edge->next = edge;
            }
            prev_edge = edge;
            g->m++;

            api_edge = api_edge->next;
        }
    }

    g->source = -1;  /* Will be set when algorithm runs */
    return g;
}

/**
 * @brief Free breakthrough algorithm graph structure
 */
static void free_breakthrough_graph(Graph* g) {
    if (g == NULL) {
        return;
    }

    /* Free adjacency lists */
    if (g->adj != NULL) {
        for (int i = 0; i < g->n; i++) {
            Edge* edge = g->adj[i];
            while (edge != NULL) {
                Edge* next = edge->next;
                free(edge);
                edge = next;
            }
        }
        free(g->adj);
    }

    free(g->dist);
    free(g->pred);
    free(g->complete);
    free(g);
}

/* ============================================================================
 * SSSP SOLVER IMPLEMENTATION
 * ============================================================================ */

/**
 * @brief Standard Dijkstra's algorithm implementation for small graphs
 *
 * This is used as a fallback for graphs with n < 100 where the breakthrough
 * algorithm's parameters (especially k) become too small for correct operation.
 *
 * @param graph The input graph
 * @param source The source vertex
 * @return SSSP result structure, or NULL on error
 */
static sssp_result_t* sssp_solve_dijkstra(sssp_graph_t* graph, int source) {
    int n = graph->num_vertices;

    /* Allocate result structure */
    sssp_result_t* result = (sssp_result_t*)calloc(1, sizeof(sssp_result_t));
    if (result == NULL) {
        return NULL;
    }

    result->num_vertices = n;
    result->source = source;
    result->graph_ref = graph;
    result->status = SSSP_SUCCESS;
    result->magic = RESULT_MAGIC;

    /* Allocate distance and predecessor arrays */
    result->distances = (double*)malloc(n * sizeof(double));
    result->predecessors = (int*)malloc(n * sizeof(int));

    if (result->distances == NULL || result->predecessors == NULL) {
        free(result->distances);
        free(result->predecessors);
        free(result);
        return NULL;
    }

    /* Initialize arrays */
    for (int i = 0; i < n; i++) {
        result->distances[i] = SSSP_INFINITY;
        result->predecessors[i] = SSSP_INVALID_VERTEX;
    }
    result->distances[source] = 0.0;

    /* Run Dijkstra's algorithm */
    bool* complete = (bool*)calloc(n, sizeof(bool));
    if (complete == NULL) {
        free(result->distances);
        free(result->predecessors);
        free(result);
        return NULL;
    }

    for (int iter = 0; iter < n; iter++) {
        /* Find minimum distance incomplete vertex */
        int u = -1;
        double min_dist = SSSP_INFINITY;

        for (int v = 0; v < n; v++) {
            if (!complete[v] && result->distances[v] < min_dist) {
                min_dist = result->distances[v];
                u = v;
            }
        }

        if (u == -1) break;  /* All remaining vertices are unreachable */

        complete[u] = true;

        /* Relax edges from u */
        sssp_edge_t* edge = graph->adj_list[u];
        while (edge != NULL) {
            int v = edge->target;
            double new_dist = result->distances[u] + edge->weight;

            if (new_dist < result->distances[v]) {
                result->distances[v] = new_dist;
                result->predecessors[v] = u;
            }
            edge = edge->next;
        }
    }

    free(complete);
    return result;
}

sssp_result_t* sssp_solve(sssp_graph_t* graph, int source) {
    /* Validate inputs */
    if (!is_valid_graph(graph)) {
        return NULL;
    }
    if (source < 0 || source >= graph->num_vertices) {
        return NULL;
    }

    int n = graph->num_vertices;

    /* ALGORITHM SELECTION STRATEGY:
     *
     * The breakthrough algorithm achieves O(m log^{2/3} n) complexity but has
     * overhead that makes it less efficient for very small graphs.
     *
     * For small graphs (n < 50):
     * - k = ⌊log^{1/3}(n)⌋ becomes too small (k=1)
     * - Overhead dominates the speedup
     * - Simple Dijkstra is faster in practice
     *
     * For medium and large graphs (n >= 50):
     * - Use breakthrough algorithm for theoretical speedup
     * - Factor of log^{1/3}(n) improvement over Dijkstra
     * - Properly handles complete SSSP with fixed implementation
     */
    if (n < 50) {  /* Use simple Dijkstra for very small graphs */
        return sssp_solve_dijkstra(graph, source);
    }

    /* Convert API graph to breakthrough algorithm graph */
    Graph* bt_graph = convert_to_breakthrough_graph(graph);
    if (bt_graph == NULL) {
        return NULL;
    }

    /* Run breakthrough O(m log^{2/3} n) algorithm */
    sssp_breakthrough(bt_graph, source);

    /* Allocate result structure */
    sssp_result_t* result = (sssp_result_t*)calloc(1, sizeof(sssp_result_t));
    if (result == NULL) {
        free_breakthrough_graph(bt_graph);
        return NULL;
    }

    result->num_vertices = n;
    result->source = source;
    result->graph_ref = graph;
    result->status = SSSP_SUCCESS;
    result->magic = RESULT_MAGIC;

    /* Allocate distance and predecessor arrays */
    result->distances = (double*)malloc(n * sizeof(double));
    result->predecessors = (int*)malloc(n * sizeof(int));

    if (result->distances == NULL || result->predecessors == NULL) {
        free(result->distances);
        free(result->predecessors);
        free(result);
        free_breakthrough_graph(bt_graph);
        return NULL;
    }

    /* Copy results from breakthrough graph to API result */
    for (int i = 0; i < n; i++) {
        result->distances[i] = bt_graph->dist[i];
        result->predecessors[i] = bt_graph->pred[i];
    }

    /* Cleanup breakthrough graph */
    free_breakthrough_graph(bt_graph);

    return result;
}

/* ============================================================================
 * RESULT QUERYING IMPLEMENTATION
 * ============================================================================ */

sssp_status_t sssp_result_get_status(const sssp_result_t* result) {
    if (!is_valid_result(result)) {
        return SSSP_ERROR_NULL_POINTER;
    }
    return result->status;
}

double sssp_result_get_distance(const sssp_result_t* result, int vertex) {
    if (!is_valid_result(result)) {
        return SSSP_INFINITY;
    }
    if (vertex < 0 || vertex >= result->num_vertices) {
        return SSSP_INFINITY;
    }
    return result->distances[vertex];
}

int sssp_result_get_predecessor(const sssp_result_t* result, int vertex) {
    if (!is_valid_result(result)) {
        return SSSP_INVALID_VERTEX;
    }
    if (vertex < 0 || vertex >= result->num_vertices) {
        return SSSP_INVALID_VERTEX;
    }
    return result->predecessors[vertex];
}

int sssp_result_get_source(const sssp_result_t* result) {
    if (!is_valid_result(result)) {
        return -1;
    }
    return result->source;
}

bool sssp_result_is_reachable(const sssp_result_t* result, int vertex) {
    if (!is_valid_result(result)) {
        return false;
    }
    if (vertex < 0 || vertex >= result->num_vertices) {
        return false;
    }
    return result->distances[vertex] < SSSP_INFINITY;
}

sssp_status_t sssp_result_get_path(const sssp_result_t* result, int vertex,
                                    int* path, int* path_length) {
    if (!is_valid_result(result)) {
        return SSSP_ERROR_NULL_POINTER;
    }
    if (path == NULL || path_length == NULL) {
        return SSSP_ERROR_NULL_POINTER;
    }
    if (vertex < 0 || vertex >= result->num_vertices) {
        return SSSP_ERROR_INVALID_VERTEX;
    }

    /* Check if vertex is reachable */
    if (!sssp_result_is_reachable(result, vertex)) {
        *path_length = 0;
        return SSSP_SUCCESS;
    }

    /* Reconstruct path by following predecessors */
    int temp_path[SSSP_MAX_VERTICES];
    int len = 0;
    int curr = vertex;

    /* Walk backwards from vertex to source */
    while (curr != SSSP_INVALID_VERTEX && len < SSSP_MAX_VERTICES) {
        temp_path[len++] = curr;
        if (curr == result->source) {
            break;
        }
        curr = result->predecessors[curr];
    }

    /* Check for cycle in predecessor chain */
    if (curr != result->source) {
        return SSSP_ERROR_INVALID_GRAPH;
    }

    /* Reverse path to get source-to-vertex order */
    for (int i = 0; i < len; i++) {
        path[i] = temp_path[len - 1 - i];
    }
    *path_length = len;

    return SSSP_SUCCESS;
}

bool sssp_result_verify(const sssp_result_t* result) {
    if (!is_valid_result(result) || !is_valid_graph(result->graph_ref)) {
        return false;
    }

    /* Check triangle inequality for all edges */
    sssp_graph_t* graph = result->graph_ref;

    for (int u = 0; u < graph->num_vertices; u++) {
        if (result->distances[u] == SSSP_INFINITY) {
            continue;
        }

        sssp_edge_t* edge = graph->adj_list[u];
        while (edge != NULL) {
            int v = edge->target;
            double expected_dist = result->distances[u] + edge->weight;

            /* Allow small numerical error */
            if (expected_dist < result->distances[v] - 1e-9) {
                return false;  /* Triangle inequality violated */
            }
            edge = edge->next;
        }
    }

    return true;
}

void sssp_result_destroy(sssp_result_t* result) {
    if (!is_valid_result(result)) {
        return;
    }

    free(result->distances);
    free(result->predecessors);
    result->magic = 0;  /* Invalidate */
    free(result);
}

/* ============================================================================
 * GRAPH I/O IMPLEMENTATION
 * ============================================================================ */

sssp_graph_t* sssp_graph_load_edge_list(const char* filename, char* error_msg, size_t msg_size) {
    if (filename == NULL) {
        safe_strcpy(error_msg, "Filename is NULL", msg_size);
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        safe_strcpy(error_msg, "Cannot open file", msg_size);
        return NULL;
    }

    int num_vertices, num_edges;
    if (fscanf(fp, "%d %d", &num_vertices, &num_edges) != 2) {
        safe_strcpy(error_msg, "Invalid file format", msg_size);
        fclose(fp);
        return NULL;
    }

    sssp_graph_t* graph = sssp_graph_create(num_vertices);
    if (graph == NULL) {
        safe_strcpy(error_msg, "Failed to create graph", msg_size);
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < num_edges; i++) {
        int from, to;
        double weight;

        if (fscanf(fp, "%d %d %lf", &from, &to, &weight) != 3) {
            safe_strcpy(error_msg, "Invalid edge format", msg_size);
            sssp_graph_destroy(graph);
            fclose(fp);
            return NULL;
        }

        sssp_status_t status = sssp_graph_add_edge(graph, from, to, weight);
        if (status != SSSP_SUCCESS) {
            safe_strcpy(error_msg, "Failed to add edge", msg_size);
            sssp_graph_destroy(graph);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    safe_strcpy(error_msg, "Success", msg_size);
    return graph;
}

sssp_status_t sssp_graph_save_edge_list(const sssp_graph_t* graph, const char* filename) {
    if (!is_valid_graph(graph) || filename == NULL) {
        return SSSP_ERROR_NULL_POINTER;
    }

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        return SSSP_ERROR_INVALID_GRAPH;
    }

    /* Write header */
    fprintf(fp, "%d %d\n", graph->num_vertices, graph->num_edges);

    /* Write edges */
    for (int u = 0; u < graph->num_vertices; u++) {
        sssp_edge_t* edge = graph->adj_list[u];
        while (edge != NULL) {
            fprintf(fp, "%d %d %.10f\n", u, edge->target, edge->weight);
            edge = edge->next;
        }
    }

    fclose(fp);
    return SSSP_SUCCESS;
}

sssp_graph_t* sssp_graph_load_dimacs(const char* filename, char* error_msg, size_t msg_size) {
    if (filename == NULL) {
        safe_strcpy(error_msg, "Filename is NULL", msg_size);
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        safe_strcpy(error_msg, "Cannot open file", msg_size);
        return NULL;
    }

    char line[1024];
    int num_vertices = 0, num_edges = 0;
    sssp_graph_t* graph = NULL;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'c') {
            /* Comment line, skip */
            continue;
        } else if (line[0] == 'p') {
            /* Problem line: p sp <vertices> <edges> */
            char format[10];
            if (sscanf(line, "p %s %d %d", format, &num_vertices, &num_edges) != 3) {
                safe_strcpy(error_msg, "Invalid problem line", msg_size);
                fclose(fp);
                return NULL;
            }

            graph = sssp_graph_create(num_vertices);
            if (graph == NULL) {
                safe_strcpy(error_msg, "Failed to create graph", msg_size);
                fclose(fp);
                return NULL;
            }
        } else if (line[0] == 'a') {
            /* Arc line: a <from> <to> <weight> */
            if (graph == NULL) {
                safe_strcpy(error_msg, "Edge before problem line", msg_size);
                fclose(fp);
                return NULL;
            }

            int from, to;
            double weight;
            if (sscanf(line, "a %d %d %lf", &from, &to, &weight) != 3) {
                safe_strcpy(error_msg, "Invalid edge line", msg_size);
                sssp_graph_destroy(graph);
                fclose(fp);
                return NULL;
            }

            /* DIMACS uses 1-indexed vertices, convert to 0-indexed */
            from--;
            to--;

            sssp_status_t status = sssp_graph_add_edge(graph, from, to, weight);
            if (status != SSSP_SUCCESS) {
                safe_strcpy(error_msg, "Failed to add edge", msg_size);
                sssp_graph_destroy(graph);
                fclose(fp);
                return NULL;
            }
        }
    }

    fclose(fp);
    safe_strcpy(error_msg, "Success", msg_size);
    return graph;
}

sssp_status_t sssp_result_export(const sssp_result_t* result, const char* filename) {
    if (!is_valid_result(result) || filename == NULL) {
        return SSSP_ERROR_NULL_POINTER;
    }

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        return SSSP_ERROR_INVALID_GRAPH;
    }

    fprintf(fp, "# SSSP Result - Source vertex: %d\n", result->source);
    fprintf(fp, "# Format: vertex distance predecessor\n");
    fprintf(fp, "%d\n", result->num_vertices);

    for (int v = 0; v < result->num_vertices; v++) {
        fprintf(fp, "%d %.10f %d\n", v, result->distances[v], result->predecessors[v]);
    }

    fclose(fp);
    return SSSP_SUCCESS;
}

/* ============================================================================
 * UTILITY FUNCTIONS IMPLEMENTATION
 * ============================================================================ */

const char* sssp_status_to_string(sssp_status_t status) {
    switch (status) {
        case SSSP_SUCCESS:
            return "Success";
        case SSSP_ERROR_NULL_POINTER:
            return "Null pointer error";
        case SSSP_ERROR_INVALID_VERTEX:
            return "Invalid vertex ID";
        case SSSP_ERROR_INVALID_EDGE:
            return "Invalid edge parameters";
        case SSSP_ERROR_MEMORY_ALLOCATION:
            return "Memory allocation failed";
        case SSSP_ERROR_GRAPH_TOO_LARGE:
            return "Graph exceeds size limits";
        case SSSP_ERROR_NEGATIVE_CYCLE:
            return "Negative cycle detected";
        case SSSP_ERROR_INVALID_GRAPH:
            return "Invalid graph structure";
        case SSSP_ERROR_NOT_SOLVED:
            return "SSSP not solved";
        default:
            return "Unknown error";
    }
}

void sssp_result_print(const sssp_result_t* result) {
    if (!is_valid_result(result)) {
        printf("Invalid result\n");
        return;
    }

    printf("SSSP Result from source vertex %d:\n", result->source);
    printf("%-8s %-15s %-12s\n", "Vertex", "Distance", "Predecessor");
    printf("-------------------------------------------\n");

    for (int v = 0; v < result->num_vertices; v++) {
        if (result->distances[v] < SSSP_INFINITY) {
            printf("%-8d %-15.6f %-12d\n", v, result->distances[v],
                   result->predecessors[v]);
        } else {
            printf("%-8d %-15s %-12s\n", v, "INFINITY", "-");
        }
    }
}

const char* sssp_get_version(void) {
    return "SSSP Breakthrough Algorithm v1.0.0 (2025)";
}

const char* sssp_get_info(void) {
    return "O(m log^{2/3} n) Single-Source Shortest Paths Algorithm\n"
           "Paper: 'Breaking the Sorting Barrier for Directed SSSP'\n"
           "Authors: Duan, Mao, Mao, Shu, Yin (2025)";
}
