# SSSP Breakthrough - Performance Testing Guide

This guide explains how to run comprehensive performance tests comparing the SSSP Breakthrough algorithm with Dijkstra's algorithm.

## Quick Start

```bash
# 1. Build the performance test
cd build
cmake --build .

# 2. Run the test suite
./bin/examples/performance_test

# 3. Plot the results (requires Python + matplotlib)
cd ..
python3 examples/plot_performance.py
```

## What Gets Tested

### Graph Sizes
- **Small**: 100-500 vertices
- **Medium**: 1K-5K vertices
- **Large**: 10K-50K vertices
- **Very Large**: 100K vertices (optional)

### Graph Types
- **Sparse**: Average degree ~3 (realistic for many networks)
- **Medium**: Average degree ~10
- **Dense**: Average degree ~50
- **Grid**: 2D lattice structure
- **DAG**: Directed acyclic graphs

### Metrics Measured
- ✓ Execution time (averaged over multiple trials)
- ✓ Memory usage (peak allocation)
- ✓ Speedup ratio (Dijkstra / Breakthrough)
- ✓ Correctness verification
- ✓ Number of vertices and edges

## Running the Tests

### Standard Test Suite

```bash
./bin/examples/performance_test
```

**Runs:**
- 15 different test configurations
- Multiple trials per configuration (3-10 trials)
- Total time: ~5-15 minutes (depending on hardware)

**Output:**
```
Testing: n=1000, type=sparse, trials=5
  Progress: 5/5 trials completed

┌────────┬─────────┬──────────┬──────────────┬──────────────┬──────────┬─────────┬────────┐
│   n    │    m    │   Type   │ Breakthrough │   Dijkstra   │  Speedup │ Memory  │ Match? │
├────────┼─────────┼──────────┼──────────────┼──────────────┼──────────┼─────────┼────────┤
│   1000 │    1500 │ sparse   │    1.234 ms  │    0.987 ms  │   0.80x  │   2.3M  │   ✓    │
│   5000 │    7500 │ sparse   │   12.456 ms  │   15.234 ms  │   1.22x  │  15.7M  │   ✓    │
...
```

### Quick Test Mode

For faster testing during development:

```bash
./bin/examples/performance_test --quick
```

Runs only the first 6 tests (~2 minutes).

### Custom Configuration

Edit `performance_test.c` and modify the `tests[]` array:

```c
TestConfig tests[] = {
    {vertices, edges, "graph_type", num_trials},
    {1000,     -1,    "sparse",     5},  // -1 = auto-calculate edges
    {5000,     -1,    "medium",     3},
    // Add your own...
};
```

## Output Files

### performance_results.csv

CSV format suitable for plotting and analysis:

```csv
n,m,graph_type,time_breakthrough,time_dijkstra,speedup,memory_mb,match
100,150,sparse,0.000234,0.000187,0.799,0.5,1
500,750,sparse,0.001234,0.001456,1.180,2.3,1
```

**Columns:**
- `n` - Number of vertices
- `m` - Number of edges
- `graph_type` - Type of graph tested
- `time_breakthrough` - Breakthrough time in seconds
- `time_dijkstra` - Dijkstra time in seconds
- `speedup` - Ratio (>1 means breakthrough is faster)
- `memory_mb` - Peak memory in megabytes
- `match` - 1 if results match, 0 otherwise

### performance_results.txt

Human-readable detailed results:

```
SSSP Breakthrough Algorithm - Performance Test Results
======================================================

Test Date: Nov 9 2024
Number of Tests: 15

Test 1:
  Graph: 100 vertices, 150 edges (sparse)
  Breakthrough: 0.000234 seconds
  Dijkstra:     0.000187 seconds
  Speedup:      0.80x
  Memory:       0.50 MB
  Results match: Yes
  Trials run:    10
...
```

## Plotting Results

### Using Python (Recommended)

```bash
python3 examples/plot_performance.py
```

**Requirements:**
```bash
pip install pandas matplotlib numpy
```

**Generates:**
- `performance_speedup.png` - Speedup vs graph size
- `performance_time.png` - Absolute time comparison
- `performance_memory.png` - Memory usage analysis
- `performance_combined.png` - All-in-one comprehensive view

### Manual Plotting with Python

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv('performance_results.csv')

# Plot speedup
plt.figure(figsize=(10, 6))
for graph_type in df['graph_type'].unique():
    data = df[df['graph_type'] == graph_type]
    plt.plot(data['n'], data['speedup'],
             marker='o', label=graph_type)

