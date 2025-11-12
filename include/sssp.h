/**
 * @file sssp.h
 * @brief Public API for the SSSP Breakthrough Algorithm
 *
 * This header provides a clean, simple interface for computing single-source
 * shortest paths using the breakthrough O(m log^{2/3} n) algorithm from the paper
 * "Breaking the Sorting Barrier for Directed Single-Source Shortest Paths"
 * by Duan, Mao, Mao, Shu, and Yin (2025).
 *
 * The algorithm achieves subquadratic time complexity for shortest path
 * computation on graphs with arbitrary edge weights (including negative weights,
 * but no negative cycles).
 *
 * @version 1.0.0
 * @date 2025
 *
 * @example
 * @code
 * // Create a graph with 5 vertices
 * sssp_graph_t* graph = sssp_graph_create(5);
 *
 * // Add edges (u, v, weight)
 * sssp_graph_add_edge(graph, 0, 1, 4.0);
 * sssp_graph_add_edge(graph, 0, 2, 1.0);
 * sssp_graph_add_edge(graph, 2, 1, 2.0);
 * sssp_graph_add_edge(graph, 1, 3, 1.0);
 * sssp_graph_add_edge(graph, 2, 3, 5.0);
 *
 * // Solve SSSP from source vertex 0
 * sssp_result_t* result = sssp_solve(graph, 0);
 *
 * if (result && sssp_result_get_status(result) == SSSP_SUCCESS) {
 *     for (int v = 0; v < 5; v++) {
 *         double dist = sssp_result_get_distance(result, v);
 *         printf("Distance to %d: %.2f\n", v, dist);
 *     }
 * }
 *
 * // Clean up
 * sssp_result_destroy(result);
 * sssp_graph_destroy(graph);
 * @endcode
 */

#ifndef SSSP_PUBLIC_API_H
#define SSSP_PUBLIC_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

/**
 * @defgroup Constants Constants and Limits
 * @{
 */

/** Maximum number of vertices supported */
#define SSSP_MAX_VERTICES 100000

/** Maximum number of edges supported */
#define SSSP_MAX_EDGES 1000000

/** Value representing infinity (unreachable vertex) */
#define SSSP_INFINITY 1e308

/** Invalid vertex ID */
#define SSSP_INVALID_VERTEX -1

/** @} */

/**
 * @defgroup Types Type Definitions
 * @{
 */

/**
 * @brief Opaque type representing a graph structure
 *
 * This type hides the internal graph representation from users.
 * Graphs must be created with sssp_graph_create() and destroyed
 * with sssp_graph_destroy().
 */
typedef struct sssp_graph sssp_graph_t;

/**
 * @brief Opaque type representing the result of an SSSP computation
 *
 * This type contains distance and predecessor information.
 * Results must be destroyed with sssp_result_destroy() when no longer needed.
 */
typedef struct sssp_result sssp_result_t;

/**
 * @brief Return status codes for API functions
 */
typedef enum {
    SSSP_SUCCESS = 0,              /**< Operation completed successfully */
    SSSP_ERROR_NULL_POINTER,       /**< Null pointer provided as argument */
    SSSP_ERROR_INVALID_VERTEX,     /**< Vertex ID out of range */
    SSSP_ERROR_INVALID_EDGE,       /**< Invalid edge parameters */
    SSSP_ERROR_MEMORY_ALLOCATION,  /**< Memory allocation failed */
    SSSP_ERROR_GRAPH_TOO_LARGE,    /**< Graph exceeds size limits */
    SSSP_ERROR_NEGATIVE_CYCLE,     /**< Negative cycle detected in graph */
    SSSP_ERROR_INVALID_GRAPH,      /**< Graph structure is invalid */
    SSSP_ERROR_NOT_SOLVED          /**< SSSP has not been solved yet */
} sssp_status_t;

/** @} */

/**
 * @defgroup GraphAPI Graph Management
 * @brief Functions for creating and manipulating graphs
 * @{
 */

