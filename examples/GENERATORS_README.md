# Graph Generators - Complete Guide

Comprehensive graph generation tool for creating test graphs for SSSP algorithms.

## Quick Start

```bash
# Build
cd build
cmake --build .

# Interactive menu
./bin/examples/graph_generators --menu

# Command line
./bin/examples/graph_generators --type sparse --vertices 1000 --output test.txt
```

## Available Graph Types

### 1. Random Graphs

#### **Sparse** (`sparse`)
- Average degree: ~3
- Realistic for social networks, web graphs
- Fast to generate and process

```bash
./bin/examples/graph_generators --type sparse -n 1000 -o sparse_1k.txt
```

#### **Medium Density** (`medium`)
- Average degree: ~10
- Moderate connectivity
- Good for general testing

```bash
./bin/examples/graph_generators --type medium -n 500 -o medium.txt
```

#### **Dense** (`dense`)
- Average degree: ~50
- High connectivity
- Stress tests for algorithms

```bash
./bin/examples/graph_generators --type dense -n 200 -o dense.txt
```

### 2. Structured Graphs

#### **Grid 2D** (`grid`)
- Regular 2D lattice
- Models spatial networks
- Each node has 2-4 neighbors

```bash
./bin/examples/graph_generators --type grid -n 100 -o grid.txt
# Creates 10x10 grid (√100 = 10)
```

**Visualization:**
```
0 - 1 - 2
|   |   |
3 - 4 - 5
|   |   |
6 - 7 - 8
```

#### **Grid 3D** (`grid3d`)
- Regular 3D cube lattice
- Models 3D structures
- Each node has 3-6 neighbors

```bash
./bin/examples/graph_generators --type grid3d -n 125 -o grid3d.txt
# Creates 5x5x5 cube (∛125 = 5)
```

#### **Complete Graph** (`complete`)
- All vertices connected to all others
- n*(n-1) edges
- Maximum density

```bash
./bin/examples/graph_generators --type complete -n 50 -o complete.txt
```

#### **Tree** (`tree`)
- Random spanning tree
- Exactly n-1 edges
- No cycles

```bash
./bin/examples/graph_generators --type tree -n 100 -o tree.txt
```

#### **DAG** (`dag`)
- Directed Acyclic Graph
- Topologically ordered
- No cycles (negative weights safe)

```bash
./bin/examples/graph_generators --type dag -n 100 -o dag.txt
```

#### **Path** (`path`)
- Simple chain: 0 → 1 → 2 → ... → n-1
- Exactly n-1 edges
- Maximum diameter

```bash
./bin/examples/graph_generators --type path -n 100 -o path.txt
```

### 3. Scale-Free Networks

#### **Barabási-Albert Model** (`scale_free`)
- Preferential attachment
- Power-law degree distribution
- Models internet, citation networks

**Properties:**
- Few high-degree "hubs"
- Many low-degree nodes
- Robust to random failures

```bash
./bin/examples/graph_generators --type scale_free -n 500 -o scalefree.txt
```

**Algorithm:**
1. Start with m₀ connected vertices
2. Add vertices one by one
3. Each new vertex connects to m existing vertices
4. Probability ∝ vertex degree (rich get richer)

### 4. Transportation Networks

#### **Road Network** (`road`)
- Grid base with diagonal shortcuts
- Long-range highways (fast routes)
- Models city street networks

**Features:**
- Local roads (grid structure)
- Diagonal shortcuts (30% probability)
- Highways (5% of vertices, low weight)

```bash
./bin/examples/graph_generators --type road -n 100 -o road.txt --visualize
```

**Structure:**
```
Regular roads: weight 1-10
Diagonals:     weight 8-15
Highways:      weight 0.5-2 (fast!)
```

#### **Airline Network** (`airline`)
- Hub-and-spoke topology
- Few major hubs (10% of vertices)
- Most airports connect through hubs

**Features:**
- Hubs fully connected to each other
- Spoke airports connect to 1-3 hubs
- Occasional direct spoke-to-spoke flights (10%)

