# SSSP Utilities Documentation

Comprehensive utility functions for the SSSP Breakthrough algorithm project.

## Overview

The utilities module (`utils.h` / `utils.c`) provides essential helper functions for:
- **Graph File I/O** - Read/write graphs in multiple formats
- **Memory Management** - Safe allocation with tracking
- **Performance Timing** - High-resolution cross-platform timing
- **Graph Validation** - Verify graph properties and correctness
- **Random Graph Generation** - Generate test graphs
- **General Utilities** - Random numbers, argument parsing, etc.

## Features at a Glance

| Category | Functions | Purpose |
|----------|-----------|---------|
| File I/O | 8 functions | Read/write edge list, DIMACS, DOT formats |
| Memory | 7 functions | Safe malloc/calloc/free with tracking |
| Timing | 6 functions | High-resolution timers, time formatting |
| Validation | 7 functions | Check connectivity, DAG, negative cycles |
| Generators | 7 functions | Random, complete, grid, tree, DAG graphs |
| Utilities | 5 functions | Random numbers, CLI parsing |

## Graph File I/O

### Supported Formats

#### 1. Edge List Format (Recommended)
Simple text format:
```
5 6
0 1 2.0
0 2 4.0
1 2 1.0
1 3 7.0
2 3 3.0
3 4 1.0
```
First line: `n m` (vertices, edges)
Following lines: `u v weight`

#### 2. DIMACS Format
Standard graph challenge format:
```
c DIMACS shortest path format
p sp 5 6
s 1
a 1 2 2
a 1 3 4
a 2 3 1
a 2 4 7
a 3 4 3
a 4 5 1
```

#### 3. DOT Format (GraphViz)
For visualization:
```
digraph G {
  rankdir=LR;
  0 -> 1 [label="2.0"];
  0 -> 2 [label="4.0"];
  ...
}
```

### Usage Examples

```c
// Read from file
Graph* graph = graph_read_edge_list("graph.txt");

// Write to different formats
graph_write_edge_list(graph, "output.txt");
graph_write_dimacs(graph, "output.dimacs", 0);
graph_write_dot(graph, "output.dot", NULL);

// With distance coloring for visualization
graph_write_dot(graph, "output.dot", graph->dist);
```

### Visualization

After writing DOT format:
```bash
dot -Tpng graph.dot -o graph.png
dot -Tsvg graph.dot -o graph.svg
```

## Memory Management

### Safe Allocation Functions

```c
// Safe malloc with error checking
void* ptr = safe_malloc(size);

// Safe calloc (zeroed memory)
void* ptr = safe_calloc(n, size);

// Safe realloc
ptr = safe_realloc(ptr, new_size);

// Safe free (NULL-safe)
safe_free(ptr);
```

### Memory Tracking

```c
// Reset statistics
memory_reset_stats();

// Allocate some memory
Graph* g = graph_generate_random(&params);

// Check memory usage
size_t allocated, peak;
memory_get_stats(&allocated, &peak);
printf("Currently allocated: %zu bytes\n", allocated);
printf("Peak allocated: %zu bytes\n", peak);

// Or print directly
memory_print_stats();
```

**Example Output:**
```
Memory Statistics:
  Current allocated: 524288 bytes (0.50 MB)
  Peak allocated: 1048576 bytes (1.00 MB)
```

## Performance Timing

### Basic Timer Usage

```c
// Create and start timer
Timer* timer = timer_create();

// Do some work
Graph* graph = graph_generate_random(&params);

// Stop and get elapsed time
double seconds = timer_stop(timer);

// Format nicely
char time_str[64];
format_time(seconds, time_str, sizeof(time_str));
printf("Took: %s\n", time_str);

// Clean up
timer_destroy(timer);
```

### Multiple Timings

```c
Timer* timer = timer_create();

timer_start(timer);
// ... operation 1 ...
printf("Operation 1: %.3f ms\n", timer_stop(timer) * 1000);

timer_start(timer);
// ... operation 2 ...
printf("Operation 2: %.3f ms\n", timer_stop(timer) * 1000);

timer_destroy(timer);
```

### Time Formatting

```c
format_time(0.000000123, buf, size);  // "123.000 ns"
format_time(0.000123, buf, size);     // "123.000 μs"
format_time(0.123, buf, size);        // "123.000 ms"
format_time(1.5, buf, size);          // "1.500 s"
format_time(125.0, buf, size);        // "2m 5.000s"
format_time(7384.5, buf, size);       // "2h 3m 4.500s"
```

## Graph Validation

### Graph Properties

```c
Graph* graph = graph_generate_random(&params);

// Get all properties at once
GraphProperties props = graph_get_properties(graph);

// Print summary
graph_print_properties(&props);
```

**Example Output:**
```
Graph Properties:
  Vertices: 100
  Edges: 250
  Average degree: 2.50
  Maximum degree: 8
  Weight range: [1.000, 100.000]
  Has negative weights: no
  Has negative cycle: no
  Is connected: yes
  Is DAG: no
```

### Individual Checks

```c
// Validate graph structure
if (graph_validate(graph)) {
    printf("Graph is valid\n");
}

// Check connectivity
if (graph_is_connected(graph)) {
    printf("Graph is connected\n");
}

// Check if DAG
if (graph_is_dag(graph)) {
    printf("Graph is a DAG\n");
}

// Check for negative cycles
if (graph_has_negative_cycle(graph, 0)) {
    printf("Negative cycle detected!\n");
}
```