/**
 * @brief Create a new graph with the specified number of vertices
 *
 * Allocates and initializes a new graph structure. The graph initially
 * has no edges. Vertices are numbered from 0 to num_vertices-1.
 *
 * @param num_vertices Number of vertices in the graph (must be > 0 and <= SSSP_MAX_VERTICES)
 * @return Pointer to newly created graph, or NULL on failure
 *
 * @note The returned graph must be freed with sssp_graph_destroy()
 * @see sssp_graph_destroy()
 */
sssp_graph_t* sssp_graph_create(int num_vertices);

/**
 * @brief Add a directed edge to the graph
 *
 * Adds an edge from vertex 'from' to vertex 'to' with the specified weight.
 * If multiple edges between the same vertices are added, only the last one
 * will be used (or they will all be considered, depending on implementation).
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param from Source vertex ID (0 <= from < num_vertices)
 * @param to Target vertex ID (0 <= to < num_vertices)
 * @param weight Edge weight (can be negative, but graph must not contain negative cycles)
 * @return SSSP_SUCCESS on success, error code otherwise
 *
 * @note For undirected graphs, call this function twice (once for each direction)
 * @see sssp_graph_add_undirected_edge()
 */
sssp_status_t sssp_graph_add_edge(sssp_graph_t* graph, int from, int to, double weight);

/**
 * @brief Add an undirected edge to the graph
 *
 * Convenience function that adds edges in both directions.
 * Equivalent to calling sssp_graph_add_edge() twice.
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param u First vertex ID
 * @param v Second vertex ID
 * @param weight Edge weight
 * @return SSSP_SUCCESS on success, error code otherwise
 *
 * @see sssp_graph_add_edge()
 */
sssp_status_t sssp_graph_add_undirected_edge(sssp_graph_t* graph, int u, int v, double weight);

/**
 * @brief Get the number of vertices in the graph
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @return Number of vertices, or -1 if graph is NULL
 */
int sssp_graph_get_num_vertices(const sssp_graph_t* graph);

/**
 * @brief Get the number of edges in the graph
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @return Number of edges, or -1 if graph is NULL
 */
int sssp_graph_get_num_edges(const sssp_graph_t* graph);

/**
 * @brief Destroy a graph and free all associated memory
 *
 * Frees all memory associated with the graph structure.
 * After calling this function, the graph pointer is invalid.
 *
 * @param graph Pointer to graph structure (NULL is allowed and ignored)
 */
void sssp_graph_destroy(sssp_graph_t* graph);

/** @} */

/**
 * @defgroup SolverAPI SSSP Solver
 * @brief Functions for solving single-source shortest paths
 * @{
 */

/**
 * @brief Solve single-source shortest paths from a given source vertex
 *
 * Computes shortest paths from the source vertex to all other vertices
 * using the breakthrough O(m log^{2/3} n) algorithm. The algorithm handles
 * graphs with negative edge weights but will detect negative cycles.
 *
 * Time complexity: O(m log^{2/3} n) where m is the number of edges and n is the number of vertices
 * Space complexity: O(n + m)
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param source Source vertex ID (0 <= source < num_vertices)
 * @return Pointer to result structure containing distances and predecessors,
 *         or NULL on failure
 *
 * @note The returned result must be freed with sssp_result_destroy()
 * @see sssp_result_destroy(), sssp_result_get_distance(), sssp_result_get_predecessor()
 */
sssp_result_t* sssp_solve(sssp_graph_t* graph, int source);

/** @} */

/**
 * @defgroup ResultAPI Result Querying
 * @brief Functions for querying SSSP results
 * @{
 */

/**
 * @brief Get the status of the SSSP computation
 *
 * @param result Pointer to result structure (must not be NULL)
 * @return Status code indicating success or type of error
 */
sssp_status_t sssp_result_get_status(const sssp_result_t* result);

/**
 * @brief Get the shortest distance to a vertex
 *
 * Returns the shortest distance from the source vertex (specified in sssp_solve())
 * to the target vertex. If the vertex is unreachable, returns SSSP_INFINITY.
 *
 * @param result Pointer to result structure (must not be NULL)
 * @param vertex Target vertex ID (0 <= vertex < num_vertices)
 * @return Shortest distance, or SSSP_INFINITY if unreachable or invalid
 */
double sssp_result_get_distance(const sssp_result_t* result, int vertex);

