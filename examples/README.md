# SSSP Breakthrough - Examples

This directory contains example programs demonstrating how to use the SSSP Breakthrough algorithm.

## Available Examples

### 1. **basic_usage.c** (Recommended Starting Point)
**Size:** 21KB, 570 lines
**Best for:** Learning the basics and comparing with Dijkstra

**What it demonstrates:**
- ✓ Creating graphs manually and adding edges
- ✓ Running the SSSP breakthrough algorithm
- ✓ Running Dijkstra's algorithm for comparison
- ✓ Verifying correctness of results
- ✓ Performance measurement and comparison
- ✓ Testing different graph sizes (10, 100, 1000 vertices)
- ✓ Handling graphs with negative weights

**Examples included:**
1. **Small Manual Graph (10 vertices)** - Step-by-step walkthrough
2. **Medium Random Graph (100 vertices, 500 edges)** - Performance comparison
3. **Large Sparse Graph (1000 vertices)** - Scalability test
4. **Negative Weights Graph** - Shows advantage over Dijkstra

**Run it:**
```bash
cd build
./bin/examples/basic_usage
```

**Expected output:**
- Detailed step-by-step explanations
- Performance timings for both algorithms
- Correctness verification
- Sample shortest path distances
- Path reconstruction showing actual routes

---

### 2. **simple_example.c**
**Size:** 4.2KB
**Best for:** Quick start with the public API

**What it demonstrates:**
- ✓ Using the clean public API (sssp.h)
- ✓ Simple graph creation
- ✓ SSSP computation
- ✓ Result querying
- ✓ Error handling

**Run it:**
```bash
cd build
./bin/examples/simple_example
```

---

### 3. **utils_demo.c**
**Size:** 7.7KB
**Best for:** Learning utility functions

**What it demonstrates:**
- ✓ Random graph generation (7 different types)
- ✓ File I/O (reading/writing graphs)
- ✓ Performance timing utilities
- ✓ Graph validation and property checking
- ✓ Memory tracking

**Run it:**
```bash
cd build
./bin/examples/utils_demo
```

---

### 4. **performance_test.c**
**Size:** 22KB, 611 lines
**Best for:** Benchmarking and performance analysis

**What it demonstrates:**
- ✓ Systematic performance testing (100 to 50K vertices)
- ✓ Comparison with Dijkstra across graph types
- ✓ Memory usage analysis
- ✓ CSV output for plotting
- ✓ Multiple trials with statistical averaging

**Run it:**
```bash
cd build
./bin/examples/performance_test
python3 ../examples/plot_performance.py
```

**See:** `examples/PERFORMANCE_README.md` for detailed guide

---

### 5. **graph_generators.c**
**Size:** 30KB, 904 lines
**Best for:** Creating test graphs

**What it demonstrates:**
- ✓ 15 different graph types (sparse, grid, scale-free, etc.)
- ✓ 5 export formats (edge list, matrix, DIMACS, DOT, JSON)
- ✓ Interactive menu or command-line interface
- ✓ Transportation networks, worst-case graphs
- ✓ Statistics and visualization export

**Graph types:**
- Random: sparse, medium, dense
- Structured: grid, tree, DAG, path, complete
- Networks: scale-free, small-world, road, airline
- Special: Dijkstra worst-case, negative weights

**Run it:**
```bash
cd build
./bin/examples/graph_generators --menu
# or
./bin/examples/graph_generators --type scale_free -n 500 -o network.txt
```

**See:** `examples/GENERATORS_README.md` for complete guide

---

## Quick Start Guide

### 1. Build all examples

```bash
# From project root
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
cmake --build .
```

### 2. Run an example

```bash
# All examples are in build/bin/examples/
./bin/examples/basic_usage
./bin/examples/simple_example
./bin/examples/utils_demo
```

### 3. Explore the code

Start with `basic_usage.c` - it has detailed comments explaining every step.

## Example Comparison

| Example | Complexity | Lines | Focus | Best For |
|---------|-----------|-------|-------|----------|
| `simple_example.c` | ⭐ Simple | 130 | Public API | Quick start |
| `basic_usage.c` | ⭐⭐ Moderate | 570 | Algorithm comparison | Learning |
| `utils_demo.c` | ⭐⭐ Moderate | 246 | Utilities | Testing |

## Code Snippets

### Creating a Graph (from basic_usage.c)

```c
// Create graph with 10 vertices
Graph* graph = create_graph(10);

// Add edges (directed)
add_edge(graph, 0, 1, 4.0);  // from 0 to 1, weight 4.0
add_edge(graph, 0, 2, 2.0);
add_edge(graph, 1, 3, 5.0);

// Run SSSP from source vertex 0
sssp_breakthrough(graph, 0);

// Access results
for (int i = 0; i < graph->n; i++) {
    printf("Distance to %d: %.2f\n", i, graph->dist[i]);
}

// Cleanup
free_graph(graph);
```

### Using Utilities (from utils_demo.c)

```c
// Generate random graph
GraphGenParams params = graph_gen_params_default(100, 500);
params.min_weight = 1.0;
params.max_weight = 10.0;
Graph* graph = graph_generate_random(&params);

// Time the algorithm
Timer* timer = timer_create();
timer_start(timer);
sssp_breakthrough(graph, 0);
double seconds = timer_stop(timer);

char time_str[64];
format_time(seconds, time_str, sizeof(time_str));
printf("Completed in: %s\n", time_str);

// Cleanup
timer_destroy(timer);
free_graph(graph);
```