```bash
./bin/examples/graph_generators --type airline -n 100 -o airline.txt
```

**Example:**
```
Hubs:   0, 1, 2, ..., 9     (fully connected)
Spokes: 10-99               (connect to hubs)
```

### 5. Small-World Networks

#### **Watts-Strogatz Model** (`small_world`)
- High clustering
- Short average path length
- Models social networks, neural networks

**Parameters:**
- k = 4 (neighbors in ring)
- β = 0.1 (rewiring probability)

```bash
./bin/examples/graph_generators --type small_world -n 100 -o smallworld.txt
```

**Algorithm:**
1. Create ring lattice (each vertex connects to k nearest neighbors)
2. Rewire each edge with probability β
3. Creates shortcuts that reduce path lengths

### 6. Special Purpose Graphs

#### **Dijkstra Worst-Case** (`dijkstra_worst`)
- Maximizes priority queue updates
- Forces many decrease-key operations
- Tests Dijkstra's performance limits

```bash
./bin/examples/graph_generators --type dijkstra_worst -n 100 -o worst.txt
```

**Strategy:**
- Main path with decreasing weights
- Cross edges discovered late
- Forces re-evaluation of distances

#### **Negative Weights** (`negative`)
- Contains negative edge weights
- Guaranteed no negative cycles (is a DAG)
- Tests Bellman-Ford, breakthrough algorithm
- Dijkstra fails on these!

```bash
./bin/examples/graph_generators --type negative -n 100 -o negative.txt
```

**Properties:**
- 30% of edges are negative
- Maintains DAG property (u < v)
- Negative weights: -10 to -1
- Positive weights: 1 to 10

## Export Formats

### 1. Edge List (Default)
Simple text format, easy to parse

**Format:**
```
n m
u v weight
u v weight
...
```

**Example:**
```
5 6
0 1 2.5
0 2 3.0
1 3 1.5
2 3 4.0
3 4 2.0
4 0 5.5
```

**Use case:** Default format, universally compatible

### 2. Adjacency Matrix
Dense matrix representation

**Format:**
```
n
w[0][0] w[0][1] ... w[0][n-1]
w[1][0] w[1][1] ... w[1][n-1]
...
w[n-1][0] w[n-1][1] ... w[n-1][n-1]
```

**Example (3x3):**
```
3
0.0 2.5 INF
INF 0.0 3.0
1.5 INF 0.0
```

**Use case:** Mathematical analysis, dense graphs

### 3. DIMACS
Standard graph challenge format

**Format:**
```
c Comments
p sp n m
s source
a u v weight
a u v weight
...
```

**Example:**
```
c Example graph
p sp 5 6
s 1
a 1 2 2
a 1 3 4
a 2 3 1
a 2 4 5
a 3 4 3
a 4 5 1
```

**Use case:** Graph competitions, benchmarking

### 4. DOT (GraphViz)
Visualization format

**Format:**
```
digraph G {
  rankdir=LR;
  0 -> 1 [label="2.5"];
  ...
}
```

**Use case:** Visualization, documentation

**Render:**
```bash
dot -Tpng graph.dot -o graph.png
dot -Tsvg graph.dot -o graph.svg
```

### 5. JSON
Web applications, JavaScript

**Format:**
```json
{
  "vertices": 5,
  "edges": 6,
  "nodes": [
    {"id": 0},
    {"id": 1},
    ...
  ],
  "links": [
    {"source": 0, "target": 1, "weight": 2.5},
    ...
  ]
}
```

**Use case:** D3.js, web visualization, APIs

## Usage Examples

### Command Line Mode

```bash
# Generate sparse graph
./bin/examples/graph_generators \
  --type sparse \
  --vertices 1000 \
  --output sparse_1k.txt \
  --format edge_list

# Generate with statistics
./bin/examples/graph_generators \
  -t scale_free \
  -n 500 \
  -o scalefree.txt \
  --stats scalefree.stats

# Generate with visualization
./bin/examples/graph_generators \
  -t road \
  -n 100 \
  -o road.txt \
  --visualize

# Multiple formats
./bin/examples/graph_generators -t grid -n 64 -f matrix -o grid.mat
./bin/examples/graph_generators -t grid -n 64 -f json -o grid.json
./bin/examples/graph_generators -t grid -n 64 -f dimacs -o grid.dimacs

# With reproducible random seed
./bin/examples/graph_generators \
  -t sparse \
  -n 1000 \
  --seed 12345 \
  -o reproducible.txt
```