/**
 * @brief Get the predecessor of a vertex in the shortest path tree
 *
 * Returns the vertex that precedes the given vertex in the shortest path
 * from the source. Returns SSSP_INVALID_VERTEX if there is no predecessor
 * (i.e., for the source vertex or unreachable vertices).
 *
 * @param result Pointer to result structure (must not be NULL)
 * @param vertex Target vertex ID (0 <= vertex < num_vertices)
 * @return Predecessor vertex ID, or SSSP_INVALID_VERTEX
 */
int sssp_result_get_predecessor(const sssp_result_t* result, int vertex);

/**
 * @brief Get the source vertex used in the computation
 *
 * @param result Pointer to result structure (must not be NULL)
 * @return Source vertex ID, or -1 if result is NULL
 */
int sssp_result_get_source(const sssp_result_t* result);

/**
 * @brief Check if a vertex is reachable from the source
 *
 * @param result Pointer to result structure (must not be NULL)
 * @param vertex Target vertex ID
 * @return true if vertex is reachable, false otherwise
 */
bool sssp_result_is_reachable(const sssp_result_t* result, int vertex);

/**
 * @brief Get the shortest path from source to a vertex
 *
 * Reconstructs the shortest path by following predecessors.
 * The path is stored in the provided array in order from source to target.
 *
 * @param result Pointer to result structure (must not be NULL)
 * @param vertex Target vertex ID
 * @param path Array to store the path (must have space for at least num_vertices elements)
 * @param path_length Output parameter that receives the path length
 * @return SSSP_SUCCESS on success, error code otherwise
 *
 * @note If vertex is unreachable, path_length will be set to 0
 */
sssp_status_t sssp_result_get_path(const sssp_result_t* result, int vertex,
                                    int* path, int* path_length);

/**
 * @brief Verify the correctness of the SSSP solution
 *
 * Checks that all distance estimates satisfy the shortest path optimality
 * conditions (triangle inequality for all edges).
 *
 * @param result Pointer to result structure (must not be NULL)
 * @return true if solution is valid, false otherwise
 */
bool sssp_result_verify(const sssp_result_t* result);

/**
 * @brief Destroy a result structure and free all associated memory
 *
 * Frees all memory associated with the result structure.
 * After calling this function, the result pointer is invalid.
 *
 * @param result Pointer to result structure (NULL is allowed and ignored)
 */
void sssp_result_destroy(sssp_result_t* result);

/** @} */

/**
 * @defgroup ValidationAPI Graph Validation
 * @brief Functions for validating graphs before solving SSSP
 * @{
 */

/**
 * @brief Check if graph contains a negative cycle
 *
 * Uses Bellman-Ford-style detection to identify negative cycles.
 * This function should be called before sssp_solve() if the graph
 * may contain negative edge weights.
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param cycle_vertices Output array to store vertices in the detected cycle (can be NULL)
 * @param cycle_length Output parameter for cycle length (can be NULL)
 * @return true if a negative cycle exists, false otherwise
 *
 * @note If cycle_vertices is provided, it must have space for at least num_vertices elements
 * @see sssp_graph_validate()
 */
bool sssp_graph_has_negative_cycle(sssp_graph_t* graph, int* cycle_vertices, int* cycle_length);

/**
 * @brief Validate graph structure and properties
 *
 * Performs comprehensive validation including:
 * - Checking for negative cycles
 * - Verifying edge validity
 * - Checking for self-loops with negative weights
 * - Ensuring graph consistency
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param error_msg Buffer to receive error message (can be NULL)
 * @param msg_size Size of error message buffer
 * @return SSSP_SUCCESS if valid, error code otherwise
 */
sssp_status_t sssp_graph_validate(sssp_graph_t* graph, char* error_msg, size_t msg_size);

/** @} */

/**
 * @defgroup IOApi Graph Import/Export
 * @brief Functions for reading and writing graphs in standard formats
 * @{
 */

