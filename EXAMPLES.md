# Examples and Usage Patterns

This guide provides comprehensive examples of using the SSSP Breakthrough Algorithm library.

## Table of Contents

- [Basic Usage](#basic-usage)
- [Advanced Examples](#advanced-examples)
- [Integration Patterns](#integration-patterns)
- [Performance Tuning](#performance-tuning)
- [Common Pitfalls](#common-pitfalls)

---

## Basic Usage

### Example 1: Hello World SSSP

The simplest possible example:

```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    // Create graph with 5 vertices
    sssp_graph_t* graph = sssp_graph_create(5);

    // Add edges: 0->1, 0->2, 1->3, 2->3, 3->4
    sssp_graph_add_edge(graph, 0, 1, 4.0);
    sssp_graph_add_edge(graph, 0, 2, 1.0);
    sssp_graph_add_edge(graph, 1, 3, 1.0);
    sssp_graph_add_edge(graph, 2, 3, 5.0);
    sssp_graph_add_edge(graph, 3, 4, 3.0);

    // Solve SSSP from vertex 0
    sssp_result_t* result = sssp_solve(graph, 0);

    // Print distances
    for (int v = 0; v < 5; v++) {
        printf("Distance to %d: %.2f\n", v,
               sssp_result_get_distance(result, v));
    }

    // Cleanup
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

**Output:**
```
Distance to 0: 0.00
Distance to 1: 4.00
Distance to 2: 1.00
Distance to 3: 5.00
Distance to 4: 8.00
```

### Example 2: Loading Graph from File

```c
#include <sssp.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return 1;
    }

    // Load graph from file
    char error_msg[256];
    sssp_graph_t* graph = sssp_graph_load_edge_list(argv[1], error_msg, sizeof(error_msg));

    if (!graph) {
        fprintf(stderr, "Error loading graph: %s\n", error_msg);
        return 1;
    }

    printf("Loaded graph: %d vertices, %d edges\n",
           sssp_graph_get_num_vertices(graph),
           sssp_graph_get_num_edges(graph));

    // Solve SSSP
    sssp_result_t* result = sssp_solve(graph, 0);

    if (result && sssp_result_verify(result)) {
        printf("✓ Solution computed and verified!\n");
    }

    // Cleanup
    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

**Graph file format (graph.txt):**
```
5 6
0 1 4.0
0 2 1.0
1 3 1.0
2 3 5.0
3 4 3.0
2 1 2.0
```

### Example 3: Path Reconstruction

```c
#include <sssp.h>
#include <stdio.h>

void print_path(sssp_result_t* result, int target) {
    int path[1000];
    int path_length;

    sssp_status_t status = sssp_result_get_path(result, target, path, &path_length);

    if (status != SSSP_SUCCESS || path_length == 0) {
        printf("No path to vertex %d\n", target);
        return;
    }

    printf("Path to vertex %d: ", target);
    for (int i = 0; i < path_length; i++) {
        printf("%d", path[i]);
        if (i < path_length - 1) {
            printf(" -> ");
        }
    }
    printf(" (distance: %.2f)\n", sssp_result_get_distance(result, target));
}

int main(void) {
    sssp_graph_t* graph = sssp_graph_create(6);

    // Build graph
    sssp_graph_add_edge(graph, 0, 1, 2.0);
    sssp_graph_add_edge(graph, 0, 2, 4.0);
    sssp_graph_add_edge(graph, 1, 2, 1.0);
    sssp_graph_add_edge(graph, 1, 3, 7.0);
    sssp_graph_add_edge(graph, 2, 4, 3.0);
    sssp_graph_add_edge(graph, 3, 5, 1.0);
    sssp_graph_add_edge(graph, 4, 3, 2.0);
    sssp_graph_add_edge(graph, 4, 5, 5.0);

    sssp_result_t* result = sssp_solve(graph, 0);

    // Print paths to all vertices
    for (int v = 0; v < 6; v++) {
        print_path(result, v);
    }

    sssp_result_destroy(result);
    sssp_graph_destroy(graph);

    return 0;
}
```

---

## Advanced Examples

### Example 4: Error Handling

```c
#include <sssp.h>
#include <stdio.h>
#include <stdlib.h>

sssp_result_t* solve_with_validation(sssp_graph_t* graph, int source) {
    // Validate graph structure
    char error_msg[256];
    sssp_status_t status = sssp_graph_validate(graph, error_msg, sizeof(error_msg));

    if (status != SSSP_SUCCESS) {
        fprintf(stderr, "Graph validation failed: %s\n", error_msg);
        return NULL;
    }

    // Check for negative cycles
    if (sssp_graph_has_negative_cycle(graph, NULL, NULL)) {
        fprintf(stderr, "Error: Graph contains negative cycle!\n");
        return NULL;
    }

    // Validate source vertex
    int n = sssp_graph_get_num_vertices(graph);
    if (source < 0 || source >= n) {
        fprintf(stderr, "Error: Invalid source vertex %d (must be 0-%d)\n",
                source, n-1);
        return NULL;
    }

    // Solve SSSP
    sssp_result_t* result = sssp_solve(graph, source);

    if (!result) {
        fprintf(stderr, "Error: SSSP computation failed\n");
        return NULL;
    }

    // Check result status
    status = sssp_result_get_status(result);
    if (status != SSSP_SUCCESS) {
        fprintf(stderr, "Error: %s\n", sssp_status_to_string(status));
        sssp_result_destroy(result);
        return NULL;
    }

    // Verify solution correctness
    if (!sssp_result_verify(result)) {
        fprintf(stderr, "Warning: Solution verification failed!\n");
    }

    return result;
}

int main(void) {
    sssp_graph_t* graph = sssp_graph_create(100);

    // Add edges...

    sssp_result_t* result = solve_with_validation(graph, 0);

    if (result) {
        printf("✓ Computation successful!\n");
        sssp_result_destroy(result);
    }

    sssp_graph_destroy(graph);

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

### Example 5: Performance Comparison

```c
#include <sssp.h>
#include <time.h>
#include <stdio.h>

double measure_time(sssp_graph_t* graph, int source) {
    clock_t start = clock();
    sssp_result_t* result = sssp_solve(graph, source);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    if (result) {
        sssp_result_destroy(result);
    }

    return elapsed;
}

int main(void) {
    // Test different graph sizes
    int sizes[] = {100, 500, 1000, 5000, 10000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("Graph Size | Time (s) | Edges | m·log^(2/3)n\n");
    printf("-----------|----------|-------|-------------\n");

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int m = 4 * n;  // Sparse graph

        // Generate random graph
        sssp_graph_t* graph = sssp_graph_create(n);
        for (int e = 0; e < m; e++) {
            int u = rand() % n;
            int v = rand() % n;
            double weight = 1.0 + (rand() % 100) / 10.0;
            sssp_graph_add_edge(graph, u, v, weight);
        }

        // Measure time
        double time = measure_time(graph, 0);

        // Calculate theoretical operations
        double log_n = log((double)n);
        double log_2_3_n = pow(log_n, 2.0/3.0);
        double ops = m * log_2_3_n;

        printf("%10d | %8.4f | %5d | %11.0f\n", n, time, m, ops);

        sssp_graph_destroy(graph);
    }

    return 0;
}
```

### Example 6: Graph Generation

```c
#include <sssp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Generate random sparse graph
sssp_graph_t* generate_sparse_graph(int n, int avg_degree) {
    sssp_graph_t* graph = sssp_graph_create(n);

    int target_edges = n * avg_degree / 2;  // Undirected

    for (int i = 0; i < target_edges; i++) {
        int u = rand() % n;
        int v = rand() % n;

        if (u != v) {
            double weight = 1.0 + (rand() % 100) / 10.0;
            sssp_graph_add_edge(graph, u, v, weight);
            sssp_graph_add_edge(graph, v, u, weight);  // Bidirectional
        }
    }

    return graph;
}

// Generate grid graph
sssp_graph_t* generate_grid_graph(int rows, int cols) {
    int n = rows * cols;
    sssp_graph_t* graph = sssp_graph_create(n);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;

            // Right neighbor
            if (c + 1 < cols) {
                int v = r * cols + (c + 1);
                sssp_graph_add_edge(graph, u, v, 1.0);
                sssp_graph_add_edge(graph, v, u, 1.0);
            }

            // Down neighbor
            if (r + 1 < rows) {
                int v = (r + 1) * cols + c;
                sssp_graph_add_edge(graph, u, v, 1.0);
                sssp_graph_add_edge(graph, v, u, 1.0);
            }
        }
    }

    return graph;
}

int main(void) {
    srand(time(NULL));

    // Test sparse graph
    printf("Testing sparse graph...\n");
    sssp_graph_t* sparse = generate_sparse_graph(1000, 4);
    sssp_result_t* result1 = sssp_solve(sparse, 0);
    printf("Sparse graph: %d edges, solved in O(m log^(2/3) n)\n",
           sssp_graph_get_num_edges(sparse));
    sssp_result_destroy(result1);
    sssp_graph_destroy(sparse);

    // Test grid graph
    printf("\nTesting grid graph...\n");
    sssp_graph_t* grid = generate_grid_graph(50, 50);  // 2500 vertices
    sssp_result_t* result2 = sssp_solve(grid, 0);
    printf("Grid graph: %d vertices, %d edges\n",
           sssp_graph_get_num_vertices(grid),
           sssp_graph_get_num_edges(grid));
    sssp_result_destroy(result2);
    sssp_graph_destroy(grid);

    return 0;
}
```

---

## Integration Patterns

### Pattern 1: Using as a Library

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject C)

# Find SSSP library
find_package(sssp REQUIRED)

add_executable(myapp main.c)
target_link_libraries(myapp PRIVATE libsssp::sssp)
```

**main.c:**
```c
#include <sssp.h>

int main(void) {
    // Your application code
    sssp_graph_t* graph = sssp_graph_create(1000);
    // ...
    return 0;
}
```

### Pattern 2: Batch Processing

```c
#include <sssp.h>
#include <stdio.h>
#include <dirent.h>

void process_graph_file(const char* filename) {
    sssp_graph_t* graph = sssp_graph_load_edge_list(filename, NULL, 0);

    if (graph) {
        sssp_result_t* result = sssp_solve(graph, 0);

        if (result) {
            // Save results
            char output[256];
            snprintf(output, sizeof(output), "%s.result", filename);
            sssp_result_export(result, output);

            sssp_result_destroy(result);
        }

        sssp_graph_destroy(graph);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    DIR* dir = opendir(argv[1]);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt")) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);
            printf("Processing %s...\n", filepath);
            process_graph_file(filepath);
        }
    }

    closedir(dir);
    return 0;
}
```

---

## Performance Tuning

### Tip 1: Choose Right Graph Representation

For dense graphs (m ≈ n²), consider using adjacency matrix if you have custom implementation needs. For sparse graphs (m ≈ n), adjacency lists (default) are optimal.

### Tip 2: Pre-allocate for Known Sizes

```c
// If you know graph size in advance
sssp_graph_t* graph = sssp_graph_create(known_n);

// Add all edges at once (more cache-friendly)
for (int i = 0; i < m; i++) {
    sssp_graph_add_edge(graph, edges[i].u, edges[i].v, edges[i].weight);
}
```

### Tip 3: Reuse Graph Structures

```c
// Solve multiple sources on same graph
sssp_graph_t* graph = /* build once */;

for (int source = 0; source < n; source++) {
    sssp_result_t* result = sssp_solve(graph, source);
    // Process result
    sssp_result_destroy(result);
}

sssp_graph_destroy(graph);
```

### Tip 4: Build with Optimizations

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DSSSP_ENABLE_LTO=ON ..
make -j$(nproc)
```

---

## Common Pitfalls

### Pitfall 1: Memory Leaks

**❌ Wrong:**
```c
sssp_result_t* result = sssp_solve(graph, 0);
// Forgot to call sssp_result_destroy()!
```

**✅ Correct:**
```c
sssp_result_t* result = sssp_solve(graph, 0);
if (result) {
    // Use result...
    sssp_result_destroy(result);  // Always cleanup!
}
```

### Pitfall 2: Not Checking Return Values

**❌ Wrong:**
```c
sssp_result_t* result = sssp_solve(graph, 0);
double dist = sssp_result_get_distance(result, 10);  // result might be NULL!
```

**✅ Correct:**
```c
sssp_result_t* result = sssp_solve(graph, 0);
if (result) {
    double dist = sssp_result_get_distance(result, 10);
    sssp_result_destroy(result);
} else {
    fprintf(stderr, "Error: SSSP failed\n");
}
```

### Pitfall 3: Ignoring Negative Cycles

**❌ Wrong:**
```c
// Graph might have negative cycle!
sssp_result_t* result = sssp_solve(graph, 0);
```

**✅ Correct:**
```c
if (sssp_graph_has_negative_cycle(graph, NULL, NULL)) {
    fprintf(stderr, "Error: Graph has negative cycle!\n");
    return -1;
}
sssp_result_t* result = sssp_solve(graph, 0);
```

### Pitfall 4: Using Wrong Vertex IDs

**❌ Wrong:**
```c
sssp_graph_t* graph = sssp_graph_create(10);  // Vertices 0-9
sssp_graph_add_edge(graph, 0, 10, 1.0);  // Vertex 10 doesn't exist!
```

**✅ Correct:**
```c
sssp_graph_t* graph = sssp_graph_create(10);  // Vertices 0-9
// Only use vertex IDs 0-9
sssp_graph_add_edge(graph, 0, 9, 1.0);
```

---

## Complete Examples

See `examples/` directory for complete, working examples:

- **simple_example.c**: Basic API usage
- **basic_usage.c**: Comparison with Dijkstra
- **performance_test.c**: Comprehensive benchmarking
- **graph_generators.c**: Random graph generation
- **utils_demo.c**: Utility functions

To build and run:
```bash
cd build
make
./bin/examples/simple_example
./bin/examples/performance_test
```

---

**For more details, see:**
- [API Reference](API_REFERENCE.md) - Complete function documentation
- [Technical Analysis](TECHNICAL_ANALYSIS.md) - Algorithm details
- [Documentation](DOCUMENTATION.md) - Comprehensive guide

**Last Updated**: 2025-11-12
