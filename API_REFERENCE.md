# API Reference

Complete reference for the SSSP Breakthrough Algorithm Library API.

## Table of Contents

- [Overview](#overview)
- [Graph Management](#graph-management)
- [SSSP Solving](#sssp-solving)
- [Result Querying](#result-querying)
- [Graph Validation](#graph-validation)
- [File I/O](#file-io)
- [Utility Functions](#utility-functions)
- [Error Handling](#error-handling)
- [Constants and Types](#constants-and-types)

---

## Overview

The library provides a clean C99 API for computing single-source shortest paths using the breakthrough O(m log^(2/3) n) algorithm.

**Header File:** `#include <sssp.h>`
**Link:** `-lsssp -lm`

---

## Graph Management

### `sssp_graph_create`

```c
sssp_graph_t* sssp_graph_create(int num_vertices);
```

Creates a new graph with the specified number of vertices.

**Parameters:**
- `num_vertices`: Number of vertices (must be > 0 and ≤ SSSP_MAX_VERTICES)

**Returns:** Pointer to new graph, or NULL on failure

**Complexity:** O(n)

**Example:**
```c
sssp_graph_t* graph = sssp_graph_create(100);
if (!graph) {
    fprintf(stderr, "Failed to create graph\n");
    return -1;
}
```

---

### `sssp_graph_add_edge`

```c
sssp_status_t sssp_graph_add_edge(sssp_graph_t* graph, int from, int to, double weight);
```

Adds a directed edge to the graph.

**Parameters:**
- `graph`: Pointer to graph (must not be NULL)
- `from`: Source vertex ID (0 ≤ from < num_vertices)
- `to`: Target vertex ID (0 ≤ to < num_vertices)
- `weight`: Edge weight (can be negative, but no negative cycles allowed)

**Returns:** SSSP_SUCCESS or error code

**Complexity:** O(1)

**Example:**
```c
sssp_status_t status = sssp_graph_add_edge(graph, 0, 1, 4.5);
if (status != SSSP_SUCCESS) {
    fprintf(stderr, "Failed to add edge: %s\n", sssp_status_to_string(status));
}
```

---

### `sssp_graph_add_undirected_edge`

```c
sssp_status_t sssp_graph_add_undirected_edge(sssp_graph_t* graph, int u, int v, double weight);
```

Adds an undirected edge (adds both u→v and v→u).

**Parameters:**
- `graph`: Pointer to graph
- `u`, `v`: Vertex IDs
- `weight`: Edge weight

**Returns:** SSSP_SUCCESS or error code

**Complexity:** O(1)

---

### `sssp_graph_get_num_vertices`

```c
int sssp_graph_get_num_vertices(const sssp_graph_t* graph);
```

Returns the number of vertices in the graph.

**Returns:** Number of vertices, or -1 if graph is NULL

---

### `sssp_graph_get_num_edges`

```c
int sssp_graph_get_num_edges(const sssp_graph_t* graph);
```

Returns the number of edges in the graph.

**Returns:** Number of edges, or -1 if graph is NULL

---

### `sssp_graph_destroy`

```c
void sssp_graph_destroy(sssp_graph_t* graph);
```

Frees all memory associated with the graph.

**Parameters:**
- `graph`: Pointer to graph (NULL is allowed and ignored)

**Complexity:** O(n + m)

**Example:**
```c
sssp_graph_destroy(graph);
graph = NULL;  // Good practice
```

---

## SSSP Solving

### `sssp_solve`

```c
sssp_result_t* sssp_solve(sssp_graph_t* graph, int source);
```

Solves single-source shortest paths using the breakthrough algorithm.

**Parameters:**
- `graph`: Pointer to graph (must not be NULL)
- `source`: Source vertex ID (0 ≤ source < num_vertices)

**Returns:** Pointer to result structure, or NULL on failure

**Complexity:**
- Time: **O(m log^(2/3) n)** for large graphs (n ≥ 50)
- Time: O(m + n log n) for small graphs (automatic fallback to Dijkstra)
- Space: O(n + m)

**Example:**
```c
sssp_result_t* result = sssp_solve(graph, 0);
if (!result) {
    fprintf(stderr, "SSSP computation failed\n");
    return -1;
}

// Use result...

sssp_result_destroy(result);
```

**Thread Safety:** NOT thread-safe for the same graph. Can solve different graphs concurrently.

---

## Result Querying

### `sssp_result_get_status`

```c
sssp_status_t sssp_result_get_status(const sssp_result_t* result);
```

Returns the computation status.

**Returns:** Status code (SSSP_SUCCESS, SSSP_ERROR_*, etc.)

---

### `sssp_result_get_distance`

```c
double sssp_result_get_distance(const sssp_result_t* result, int vertex);
```

Returns the shortest distance from source to the specified vertex.

**Parameters:**
- `result`: Pointer to result
- `vertex`: Target vertex ID

**Returns:**
- Shortest distance if reachable
- SSSP_INFINITY if unreachable or invalid

**Complexity:** O(1)

**Example:**
```c
for (int v = 0; v < n; v++) {
    double dist = sssp_result_get_distance(result, v);
    if (dist == SSSP_INFINITY) {
        printf("Vertex %d is unreachable\n", v);
    } else {
        printf("Distance to %d: %.2f\n", v, dist);
    }
}
```

---

### `sssp_result_get_predecessor`

```c
int sssp_result_get_predecessor(const sssp_result_t* result, int vertex);
```

Returns the predecessor of a vertex in the shortest path tree.

**Returns:**
- Predecessor vertex ID
- SSSP_INVALID_VERTEX if no predecessor (source or unreachable)

**Complexity:** O(1)

---

### `sssp_result_get_source`

```c
int sssp_result_get_source(const sssp_result_t* result);
```

Returns the source vertex used in the computation.

**Returns:** Source vertex ID, or -1 if result is NULL

---

### `sssp_result_is_reachable`

```c
bool sssp_result_is_reachable(const sssp_result_t* result, int vertex);
```

Checks if a vertex is reachable from the source.

**Returns:** true if reachable, false otherwise

**Example:**
```c
if (sssp_result_is_reachable(result, 42)) {
    printf("Vertex 42 is reachable!\n");
}
```

---

### `sssp_result_get_path`

```c
sssp_status_t sssp_result_get_path(const sssp_result_t* result, int vertex,
                                    int* path, int* path_length);
```

Reconstructs the shortest path from source to vertex.

**Parameters:**
- `result`: Pointer to result
- `vertex`: Target vertex
- `path`: Array to store path (must have space for ≥ num_vertices elements)
- `path_length`: Output parameter for path length

**Returns:** SSSP_SUCCESS or error code

**Complexity:** O(path_length)

**Example:**
```c
int path[1000];
int path_len;

if (sssp_result_get_path(result, 99, path, &path_len) == SSSP_SUCCESS) {
    printf("Path: ");
    for (int i = 0; i < path_len; i++) {
        printf("%d%s", path[i], i < path_len - 1 ? " -> " : "\n");
    }
}
```

---

### `sssp_result_verify`

```c
bool sssp_result_verify(const sssp_result_t* result);
```

Verifies the correctness of the solution by checking triangle inequality for all edges.

**Returns:** true if solution is correct, false otherwise

**Complexity:** O(m)

**Example:**
```c
if (sssp_result_verify(result)) {
    printf("✓ Solution verified!\n");
} else {
    fprintf(stderr, "✗ Solution verification failed!\n");
}
```

---

### `sssp_result_destroy`

```c
void sssp_result_destroy(sssp_result_t* result);
```

Frees all memory associated with the result.

**Parameters:**
- `result`: Pointer to result (NULL is allowed and ignored)

---

## Graph Validation

### `sssp_graph_has_negative_cycle`

```c
bool sssp_graph_has_negative_cycle(sssp_graph_t* graph, int* cycle_vertices, int* cycle_length);
```

Detects negative cycles in the graph using Bellman-Ford.

**Parameters:**
- `graph`: Pointer to graph
- `cycle_vertices`: Optional array to store cycle vertices (can be NULL)
- `cycle_length`: Optional output parameter for cycle length (can be NULL)

**Returns:** true if negative cycle exists, false otherwise

**Complexity:** O(nm)

**Example:**
```c
int cycle[100];
int cycle_len;

if (sssp_graph_has_negative_cycle(graph, cycle, &cycle_len)) {
    printf("Negative cycle detected: ");
    for (int i = 0; i < cycle_len; i++) {
        printf("%d ", cycle[i]);
    }
    printf("\n");
}
```

---

### `sssp_graph_validate`

```c
sssp_status_t sssp_graph_validate(sssp_graph_t* graph, char* error_msg, size_t msg_size);
```

Validates graph structure and properties.

**Checks:**
- Negative cycles
- Negative self-loops
- Edge validity
- Graph consistency

**Parameters:**
- `graph`: Pointer to graph
- `error_msg`: Buffer for error message (can be NULL)
- `msg_size`: Size of error message buffer

**Returns:** SSSP_SUCCESS if valid, error code otherwise

**Example:**
```c
char error[256];
sssp_status_t status = sssp_graph_validate(graph, error, sizeof(error));

if (status != SSSP_SUCCESS) {
    fprintf(stderr, "Graph validation failed: %s\n", error);
    return -1;
}
```

---

## File I/O

### `sssp_graph_load_edge_list`

```c
sssp_graph_t* sssp_graph_load_edge_list(const char* filename, char* error_msg, size_t msg_size);
```

Loads graph from edge list format.

**File Format:**
```
num_vertices num_edges
from_vertex to_vertex weight
...
```

**Returns:** Pointer to loaded graph, or NULL on error

**Example:**
```c
char error[256];
sssp_graph_t* graph = sssp_graph_load_edge_list("graph.txt", error, sizeof(error));

if (!graph) {
    fprintf(stderr, "Failed to load graph: %s\n", error);
    return -1;
}
```

---

### `sssp_graph_save_edge_list`

```c
sssp_status_t sssp_graph_save_edge_list(const sssp_graph_t* graph, const char* filename);
```

Saves graph to edge list format.

**Returns:** SSSP_SUCCESS or error code

---

### `sssp_graph_load_dimacs`

```c
sssp_graph_t* sssp_graph_load_dimacs(const char* filename, char* error_msg, size_t msg_size);
```

Loads graph from DIMACS shortest path format.

**Returns:** Pointer to loaded graph, or NULL on error

---

### `sssp_result_export`

```c
sssp_status_t sssp_result_export(const sssp_result_t* result, const char* filename);
```

Exports SSSP results to a file.

**Format:**
```
source_vertex
vertex distance predecessor
...
```

**Returns:** SSSP_SUCCESS or error code

---

## Utility Functions

### `sssp_status_to_string`

```c
const char* sssp_status_to_string(sssp_status_t status);
```

Converts status code to human-readable string.

**Returns:** Static string describing the status

**Example:**
```c
sssp_status_t status = sssp_graph_add_edge(graph, 0, 1000, 1.0);
if (status != SSSP_SUCCESS) {
    fprintf(stderr, "Error: %s\n", sssp_status_to_string(status));
}
```

---

### `sssp_result_print`

```c
void sssp_result_print(const sssp_result_t* result);
```

Prints distances to all vertices (for debugging).

---

### `sssp_get_version`

```c
const char* sssp_get_version(void);
```

Returns library version string.

**Returns:** Version string (e.g., "1.0.0")

---

### `sssp_get_info`

```c
const char* sssp_get_info(void);
```

Returns algorithm information string.

**Returns:** Algorithm description

**Example:**
```c
printf("Using: %s\n", sssp_get_info());
// Output: "O(m log^(2/3) n) Breakthrough SSSP"
```

---

## Error Handling

### Status Codes

```c
typedef enum {
    SSSP_SUCCESS = 0,              // Operation successful
    SSSP_ERROR_NULL_POINTER,       // NULL pointer argument
    SSSP_ERROR_INVALID_VERTEX,     // Vertex ID out of range
    SSSP_ERROR_INVALID_EDGE,       // Invalid edge parameters
    SSSP_ERROR_MEMORY_ALLOCATION,  // Memory allocation failed
    SSSP_ERROR_GRAPH_TOO_LARGE,    // Graph exceeds size limits
    SSSP_ERROR_NEGATIVE_CYCLE,     // Negative cycle detected
    SSSP_ERROR_INVALID_GRAPH,      // Graph structure invalid
    SSSP_ERROR_NOT_SOLVED          // SSSP not yet solved
} sssp_status_t;
```

### Error Handling Pattern

```c
// Check every return value
sssp_graph_t* graph = sssp_graph_create(n);
if (!graph) {
    return handle_error("Failed to create graph");
}

sssp_status_t status = sssp_graph_add_edge(graph, u, v, w);
if (status != SSSP_SUCCESS) {
    sssp_graph_destroy(graph);
    return handle_error(sssp_status_to_string(status));
}

sssp_result_t* result = sssp_solve(graph, source);
if (!result || sssp_result_get_status(result) != SSSP_SUCCESS) {
    sssp_graph_destroy(graph);
    return handle_error("SSSP computation failed");
}

// Use result...

// Always cleanup
sssp_result_destroy(result);
sssp_graph_destroy(graph);
```

---

## Constants and Types

### Constants

```c
#define SSSP_MAX_VERTICES 100000      // Maximum vertices
#define SSSP_MAX_EDGES 1000000        // Maximum edges
#define SSSP_INFINITY 1e308           // Infinity value
#define SSSP_INVALID_VERTEX -1        // Invalid vertex ID
```

### Opaque Types

```c
typedef struct sssp_graph sssp_graph_t;      // Graph structure
typedef struct sssp_result sssp_result_t;    // Result structure
```

These are opaque types - their internal structure is hidden. Always use API functions to access them.

---

## Thread Safety

### Safety Guarantees

| Operation | Thread Safety |
|-----------|--------------|
| Graph creation/destruction | ❌ Not thread-safe |
| Graph modification | ❌ Not thread-safe |
| SSSP solving (different graphs) | ✅ Thread-safe |
| SSSP solving (same graph) | ❌ Not thread-safe |
| Result querying (read-only) | ✅ Thread-safe |
| Result destruction | ❌ Not thread-safe |

### Safe Concurrent Pattern

```c
// Thread A
sssp_graph_t* graph_a = sssp_graph_create(100);
// ... add edges ...
sssp_result_t* result_a = sssp_solve(graph_a, 0);  // ✅ OK

// Thread B (concurrent with Thread A)
sssp_graph_t* graph_b = sssp_graph_create(200);
// ... add edges ...
sssp_result_t* result_b = sssp_solve(graph_b, 0);  // ✅ OK

// Unsafe: Two threads solving on same graph
// sssp_solve(same_graph, 0);  // ❌ NOT SAFE
```

---

## Memory Management

### Ownership Rules

1. **Graphs created with `sssp_graph_create()`** must be destroyed with `sssp_graph_destroy()`
2. **Results returned by `sssp_solve()`** must be destroyed with `sssp_result_destroy()`
3. **Strings returned by API** (version, status, info) are static - don't free them
4. **User owns arrays** passed to API (path arrays, error message buffers)

### Memory Safety

- All allocations are checked
- NULL arguments are safely handled
- Magic numbers validate structure integrity
- No memory leaks (verified with Valgrind)

---

## Performance Characteristics

### Time Complexity Summary

| Operation | Complexity |
|-----------|------------|
| `sssp_graph_create` | O(n) |
| `sssp_graph_add_edge` | O(1) |
| `sssp_solve` | **O(m log^(2/3) n)** |
| `sssp_result_get_distance` | O(1) |
| `sssp_result_get_path` | O(path length) |
| `sssp_result_verify` | O(m) |
| `sssp_graph_has_negative_cycle` | O(nm) |

### Space Complexity

- Graph storage: O(n + m)
- Algorithm state: O(n)
- Total: O(n + m)

---

For complete examples and usage patterns, see [EXAMPLES.md](EXAMPLES.md).

For implementation details, see [TECHNICAL_ANALYSIS.md](TECHNICAL_ANALYSIS.md).

**Last Updated**: 2025-11-12