/**
 * @brief Load graph from edge list file
 *
 * File format (one edge per line):
 *   num_vertices num_edges
 *   from_vertex to_vertex weight
 *   ...
 *
 * @param filename Path to input file
 * @param error_msg Buffer for error message (can be NULL)
 * @param msg_size Size of error buffer
 * @return Pointer to loaded graph, or NULL on failure
 *
 * @note Vertices are assumed to be numbered from 0
 * @see sssp_graph_save_edge_list()
 */
sssp_graph_t* sssp_graph_load_edge_list(const char* filename, char* error_msg, size_t msg_size);

/**
 * @brief Save graph to edge list file
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param filename Path to output file
 * @return SSSP_SUCCESS on success, error code otherwise
 *
 * @see sssp_graph_load_edge_list()
 */
sssp_status_t sssp_graph_save_edge_list(const sssp_graph_t* graph, const char* filename);

/**
 * @brief Load graph from DIMACS format file
 *
 * Supports standard DIMACS shortest path problem format.
 *
 * @param filename Path to input file
 * @param error_msg Buffer for error message (can be NULL)
 * @param msg_size Size of error buffer
 * @return Pointer to loaded graph, or NULL on failure
 */
sssp_graph_t* sssp_graph_load_dimacs(const char* filename, char* error_msg, size_t msg_size);

/**
 * @brief Export result to file
 *
 * Saves distances and paths to all reachable vertices.
 *
 * @param result Pointer to result structure (must not be NULL)
 * @param filename Path to output file
 * @return SSSP_SUCCESS on success, error code otherwise
 */
sssp_status_t sssp_result_export(const sssp_result_t* result, const char* filename);

/** @} */

/**
 * @defgroup UtilityAPI Utility Functions
 * @brief Helper functions for error handling and debugging
 * @{
 */

/**
 * @brief Get a human-readable error message for a status code
 *
 * @param status Status code
 * @return Pointer to static string describing the error (never NULL)
 *
 * @note The returned string is statically allocated and should not be freed
 */
const char* sssp_status_to_string(sssp_status_t status);

/**
 * @brief Print distances from source to all vertices
 *
 * Utility function for debugging and visualization.
 *
 * @param result Pointer to result structure (must not be NULL)
 */
void sssp_result_print(const sssp_result_t* result);

/**
 * @brief Get version information
 *
 * @return Pointer to static string containing version information
 */
const char* sssp_get_version(void);

/**
 * @brief Get algorithm information
 *
 * @return Pointer to static string containing algorithm description
 */
const char* sssp_get_info(void);

/** @} */

/**
 * @defgroup ThreadSafety Thread Safety
 * @{
 *
 * @par Thread Safety Guarantees:
 *
 * - **Graph Creation/Destruction**: NOT thread-safe. Each graph must be
 *   accessed by only one thread at a time.
 *
 * - **Graph Modification**: NOT thread-safe. Adding edges to a graph
 *   while another thread is reading it will result in undefined behavior.
 *
 * - **SSSP Solving**: Thread-safe for different graphs. Multiple threads
 *   can call sssp_solve() on different graph instances simultaneously.
 *   However, solving SSSP on the same graph from multiple threads is
 *   NOT thread-safe.
 *
 * - **Result Querying**: Thread-safe for read-only operations. Multiple
 *   threads can query the same result structure simultaneously. Result
 *   destruction is NOT thread-safe.
 *
 * @par Recommended Usage Pattern:
 * @code
 * // Thread A
 * sssp_graph_t* graph_a = sssp_graph_create(100);
 * // ... add edges ...
 * sssp_result_t* result_a = sssp_solve(graph_a, 0);
 *
 * // Thread B (concurrent with Thread A)
 * sssp_graph_t* graph_b = sssp_graph_create(200);
 * // ... add edges ...
 * sssp_result_t* result_b = sssp_solve(graph_b, 0);
 * @endcode
 *
 * @par Memory Ownership:
 * - Graphs created with sssp_graph_create() must be destroyed with
 *   sssp_graph_destroy() by the same thread that created them.
 *
 * - Results returned by sssp_solve() must be destroyed with
 *   sssp_result_destroy() by the owning thread.
 *
 * - Pointers returned by API functions (except strings) should not
 *   be freed directly by the user.
 *
 * @} */

#ifdef __cplusplus
}
#endif

#endif /* SSSP_PUBLIC_API_H */