### Comparing Algorithms (from basic_usage.c)

```c
Graph* graph = graph_generate_random(&params);

// Run breakthrough algorithm
Timer* timer = timer_create();
timer_start(timer);
sssp_breakthrough(graph, 0);
double time_breakthrough = timer_stop(timer);

// Save results
double* dist_breakthrough = malloc(graph->n * sizeof(double));
memcpy(dist_breakthrough, graph->dist, graph->n * sizeof(double));

// Run Dijkstra
timer_start(timer);
dijkstra(graph, 0);
double time_dijkstra = timer_stop(timer);

// Compare
printf("Breakthrough: %.3f ms\n", time_breakthrough * 1000);
printf("Dijkstra:     %.3f ms\n", time_dijkstra * 1000);
printf("Speedup:      %.2fx\n", time_dijkstra / time_breakthrough);

// Verify results match
Graph temp = *graph;
temp.dist = dist_breakthrough;
bool match = compare_results(&temp, graph);
printf("Results match: %s\n", match ? "YES" : "NO");
```

## Sample Data Files

### sample_graph.txt
A small example graph in edge list format:
```
7 11
0 1 2.0
0 2 6.0
1 2 3.0
...
```

**Use it:**
```c
Graph* graph = graph_read_edge_list("examples/sample_graph.txt");
```

## Output Examples

### basic_usage.c Output Structure

```
═══════════════════════════════════════════════════════════════
Example 1: Small Manual Graph (10 vertices)
═══════════════════════════════════════════════════════════════

Step 1: Creating graph with 10 vertices...
Step 2: Adding edges to form a network...
  Added 14 edges

Step 3: Running SSSP Breakthrough Algorithm...
  Completed in 234.567 μs

Step 4: Running Dijkstra's Algorithm (for comparison)...
  Completed in 123.456 μs

Step 5: Verifying results match...
  Results MATCH ✓

Step 6: Shortest distances from source (vertex 0):
  Sample distances:
    Vertex   0:    0.000  [path: 0]
    Vertex   1:    4.000  [path: 0 → 1]
    Vertex   2:    2.000  [path: 0 → 2]
    Vertex   3:    9.000  [path: 0 → 1 → 3]
    ...

Step 7: Verifying solution optimality...
  Solution verified: All distances satisfy optimality ✓
```

## Understanding the Examples

### What is SSSP?
Single-Source Shortest Path - finding the shortest path from one source vertex to all other vertices in a weighted graph.

### Why Compare with Dijkstra?
- **Dijkstra**: Classic algorithm, O((V+E) log V) time
- **Breakthrough**: New algorithm, O(m^{1+o(1)}) time (where m = edges)
- Comparison shows correctness and performance characteristics

### Dijkstra vs Breakthrough

| Algorithm | Time Complexity | Negative Weights | Best Use Case |
|-----------|----------------|------------------|---------------|
| Dijkstra | O((V+E) log V) | ❌ No | Dense graphs, non-negative weights |
| Breakthrough | O(m^{1+o(1)}) | ✅ Yes | Large sparse graphs, theoretical improvement |

### When to Use Each Example

**Use `simple_example.c` when:**
- You want a quick introduction
- You're integrating into your project
- You need minimal code to understand the API

**Use `basic_usage.c` when:**
- You want to understand the algorithm deeply
- You need to compare performance
- You're testing different graph types
- You want to see detailed explanations

**Use `utils_demo.c` when:**
- You need to generate test graphs
- You want to save/load graphs from files
- You need timing and profiling tools
- You're debugging graph properties

## Building Your Own Example

Template for a new example:

```c
#include "sssp_breakthrough.h"
#include "utils.h"

int main(void) {
    // 1. Create or load graph
    Graph* graph = create_graph(n);
    // ... add edges ...

    // 2. Run algorithm
    sssp_breakthrough(graph, source);

    // 3. Use results
    for (int i = 0; i < graph->n; i++) {
        printf("Distance to %d: %.2f\n", i, graph->dist[i]);
    }

    // 4. Cleanup
    free_graph(graph);

    return 0;
}
```

Add it to `examples/` and it will be automatically built by CMake.

## Troubleshooting

### Example doesn't compile
```bash
# Make sure you're building with examples enabled
cmake -DBUILD_EXAMPLES=ON ..
cmake --build .
```

### Can't find executables
```bash
# They're in build/bin/examples/, not build/examples/
ls build/bin/examples/
```

### Want more verbose output
Edit the example file and change `max_print` values in `print_sample_distances()`.

## Learning Path

1. **Start here:** `simple_example.c` - Understand basic API
2. **Then read:** `basic_usage.c` - Learn algorithm details
3. **Explore:** `utils_demo.c` - See available utilities
4. **Create:** Your own example based on templates above

## Further Reading

- **Public API:** See `include/sssp.h` and `include/USAGE.md`
- **Utilities:** See `src/utils.h` and `src/UTILS_README.md`
- **Algorithm:** See `TECHNICAL_ANALYSIS.md` in project root
- **Paper:** See `paper/` directory

## Contributing Examples

To add a new example:

1. Create `examples/your_example.c`
2. It will be automatically detected by CMake
3. Build and test: `cmake --build . && ./bin/examples/your_example`
4. Document it in this README

## Questions?

- Check the source code - it's heavily commented
- Read the technical documentation in the project root
- See the API documentation in `include/`
