# SSSP Public API Usage Guide

This guide demonstrates how to use the clean public API for the SSSP Breakthrough algorithm.

## Table of Contents

- [Quick Start](#quick-start)
- [API Overview](#api-overview)
- [Detailed Examples](#detailed-examples)
- [Error Handling](#error-handling)
- [Integration Guide](#integration-guide)
- [Best Practices](#best-practices)

## Quick Start

### Basic Example

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    // Create a graph with 5 vertices
    sssp_graph_t* graph = sssp_graph_create(5);
    if (!graph) {
        fprintf(stderr, "Failed to create graph\n");
        return 1;
    }

    // Add edges (directed)
    sssp_graph_add_edge(graph, 0, 1, 4.0);
    sssp_graph_add_edge(graph, 0, 2, 1.0);
    sssp_graph_add_edge(graph, 2, 1, 2.0);
    sssp_graph_add_edge(graph, 1, 3, 1.0);
    sssp_graph_add_edge(graph, 2, 3, 5.0);
    sssp_graph_add_edge(graph, 3, 4, 3.0);

    // Solve SSSP from source vertex 0
    sssp_result_t* result = sssp_solve(graph, 0);
    if (!result) {
        fprintf(stderr, "Failed to solve SSSP\n");
        sssp_graph_destroy(graph);
        return 1;
    }

    // Check if solution is valid
    if (sssp_result_get_status(result) != SSSP_SUCCESS) {
        fprintf(stderr, "SSSP failed: %s\n",
                sssp_status_to_string(sssp_result_get_status(result)));
        sssp_result_destroy(result);
        sssp_graph_destroy(graph);
        return 1;
    }

    // Print distances
    printf("Shortest distances from vertex 0:\n");
    for (int v = 0; v < 5; v++) {
        double dist = sssp_result_get_distance(result, v);
        if (dist == SSSP_INFINITY) {
            printf("  Vertex %d: unreachable\n", v);
        } else {
            printf("  Vertex %d: %.2f\n", v, dist);
        }
    }

    // Clean up
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

### Compile and Run

```bash
# Using the installed library
gcc -o example example.c -lsssp -lm

# Or with explicit include path
gcc -I/path/to/include -o example example.c -lsssp -lm

# Run
./example
```

## API Overview

### Graph Management Functions

| Function | Description |
|----------|-------------|
| `sssp_graph_create(n)` | Create a graph with n vertices |
| `sssp_graph_add_edge(g, u, v, w)` | Add directed edge from u to v with weight w |
| `sssp_graph_add_undirected_edge(g, u, v, w)` | Add undirected edge between u and v |
| `sssp_graph_get_num_vertices(g)` | Get number of vertices |
| `sssp_graph_get_num_edges(g)` | Get number of edges |
| `sssp_graph_destroy(g)` | Free graph memory |

### SSSP Solver Functions

| Function | Description |
|----------|-------------|
| `sssp_solve(g, source)` | Solve SSSP from source vertex |
| `sssp_solve_advanced(g, source, k, t)` | Solve with custom parameters |

### Result Query Functions

| Function | Description |
|----------|-------------|
| `sssp_result_get_status(r)` | Get computation status |
| `sssp_result_get_distance(r, v)` | Get distance to vertex v |
| `sssp_result_get_predecessor(r, v)` | Get predecessor of vertex v |
| `sssp_result_get_source(r)` | Get source vertex |
| `sssp_result_is_reachable(r, v)` | Check if v is reachable |
| `sssp_result_get_path(r, v, path, len)` | Get shortest path to v |
| `sssp_result_verify(r)` | Verify solution correctness |
| `sssp_result_print(r)` | Print all distances |
| `sssp_result_destroy(r)` | Free result memory |

### Utility Functions

| Function | Description |
|----------|-------------|
| `sssp_status_to_string(status)` | Convert status code to string |
| `sssp_get_version()` | Get library version |
| `sssp_get_info()` | Get algorithm information |

## Detailed Examples

### Example 1: Undirected Graph

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    // Create graph for undirected edges
    sssp_graph_t* graph = sssp_graph_create(4);

    // Add undirected edges (adds both directions)
    sssp_graph_add_undirected_edge(graph, 0, 1, 1.0);
    sssp_graph_add_undirected_edge(graph, 1, 2, 2.0);
    sssp_graph_add_undirected_edge(graph, 2, 3, 1.0);
    sssp_graph_add_undirected_edge(graph, 0, 3, 5.0);

    // Solve from vertex 0
    sssp_result_t* result = sssp_solve(graph, 0);

    // Print results
    sssp_result_print(result);

    // Clean up
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

### Example 2: Path Reconstruction

```c
#include <sssp.h>
#include <stdio.h>

void print_path(sssp_result_t* result, int target) {
    int path[SSSP_MAX_VERTICES];
    int path_length;

    sssp_status_t status = sssp_result_get_path(result, target, path, &path_length);

    if (status != SSSP_SUCCESS) {
        printf("Failed to get path to vertex %d\n", target);
        return;
    }

    if (path_length == 0) {
        printf("Vertex %d is unreachable\n", target);
        return;
    }

    printf("Path to vertex %d: ", target);
    for (int i = 0; i < path_length; i++) {
        printf("%d", path[i]);
        if (i < path_length - 1) printf(" -> ");
    }
    printf(" (distance: %.2f)\n", sssp_result_get_distance(result, target));
}

int main(void) {
    sssp_graph_t* graph = sssp_graph_create(5);

    // Build a simple path graph: 0 -> 1 -> 2 -> 3 -> 4
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 2.0);
    sssp_graph_add_edge(graph, 2, 3, 3.0);
    sssp_graph_add_edge(graph, 3, 4, 4.0);

    sssp_result_t* result = sssp_solve(graph, 0);

    // Print paths to all vertices
    for (int v = 0; v < 5; v++) {
        print_path(result, v);
    }

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

### Example 3: Negative Weights

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    sssp_graph_t* graph = sssp_graph_create(4);

    // Graph with negative edge weights (but no negative cycles)
    sssp_graph_add_edge(graph, 0, 1, 5.0);
    sssp_graph_add_edge(graph, 0, 2, 2.0);
    sssp_graph_add_edge(graph, 1, 2, -3.0);  // Negative weight
    sssp_graph_add_edge(graph, 2, 3, 1.0);

    sssp_result_t* result = sssp_solve(graph, 0);

    if (sssp_result_get_status(result) == SSSP_ERROR_NEGATIVE_CYCLE) {
        printf("Negative cycle detected!\n");
    } else {
        printf("Distances (with negative weights):\n");
        for (int v = 0; v < 4; v++) {
            printf("  Vertex %d: %.2f\n", v, sssp_result_get_distance(result, v));
        }

        // Verify the solution
        if (sssp_result_verify(result)) {
            printf("\nSolution verified successfully!\n");
        }
    }

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

### Example 4: Checking Reachability

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    // Graph with disconnected components
    sssp_graph_t* graph = sssp_graph_create(6);

    // Component 1: vertices 0, 1, 2
    sssp_graph_add_edge(graph, 0, 1, 1.0);
    sssp_graph_add_edge(graph, 1, 2, 1.0);

    // Component 2: vertices 3, 4, 5 (disconnected from component 1)
    sssp_graph_add_edge(graph, 3, 4, 1.0);
    sssp_graph_add_edge(graph, 4, 5, 1.0);

    sssp_result_t* result = sssp_solve(graph, 0);

    printf("Reachability from vertex 0:\n");
    for (int v = 0; v < 6; v++) {
        if (sssp_result_is_reachable(result, v)) {
            printf("  Vertex %d: REACHABLE (distance: %.2f)\n",
                   v, sssp_result_get_distance(result, v));
        } else {
            printf("  Vertex %d: UNREACHABLE\n", v);
        }
    }

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

## Error Handling

### Proper Error Checking

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    sssp_graph_t* graph = NULL;
    sssp_result_t* result = NULL;
    sssp_status_t status;

    // Create graph with error checking
    graph = sssp_graph_create(100);
    if (!graph) {
        fprintf(stderr, "Error: Failed to create graph\n");
        return 1;
    }

    // Add edge with error checking
    status = sssp_graph_add_edge(graph, 0, 1, 1.0);
    if (status != SSSP_SUCCESS) {
        fprintf(stderr, "Error adding edge: %s\n", sssp_status_to_string(status));
        sssp_graph_destroy(graph);
        return 1;
    }

    // Invalid edge (out of range)
    status = sssp_graph_add_edge(graph, 0, 200, 1.0);
    if (status == SSSP_ERROR_INVALID_VERTEX) {
        printf("Caught invalid vertex error (expected)\n");
    }

    // Solve with error checking
    result = sssp_solve(graph, 0);
    if (!result) {
        fprintf(stderr, "Error: Failed to solve SSSP\n");
        sssp_graph_destroy(graph);
        return 1;
    }

    // Check computation status
    status = sssp_result_get_status(result);
    if (status != SSSP_SUCCESS) {
        fprintf(stderr, "Error: SSSP computation failed: %s\n",
                sssp_status_to_string(status));
        sssp_result_destroy(result);
        sssp_graph_destroy(graph);
        return 1;
    }

    printf("SSSP computed successfully!\n");

    // Clean up
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

### All Error Codes

```c
SSSP_SUCCESS                  // Success
SSSP_ERROR_NULL_POINTER       // NULL pointer argument
SSSP_ERROR_INVALID_VERTEX     // Vertex ID out of range
SSSP_ERROR_INVALID_EDGE       // Invalid edge parameters
SSSP_ERROR_MEMORY_ALLOCATION  // Memory allocation failed
SSSP_ERROR_GRAPH_TOO_LARGE    // Graph exceeds limits
SSSP_ERROR_NEGATIVE_CYCLE     // Negative cycle detected
SSSP_ERROR_INVALID_GRAPH      // Invalid graph structure
SSSP_ERROR_NOT_SOLVED         // SSSP not yet solved
```

## Integration Guide

### Using as a Library

#### Option 1: System-wide Installation

```bash
# Build and install
mkdir build && cd build
cmake ..
make
sudo make install

# Use in your project
gcc -o myapp myapp.c -lsssp -lm
```

#### Option 2: Local Integration

```bash
# Copy the header to your project
cp include/sssp.h myproject/include/

# Link against the implementation
gcc -I./include -o myapp myapp.c /path/to/sssp/lib/libsssp.a -lm
```

#### Option 3: CMake Integration

In your `CMakeLists.txt`:

```cmake
# Find the SSSP library
find_path(SSSP_INCLUDE_DIR sssp.h)
find_library(SSSP_LIBRARY sssp)

# Add to your target
include_directories(${SSSP_INCLUDE_DIR})
target_link_libraries(myapp ${SSSP_LIBRARY} m)
```

### Minimal Example Project Structure

```
myproject/
├── CMakeLists.txt
├── src/
│   └── main.c
└── include/
    └── sssp.h  (copied from SSSP project)
```

## Best Practices

### Memory Management

```c
// Always initialize pointers to NULL
sssp_graph_t* graph = NULL;
sssp_result_t* result = NULL;

// Create resources
graph = sssp_graph_create(100);
result = sssp_solve(graph, 0);

// ALWAYS clean up in reverse order
if (result) sssp_result_destroy(result);
if (graph) sssp_graph_destroy(graph);
```

### Performance Tips

1. **Pre-allocate graphs**: If you know the graph size, create it once
2. **Reuse graphs**: You can solve SSSP multiple times from different sources
3. **Verify sparingly**: `sssp_result_verify()` is O(m), use only for debugging
4. **Batch edge additions**: Add all edges before solving

### Thread Safety

The current API is **not thread-safe**. If using in a multi-threaded environment:

```c
// Create separate graph instances per thread
#pragma omp parallel
{
    sssp_graph_t* my_graph = sssp_graph_create(n);
    // ... use graph ...
    sssp_graph_destroy(my_graph);
}
```

### Debugging

```c
// Enable verbose output (if compiled with DEBUG)
#ifdef DEBUG
    sssp_result_print(result);

    if (!sssp_result_verify(result)) {
        fprintf(stderr, "Warning: Solution verification failed!\n");
    }
#endif
```

## Advanced Usage

### Custom Parameters (Advanced)

```c
// Use advanced solver with custom parameters
// k: recursion depth parameter
// t: time budget parameter
sssp_result_t* result = sssp_solve_advanced(graph, source, k, t);

// Most users should stick with sssp_solve() which auto-tunes these
```

### Large Graphs

```c
// For very large graphs, check limits
int n = 50000;  // vertices
int m = 500000; // edges

if (n > SSSP_MAX_VERTICES || m > SSSP_MAX_EDGES) {
    fprintf(stderr, "Graph exceeds limits\n");
    // Consider using sparse representation or splitting the graph
}
```

## Getting Help

- API Reference: See header file `include/sssp.h`
- Algorithm Details: See `TECHNICAL_ANALYSIS.md`
- Project README: See `README.md`
- Version Info: Call `sssp_get_version()` and `sssp_get_info()`

## Version Information

```c
printf("SSSP Library Version: %s\n", sssp_get_version());
printf("Algorithm: %s\n", sssp_get_info());
```