### Interactive Menu Mode

```bash
./bin/examples/graph_generators --menu
```

**Menu workflow:**
```
Select graph type (1-15): 10         # Scale-free
Number of vertices: 500
Output filename: network.txt
Format (edge_list/matrix/dimacs/dot/json) [edge_list]: json
Generate statistics? (y/n) [n]: y
Generate visualization? (y/n) [n]: y

Generating Scale-Free with 500 vertices...
Generated graph: 500 vertices, 994 edges
Exporting to network.txt (json format)...
Successfully exported!
Generating statistics: network.txt.stats
Generating DOT file: network.txt.dot
```

### Batch Generation

```bash
#!/bin/bash
# generate_all.sh - Generate test suite

GENERATOR=./bin/examples/graph_generators

# Small graphs for testing
$GENERATOR -t sparse -n 100 -o test_sparse_100.txt
$GENERATOR -t grid -n 100 -o test_grid_100.txt
$GENERATOR -t scale_free -n 100 -o test_sf_100.txt

# Medium graphs for benchmarking
$GENERATOR -t sparse -n 1000 -o bench_sparse_1k.txt
$GENERATOR -t road -n 1000 -o bench_road_1k.txt

# Large graphs for stress testing
$GENERATOR -t sparse -n 10000 -o stress_sparse_10k.txt

# Special purpose
$GENERATOR -t dijkstra_worst -n 500 -o dijkstra_worst.txt
$GENERATOR -t negative -n 500 -o negative_weights.txt

echo "Test suite generated!"
```

## Statistics Output

When using `--stats` option:

```
Graph Statistics
================

Basic Properties:
  Vertices (n):        500
  Edges (m):           994
  Average degree:      3.98
  Maximum degree:      42

Edge Weights:
  Minimum weight:      1.234567
  Maximum weight:      99.876543
  Has negative edges:  No

Graph Structure:
  Is connected:        Yes
  Is DAG:              No
  Has negative cycle:  No

Graph Density:         0.007976
```

## Use Cases

### 1. Testing SSSP Algorithms

```bash
# Test on various graph types
for type in sparse medium grid scale_free; do
  ./bin/examples/graph_generators -t $type -n 1000 -o test_${type}.txt
  ./bin/examples/basic_usage test_${type}.txt
done
```

### 2. Performance Benchmarking

```bash
# Generate graphs of increasing size
for n in 100 500 1000 5000 10000; do
  ./bin/examples/graph_generators -t sparse -n $n -o bench_${n}.txt
done
```

### 3. Algorithm Comparison

```bash
# Create worst-case for Dijkstra
./bin/examples/graph_generators -t dijkstra_worst -n 500 -o worst_case.txt

# Create graph with negative weights
./bin/examples/graph_generators -t negative -n 500 -o negative.txt

# Test both algorithms
./bin/examples/basic_usage worst_case.txt
./bin/examples/basic_usage negative.txt
```

### 4. Visualization

```bash
# Generate small graph for visualization
./bin/examples/graph_generators \
  -t small_world \
  -n 50 \
  -o network.txt \
  --visualize

# Render
dot -Tpng network.txt.dot -o network.png
open network.png
```

### 5. Web Application Data

```bash
# Generate JSON for web viz
./bin/examples/graph_generators \
  -t airline \
  -n 100 \
  -f json \
  -o airline_network.json

# Use in D3.js:
# d3.json("airline_network.json").then(data => { ... });
```

## Graph Characteristics

### Connectivity

| Type | Always Connected? | Components |
|------|------------------|------------|
| Complete | Yes | 1 |
| Tree | Yes | 1 |
| Path | Yes | 1 |
| Grid | Yes | 1 |
| DAG | No | Varies |
| Sparse | Usually | 1-few |
| Scale-Free | Usually | 1 |
| Road | Yes | 1 |
| Airline | Yes | 1 |