## Random Graph Generators

### 1. Random Graph (Erdős-Rényi)

```c
// Create parameters
GraphGenParams params = graph_gen_params_default(100, 250);
params.min_weight = 1.0;
params.max_weight = 10.0;
params.seed = 42;  // For reproducibility

// Generate graph
Graph* graph = graph_generate_random(&params);
```

### 2. Complete Graph

```c
// All vertices connected to all others
Graph* graph = graph_generate_complete(20, 1.0, 100.0);
// n=20, weights in [1.0, 100.0]
// Results in 20 * 19 = 380 edges
```

### 3. Grid Graph

```c
// 2D grid with neighbor connections
Graph* graph = graph_generate_grid(5, 5, 1.0, 10.0, false);
// 5x5 grid, no diagonals = 25 vertices, 40 edges

// With diagonals
Graph* graph = graph_generate_grid(5, 5, 1.0, 10.0, true);
// 5x5 grid, with diagonals = 25 vertices, 98 edges
```

### 4. Random Tree

```c
// Connected acyclic graph
Graph* graph = graph_generate_tree(50, 1.0, 5.0);
// n=50, exactly 49 edges
```

### 5. Random DAG

```c
// Directed Acyclic Graph
Graph* graph = graph_generate_dag(30, 60, 1.0, 10.0);
// n=30, m=60, all edges go from lower to higher vertex IDs
```

### 6. Sparse Graph

```c
// Graph with controlled average degree
Graph* graph = graph_generate_sparse(1000, 3, 1.0, 100.0);
// n=1000, avg_degree=3, approximately 1500 edges
```

### 7. Path Graph

```c
// Simple path from 0 to n-1
Graph* graph = graph_generate_path(100, 1.0, 5.0);
// n=100, exactly 99 edges forming a path
```

## General Utilities

### Random Number Generation

```c
// Set seed for reproducibility
utils_set_random_seed(12345);

// Generate random integers
int x = random_int(1, 100);  // [1, 100] inclusive

// Generate random doubles
double y = random_double(0.0, 1.0);  // [0.0, 1.0]
```

### Argument Parsing

```c
int main(int argc, char** argv) {
    int n, m, source;

    if (!parse_args(argc, argv, &n, &m, &source)) {
        print_usage(argv[0]);
        return 1;
    }

    printf("n=%d, m=%d, source=%d\n", n, m, source);
    return 0;
}
```

## Complete Example

```c
#include "sssp_breakthrough.h"
#include "utils.h"

int main(void) {
    // Initialize
    utils_set_random_seed(0);
    memory_reset_stats();
    Timer* timer = timer_create();

    // Generate random graph
    printf("Generating graph...\n");
    GraphGenParams params = graph_gen_params_default(1000, 5000);
    params.min_weight = 1.0;
    params.max_weight = 100.0;

    Graph* graph = graph_generate_random(&params);

    // Validate
    if (!graph_validate(graph)) {
        fprintf(stderr, "Invalid graph!\n");
        return 1;
    }

    // Analyze properties
    GraphProperties props = graph_get_properties(graph);
    graph_print_properties(&props);

    // Save to file
    graph_write_edge_list(graph, "test_graph.txt");
    graph_write_dot(graph, "test_graph.dot", NULL);

    // Time SSSP
    timer_start(timer);
    sssp_breakthrough(graph, 0);
    double elapsed = timer_stop(timer);

    char time_str[64];
    format_time(elapsed, time_str, sizeof(time_str));
    printf("SSSP completed in %s\n", time_str);

    // Print memory stats
    memory_print_stats();

    // Cleanup
    free_graph(graph);
    timer_destroy(timer);

    return 0;
}
```

## Platform Support

All utilities are cross-platform compatible:
- **Linux** - Fully supported
- **macOS** - Fully supported
- **Windows (WSL)** - Fully supported
- **Windows (native)** - Timing functions adapted for Windows API

## Performance Notes

1. **File I/O**: Edge list format is fastest, DIMACS is most compatible
2. **Memory Tracking**: Minimal overhead (~5% slowdown)
3. **Timing**: Microsecond precision on all platforms
4. **Validation**: Graph property analysis is O(V + E)
5. **Generation**: Random graph generation is O(E)

## Error Handling

All functions handle errors gracefully:
- NULL pointer checks
- Range validation
- File operation errors
- Memory allocation failures

Example:
```c
Graph* graph = graph_read_edge_list("nonexistent.txt");
if (!graph) {
    fprintf(stderr, "Failed to read graph\n");
    return 1;
}
```

## Building with Utilities

The utilities are automatically included in the build:

```bash
mkdir build && cd build
cmake ..
cmake --build .

# Run utilities demo
./bin/examples/utils_demo
```

## API Reference

See `utils.h` for complete API documentation with Doxygen-style comments.

## Examples

The `examples/` directory contains:
- `utils_demo.c` - Comprehensive demonstration of all utilities

Run the demo:
```bash
cd build
./bin/examples/utils_demo
```

## Contributing

When adding new utilities:
1. Add declaration to `utils.h` with documentation
2. Implement in `utils.c`
3. Add example to `utils_demo.c`
4. Update this README
