/**
 * @file utils.h
 * @brief Utility functions for SSSP breakthrough algorithm
 *
 * This header provides utility functions for:
 * - Graph file I/O (various formats)
 * - Memory management
 * - Performance timing
 * - Graph validation
 * - Random graph generation
 */

#ifndef SSSP_UTILS_H
#define SSSP_UTILS_H

#include "sssp_breakthrough.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup FileIO Graph File I/O
 * @brief Functions for reading and writing graphs in various formats
 * @{
 */

/**
 * @brief Graph file format types
 */
typedef enum {
    GRAPH_FORMAT_EDGE_LIST,    /**< Simple edge list: u v weight */
    GRAPH_FORMAT_ADJACENCY,    /**< Adjacency list format */
    GRAPH_FORMAT_DIMACS,       /**< DIMACS challenge format */
    GRAPH_FORMAT_MATRIX,       /**< Adjacency matrix format */
    GRAPH_FORMAT_DOT           /**< GraphViz DOT format */
} GraphFileFormat;

/**
 * @brief Read a graph from a file
 *
 * Automatically detects format or uses specified format.
 * Edge list format: Each line contains "u v weight"
 * DIMACS format: Standard DIMACS shortest path format
 *
 * @param filename Path to file
 * @param format Format to use (GRAPH_FORMAT_EDGE_LIST recommended)
 * @param n Output: number of vertices
 * @return Pointer to created graph, or NULL on error
 */
Graph* graph_read_file(const char* filename, GraphFileFormat format, int* n);

/**
 * @brief Write a graph to a file
 *
 * @param graph Graph to write
 * @param filename Output file path
 * @param format Output format
 * @return true on success, false on error
 */
bool graph_write_file(const Graph* graph, const char* filename, GraphFileFormat format);

/**
 * @brief Read graph from edge list format
 *
 * Format: First line contains n (vertices) and m (edges)
 *         Following lines: u v weight
 *
 * @param filename File path
 * @return Pointer to created graph, or NULL on error
 */
Graph* graph_read_edge_list(const char* filename);

/**
 * @brief Write graph in edge list format
 *
 * @param graph Graph to write
 * @param filename Output file path
 * @return true on success, false on error
 */
bool graph_write_edge_list(const Graph* graph, const char* filename);

/**
 * @brief Read graph from DIMACS format
 *
 * Standard DIMACS shortest path problem format.
 *
 * @param filename File path
 * @return Pointer to created graph, or NULL on error
 */
Graph* graph_read_dimacs(const char* filename);

/**
 * @brief Write graph in DIMACS format
 *
 * @param graph Graph to write
 * @param filename Output file path
 * @param source Source vertex for DIMACS format
 * @return true on success, false on error
 */
bool graph_write_dimacs(const Graph* graph, const char* filename, int source);

/**
 * @brief Write graph in DOT format for visualization
 *
 * Output can be rendered with GraphViz: dot -Tpng graph.dot -o graph.png
 *
 * @param graph Graph to write
 * @param filename Output file path
 * @param distances Optional distance array for coloring (can be NULL)
 * @return true on success, false on error
 */
bool graph_write_dot(const Graph* graph, const char* filename, const double* distances);

/** @} */

/**
 * @defgroup Memory Memory Management
 * @brief Safe memory allocation and tracking utilities
 * @{
 */

/**
 * @brief Safe malloc with error checking
 *
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void* safe_malloc(size_t size);

/**
 * @brief Safe calloc with error checking
 *
 * @param nmemb Number of elements
 * @param size Size of each element
 * @return Pointer to allocated and zeroed memory, or NULL on failure
 */
void* safe_calloc(size_t nmemb, size_t size);

/**
 * @brief Safe realloc with error checking
 *
 * @param ptr Pointer to reallocate
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* safe_realloc(void* ptr, size_t size);

/**
 * @brief Safe free (NULL-safe)
 *
 * @param ptr Pointer to free (can be NULL)
 */
void safe_free(void* ptr);

/**
 * @brief Get current memory usage statistics
 *
 * @param allocated Output: bytes allocated
 * @param peak Output: peak bytes allocated
 */
void memory_get_stats(size_t* allocated, size_t* peak);

/**
 * @brief Reset memory statistics
 */
void memory_reset_stats(void);

/**
 * @brief Print memory statistics to stderr
 */
void memory_print_stats(void);

/** @} */

/**
 * @defgroup Timing Performance Timing
 * @brief Cross-platform high-resolution timing functions
 * @{
 */

/**
 * @brief Timer structure for performance measurement
 */
typedef struct {
    struct timespec start_time;
    struct timespec end_time;
    double elapsed_seconds;
    bool running;
} Timer;

/**
 * @brief Create and start a new timer
 *
 * @return Pointer to timer, or NULL on error
 */
Timer* timer_create(void);

/**
 * @brief Start or restart a timer
 *
 * @param timer Timer to start
 */
void timer_start(Timer* timer);

/**
 * @brief Stop a timer
 *
 * @param timer Timer to stop
 * @return Elapsed time in seconds
 */
double timer_stop(Timer* timer);

/**
 * @brief Get elapsed time without stopping timer
 *
 * @param timer Timer to query
 * @return Elapsed time in seconds
 */
double timer_elapsed(const Timer* timer);

/**
 * @brief Destroy a timer
 *
 * @param timer Timer to destroy
 */
void timer_destroy(Timer* timer);

/**
 * @brief Get current timestamp in microseconds
 *
 * @return Microseconds since epoch
 */
long long get_timestamp_us(void);

/**
 * @brief Format time duration as human-readable string
 *
 * @param seconds Duration in seconds
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 */
void format_time(double seconds, char* buffer, size_t buffer_size);

/** @} */

/**
 * @defgroup Validation Graph Validation
 * @brief Functions for validating and analyzing graphs
 * @{
 */

/**
 * @brief Graph properties structure
 */
typedef struct {
    int num_vertices;          /**< Number of vertices */
    int num_edges;             /**< Number of edges */
    bool is_connected;         /**< Whether graph is strongly connected */
    bool has_negative_weights; /**< Whether graph has negative weights */
    bool has_negative_cycle;   /**< Whether graph has negative cycle */
    bool is_dag;               /**< Whether graph is a DAG */
    double min_weight;         /**< Minimum edge weight */
    double max_weight;         /**< Maximum edge weight */
    double avg_degree;         /**< Average vertex degree */
    int max_degree;            /**< Maximum vertex degree */
    int num_components;        /**< Number of weakly connected components */
} GraphProperties;

/**
 * @brief Compute graph properties
 *
 * @param graph Graph to analyze
 * @return Graph properties structure
 */
GraphProperties graph_get_properties(const Graph* graph);

/**
 * @brief Print graph properties
 *
 * @param props Properties to print
 */
void graph_print_properties(const GraphProperties* props);

/**
 * @brief Check if graph has negative cycle
 *
 * Uses Bellman-Ford algorithm to detect negative cycles.
 *
 * @param graph Graph to check
 * @param source Source vertex for detection
 * @return true if negative cycle detected, false otherwise
 */
bool graph_has_negative_cycle(const Graph* graph, int source);

/**
 * @brief Check if graph is connected (weakly)
 *
 * @param graph Graph to check
 * @return true if connected, false otherwise
 */
bool graph_is_connected(const Graph* graph);

/**
 * @brief Check if graph is a DAG (Directed Acyclic Graph)
 *
 * @param graph Graph to check
 * @return true if DAG, false otherwise
 */
bool graph_is_dag(const Graph* graph);

/**
 * @brief Validate graph structure
 *
 * Checks for:
 * - NULL pointers
 * - Valid vertex IDs
 * - Consistent edge counts
 *
 * @param graph Graph to validate
 * @return true if valid, false otherwise
 */
bool graph_validate(const Graph* graph);

/** @} */

/**
 * @defgroup Generators Random Graph Generators
 * @brief Functions for generating random graphs for testing
 * @{
 */

/**
 * @brief Random graph generation parameters
 */
typedef struct {
    int n;                     /**< Number of vertices */
    int m;                     /**< Number of edges (or -1 for probability) */
    double p;                  /**< Edge probability (for Erdős-Rényi) */
    double min_weight;         /**< Minimum edge weight */
    double max_weight;         /**< Maximum edge weight */
    bool allow_negative;       /**< Allow negative weights */
    bool directed;             /**< Generate directed graph */
    unsigned int seed;         /**< Random seed (0 for time-based) */
} GraphGenParams;

/**
 * @brief Create default graph generation parameters
 *
 * @param n Number of vertices
 * @param m Number of edges
 * @return Default parameters structure
 */
GraphGenParams graph_gen_params_default(int n, int m);

/**
 * @brief Generate random graph (Erdős-Rényi model)
 *
 * Each edge appears with probability p.
 *
 * @param params Generation parameters
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_random(const GraphGenParams* params);

/**
 * @brief Generate random complete graph
 *
 * All vertices connected to all other vertices.
 *
 * @param n Number of vertices
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_complete(int n, double min_weight, double max_weight);

/**
 * @brief Generate random grid graph
 *
 * Vertices arranged in rows x cols grid, edges to neighbors.
 *
 * @param rows Number of rows
 * @param cols Number of columns
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @param diagonal Include diagonal edges
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_grid(int rows, int cols, double min_weight,
                           double max_weight, bool diagonal);

/**
 * @brief Generate random tree
 *
 * @param n Number of vertices
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_tree(int n, double min_weight, double max_weight);

/**
 * @brief Generate random DAG
 *
 * Directed acyclic graph with topological ordering.
 *
 * @param n Number of vertices
 * @param m Number of edges
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_dag(int n, int m, double min_weight, double max_weight);

/**
 * @brief Generate random sparse graph
 *
 * Graph with O(n) edges.
 *
 * @param n Number of vertices
 * @param avg_degree Average vertex degree
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_sparse(int n, int avg_degree, double min_weight, double max_weight);

/**
 * @brief Generate random path graph
 *
 * Simple path from vertex 0 to n-1.
 *
 * @param n Number of vertices
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Pointer to generated graph, or NULL on error
 */
Graph* graph_generate_path(int n, double min_weight, double max_weight);

/** @} */

/**
 * @defgroup Utilities General Utilities
 * @brief Miscellaneous utility functions
 * @{
 */

/**
 * @brief Set random seed
 *
 * @param seed Random seed (0 for time-based)
 */
void utils_set_random_seed(unsigned int seed);

/**
 * @brief Get random integer in range [min, max]
 *
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Random integer
 */
int random_int(int min, int max);

/**
 * @brief Get random double in range [min, max]
 *
 * @param min Minimum value
 * @param max Maximum value
 * @return Random double
 */
double random_double(double min, double max);

/**
 * @brief Parse command line arguments for common graph parameters
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @param n Output: number of vertices
 * @param m Output: number of edges
 * @param source Output: source vertex
 * @return true if parsing successful, false otherwise
 */
bool parse_args(int argc, char** argv, int* n, int* m, int* source);

/**
 * @brief Print usage information
 *
 * @param program_name Program name (argv[0])
 */
void print_usage(const char* program_name);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SSSP_UTILS_H */