### Complexity

| Type | Vertices | Edges | Generation Time |
|------|----------|-------|----------------|
| Sparse | n | ~3n | O(n) |
| Medium | n | ~10n | O(n) |
| Grid | n | ~2n | O(n) |
| Complete | n | n² | O(n²) |
| Scale-Free | n | ~(m₀+m(n-m₀)) | O(n·m) |
| Road | n | ~2n + highways | O(n) |

### Use by Algorithm

**Best for Dijkstra:**
- Dense graphs
- Non-negative weights
- Complete graphs

**Best for Breakthrough:**
- Sparse graphs
- Large graphs
- Negative weights OK

**Best for Bellman-Ford:**
- Graphs with negative weights
- When negative cycles need detection

## Troubleshooting

### "Failed to generate graph"

Check:
- Vertex count is positive
- Graph type is valid
- Enough memory available

### Generated graph is disconnected

Some graph types may create disconnected graphs:
- Use `sparse` or `grid` for guaranteed connectivity
- Check statistics file

### File format not recognized

Valid formats:
- `edge_list`
- `matrix`
- `dimacs`
- `dot`
- `json`

### Visualization too large

For large graphs (n > 100):
- Use smaller sample
- Export statistics instead
- Use JSON for interactive web viz

### Out of memory

For very large graphs:
- Reduce vertex count
- Use sparse graph types
- Don't use `complete` type

## Advanced Features

### Custom Parameters

Edit `graph_generators.c` to customize:

```c
// Scale-free parameters
int m0 = 3;   // Initial vertices
int m = 2;    // Edges per new vertex

// Road network
double highway_prob = 0.05;  // 5% highways

// Airline network
int num_hubs = n * 0.1;  // 10% hubs

// Small world
int k = 4;           // Neighbors
double beta = 0.1;   // Rewiring probability
```

### Combining Generators

Generate hybrid graphs:

```c
Graph* graph = graph_generate_grid(side, side, 1.0, 10.0, false);
// Add scale-free connections
// Add highways
// Export
```

## Performance

### Generation Speed

On modern hardware (M1/Intel i7):

| Type | n=1K | n=10K | n=100K |
|------|------|-------|--------|
| Sparse | <1ms | ~10ms | ~100ms |
| Grid | <1ms | ~10ms | ~100ms |
| Complete | ~10ms | ~1s | ~100s |
| Scale-Free | ~5ms | ~50ms | ~500ms |

### Memory Usage

Approximate memory (MB):

```
Sparse:      (n + m) × 24 bytes / 1MB
             ~1MB per 40K vertices

Complete:    (n + n²) × 24 bytes / 1MB
             ~1MB per 200 vertices (dense!)
```

## Integration

### With Other Tools

```bash
# Generate graph
./bin/examples/graph_generators -t sparse -n 1000 -o graph.txt

# Run SSSP
./bin/examples/basic_usage graph.txt

# Benchmark
./bin/examples/performance_test graph.txt

# Visualize results
python3 examples/plot_performance.py
```

### Programmatic Use

```c
#include "utils.h"

// Generate in your code
Graph* graph = graph_generate_scale_free(500, 3, 2, 1.0, 100.0);

// Use it
sssp_breakthrough(graph, 0);

// Export
graph_write_json(graph, "output.json");

// Cleanup
free_graph(graph);
```

## References

**Graph Models:**
- Erdős-Rényi: Random graphs (1959)
- Barabási-Albert: Scale-free networks (1999)
- Watts-Strogatz: Small-world networks (1998)

**Applications:**
- Social Networks: Scale-free, small-world
- Transportation: Road, airline
- Internet: Scale-free
- Biology: Small-world

## See Also

- `examples/basic_usage.c` - Test generated graphs
- `examples/performance_test.c` - Benchmark on different types
- `src/utils.h` - Lower-level generation functions
- `TECHNICAL_ANALYSIS.md` - Algorithm details
