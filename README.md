# SSSP Breakthrough Algorithm Implementation

This repository contains a C implementation of the groundbreaking single-source shortest path algorithm from the paper "Breaking the Sorting Barrier for Directed Single-Source Shortest Paths" by Ran Duan, Jiayi Mao, Xiao Mao, Xinkai Shu, and Longhui Yin (2025).

## Theoretical Breakthrough

This algorithm represents a major theoretical advance in graph algorithms:

- **Previous best**: O(m + n log n) time (Dijkstra's algorithm)
- **This work**: O(m log^(2/3) n) time - the first to break the sorting barrier on sparse graphs
- **Model**: Comparison-addition model for real-weighted directed graphs
- **Type**: Deterministic algorithm

## Key Contributions

1. **First subquadratic improvement**: Breaks the long-standing O(m + n log n) barrier
2. **Novel techniques**: 

  - Recursive partitioning with frontier reduction
  - Sophisticated block-based data structures
  - Pivot selection algorithms
  - Combination of Dijkstra and Bellman-Ford approaches

## Algorithm Overview

The algorithm uses a divide-and-conquer approach with several key components:

### 1\. FindPivots Algorithm (Algorithm 1)

- Reduces the frontier size from potentially O(n) to O(|U|/k)
- Uses k steps of Bellman-Ford-like relaxation
- Identifies "pivot" vertices that root large subtrees

### 2\. Base Case (Algorithm 2)

- Mini Dijkstra's algorithm for level 0
- Finds k+1 closest vertices from a single source
- Sets boundary for partial execution

### 3\. Main BMSSP Algorithm (Algorithm 3)

- Recursive bounded multi-source shortest path solver
- Uses sophisticated data structure for maintaining frontiers
- Combines direct insertion and batch prepending

### 4\. Data Structure (Lemma 3.3)

- Block-based linked list with two sequences (D0, D1)
- Supports efficient Insert, BatchPrepend, and Pull operations
- Amortized time bounds crucial for overall complexity

## File Structure

- `sssp_breakthrough.h` - Main header with all declarations
- `sssp_demo.c` - Simplified demonstration implementation
- `graph_ops.c` - Basic graph operations and utilities
- `data_structure.c` - Block-based data structure implementation
- `find_pivots.c` - FindPivots algorithm implementation
- `base_case.c` - Base case algorithm
- `bmssp.c` - Main BMSSP recursive algorithm
- `main.c` - Test driver and examples
- `Makefile` - Build configuration

## Compilation

```bash
# Simple demo version
gcc -o sssp_demo sssp_demo.c -lm

# Full implementation (if all files present)
make
```

## Usage

```c
Graph* g = create_graph(n);
// Add edges...
add_edge(g, u, v, weight);

// Run the breakthrough algorithm
sssp_breakthrough(g, source);

// Access results
printf("Distance to vertex v: %f\n", g->dist[v]);
```

## Running the Demo

```bash
./sssp_demo
```

This will demonstrate:

- Algorithm on small test graphs
- Complexity analysis compared to Dijkstra
- Verification of correctness
- Theoretical improvement calculations

## Algorithm Parameters

The algorithm uses two key parameters:

- `k = ⌊log^(1/3)(n)⌋` - Controls pivot reduction
- `t = ⌊log^(2/3)(n)⌋` - Controls recursion depth

These are carefully chosen to achieve the O(m log^(2/3) n) complexity.

## Theoretical Details

### Time Complexity Analysis

The algorithm achieves O(m log^(2/3) n) time through:

- O(log n / t) recursion levels = O(log^(1/3) n) levels
- O(k + t/k) time per vertex per level
- Sophisticated frontier reduction techniques

### Space Complexity

- O(n + m) space for graph representation
- O(n) additional space for data structures

## Implementation Notes

1. **Numerical Precision**: Uses double precision floating point
2. **Graph Representation**: Adjacency lists for efficiency
3. **Memory Management**: Careful allocation/deallocation
4. **Error Handling**: Basic validation and verification

## Applications

This breakthrough has implications for:

- Route planning in transportation networks
- Network analysis and optimization
- Any application requiring single-source shortest paths
- Foundation for future graph algorithm improvements

## References

Duan, R., Mao, J., Mao, X., Shu, X., & Yin, L. (2025). Breaking the Sorting Barrier for Directed Single-Source Shortest Paths. arXiv:2504.17033v2.

## License

This implementation is provided for educational and research purposes. Please cite the original paper when using this code.