plt.xlabel('Graph Size (vertices)')
plt.ylabel('Speedup (Dijkstra / Breakthrough)')
plt.axhline(y=1.0, color='r', linestyle='--')
plt.legend()
plt.grid(True)
plt.savefig('speedup.png')
```

### Using gnuplot

```bash
gnuplot -e "
set terminal png size 1200,800;
set output 'performance.png';
set datafile separator ',';
set xlabel 'Graph Size (vertices)';
set ylabel 'Speedup';
set title 'SSSP Performance Comparison';
set grid;
set logscale x;
plot 'performance_results.csv' using 1:6 with linespoints title 'Speedup'
"
```

### Using Excel/LibreOffice

1. Open `performance_results.csv`
2. Create scatter plot with `n` on X-axis
3. Add series for `speedup`, `time_breakthrough`, `time_dijkstra`
4. Format as needed

## Understanding the Results

### Speedup Ratio

```
Speedup = Dijkstra Time / Breakthrough Time
```

- **Speedup > 1.0**: Breakthrough is faster
- **Speedup < 1.0**: Dijkstra is faster
- **Speedup = 1.0**: Equal performance

### Expected Performance Characteristics

#### Small Graphs (n < 500)
- Dijkstra often faster due to lower constant factors
- Breakthrough has more overhead for small inputs
- Speedup typically 0.5x - 0.9x

#### Medium Graphs (500 < n < 5000)
- Performance is competitive
- Results vary by graph type and density
- Speedup typically 0.8x - 1.2x

#### Large Graphs (n > 5000)
- Breakthrough's theoretical advantages may appear
- Especially on sparse graphs
- Speedup typically 1.0x - 2.0x (varies significantly)

#### Very Large Graphs (n > 50000)
- Breakthrough algorithm shows best results
- Asymptotic complexity benefits become visible
- Speedup can exceed 2.0x on optimal graph types

### Graph Type Impact

| Graph Type | Typical Speedup | Best For |
|------------|----------------|----------|
| Sparse | 0.9x - 1.5x | Breakthrough |
| Medium | 0.7x - 1.2x | Competitive |
| Dense | 0.5x - 0.9x | Dijkstra |
| Grid | 0.8x - 1.3x | Competitive |
| DAG | 1.0x - 1.8x | Breakthrough |

**Note:** These are typical values; actual results depend on hardware, implementation, and specific graph structure.

## Memory Analysis

### Memory Usage Patterns

The breakthrough algorithm typically uses:
- **Base**: O(n + m) for graph storage
- **Working**: Additional data structures for algorithm

Monitor `memory_mb` column in results to track peak usage.

### Memory vs Graph Size

```
Expected memory (MB) ≈ (n + m) × sizeof(structures) / 1024²

For sparse graph (m ≈ 3n):
  n = 1,000   →  ~2-5 MB
  n = 10,000  →  ~20-50 MB
  n = 100,000 →  ~200-500 MB
```

## Interpreting the Summary

After tests complete, you'll see:

```
═══════════════════════════════════════════════════════════════
Performance Summary
═══════════════════════════════════════════════════════════════

Speedup Statistics:
  Average speedup:  1.05x
  Maximum speedup:  1.78x
  Minimum speedup:  0.63x

Algorithm Performance:
  Breakthrough faster: 9 tests (60.0%)
  Dijkstra faster:     6 tests (40.0%)

Correctness: All results match ✓
```

### What to Look For

✓ **All results match**: Correctness verified
✓ **Average speedup > 1.0**: Breakthrough faster overall
✓ **Max speedup high**: Shows potential on optimal cases
✓ **Consistent results**: Across multiple graph types

## Troubleshooting

### "Memory allocation failed"
- Reduce graph sizes in test configuration
- Close other applications
- Run with `--quick` flag

### Tests take too long
- Use `--quick` mode
- Reduce `num_trials` in test configuration
- Test fewer/smaller graphs

### Results don't match
- This indicates a bug - report it!
- Check for floating-point precision issues
- Verify graph generation is deterministic

### Out of memory on large tests
- Comment out very large tests (100K vertices)
- Increase system swap space
- Test on a machine with more RAM

## Advanced Usage

### Adding Custom Tests

Edit `performance_test.c`:

```c
TestConfig custom_tests[] = {
    // Your configuration
    {n, m, "type", trials},

    // Example: test very sparse graphs
    {10000, 10001, "tree", 5},  // Tree structure

    // Example: test specific density
    {5000, 25000, "custom", 3},  // Custom edge count
};
```

### Benchmarking Specific Graphs

Load your own graph:

```c
Graph* graph = graph_read_edge_list("my_graph.txt");
// ... run timing tests ...
```

### Comparing with Other Algorithms

Add implementations of Bellman-Ford, Floyd-Warshall, etc. and include in comparisons.

## Performance Optimization Tips

### For Best Breakthrough Performance
- Use sparse graphs (m = O(n))
- Larger graph sizes (n > 10K)
- DAG structures
- Graphs with long paths

### For Best Dijkstra Performance
- Dense graphs
- Smaller graphs (n < 1K)
- Complete or near-complete graphs

## Automation

### Run Tests Periodically

```bash
#!/bin/bash
# performance_check.sh

cd build
./bin/examples/performance_test > ../perf_$(date +%Y%m%d).log
cd ..
python3 examples/plot_performance.py
```

### Continuous Integration

```yaml
# .github/workflows/performance.yml
- name: Run Performance Tests
  run: |
    mkdir build && cd build
    cmake ..
    make performance_test
    ./bin/examples/performance_test --quick
```

## Citation

If you use these performance results in a paper or presentation:

```bibtex
@software{sssp_breakthrough,
  title = {SSSP Breakthrough Algorithm Implementation},
  year = {2024},
  note = {Performance testing framework}
}
```

## Further Reading

- **Algorithm Details**: See `TECHNICAL_ANALYSIS.md`
- **Implementation**: See `src/sssp_breakthrough.h`
- **Basic Usage**: See `examples/basic_usage.c`
- **Original Paper**: See `paper/` directory

## Support

Issues with performance testing:
1. Check this README
2. Verify build configuration
3. Review test output logs
4. Check memory availability

For questions or bugs, see project README.
