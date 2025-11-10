# Benchmarks

Comprehensive benchmarking and scalability analysis tools for the SSSP Breakthrough Algorithm.

## Files

### Benchmark Programs

- **`scalability_test.c`** - Main scalability testing program
  - Tests algorithm on progressively larger graphs
  - Measures time and memory usage
  - Validates O(m log^(2/3) n) complexity
  - Identifies crossover points vs Dijkstra
  - Generates CSV data for plotting

- **`compare_dijkstra.c`** - Comprehensive Dijkstra comparison
  - Implements optimized Dijkstra with binary and Fibonacci heaps
  - Side-by-side comparison on identical graphs
  - Statistical analysis with confidence intervals (95%)
  - T-tests for significance testing
  - Theoretical vs actual performance analysis
  - Detailed CSV output with all timing data

### Analysis Tools

- **`plot_scalability.py`** - Python plotting script
  - Generates publication-quality plots from CSV data
  - Multiple visualization types
  - Statistical analysis and reports
  - Requires: matplotlib, numpy, pandas, scipy

- **`plot_comparison.py`** - Comparison visualization tool
  - Side-by-side performance plots
  - Speedup analysis charts
  - Theoretical vs actual fit plots
  - Confidence interval visualizations
  - Statistical summary reports

### Documentation

- **`README.md`** - This file
- **`BENCHMARKING_GUIDE.md`** - Detailed guide for running benchmarks

## Quick Start

### Build

```bash
cd build
cmake ..
make
```

The scalability test will be built as `bin/sssp_benchmarks` (if configured).

### Run Basic Test

```bash
# Run with default settings (n: 100-10000, sparse graphs)
./scalability_test

# Run with custom parameters
./scalability_test --max-n 5000 --trials 10 --output-dir results/
```

### Generate Plots

```bash
# Install Python dependencies
pip install matplotlib numpy pandas scipy

# Generate plots
python plot_scalability.py scalability_results.csv
```

## Output Files

### Scalability Test Outputs

1. **`scalability_results.csv`** - Raw performance data
   - Columns: n, m, density, times, speedup, memory, etc.
   - Can be imported into Excel, R, Python, etc.

2. **`scalability_summary.txt`** - Human-readable summary
   - Configuration details
   - Performance summary table
   - Complexity analysis
   - Crossover point identification
   - Memory usage summary

3. **Generated plots** (via Python script):
   - `time_vs_size.png` - Execution time vs graph size
   - `time_vs_size_loglog.png` - Log-log scale plot
   - `speedup.png` - Speedup vs Dijkstra
   - `complexity_validation.png` - Time per complexity unit
   - `complexity_fit.png` - Fit to theoretical model
   - `time_vs_edges.png` - Time vs number of edges
   - `memory_usage.png` - Memory usage analysis
   - `scalability_summary.png` - All-in-one summary figure

### Comparison Test Outputs

1. **`comparison_results.csv`** - Detailed comparison data
   - Columns: n, m, all algorithm timings with confidence intervals
   - Statistical measures for each algorithm
   - Speedup calculations
   - Theoretical complexity values

2. **`comparison_summary.txt`** - Statistical analysis report
   - Performance comparison tables with error bars
   - T-test results for significance
   - Confidence interval analysis
   - Crossover point identification
   - Theoretical vs actual validation

3. **Generated plots** (via `plot_comparison.py`):
   - `comparison_performance.png` - All algorithms with confidence intervals
   - `comparison_speedup.png` - Speedup analysis (2 subplots)
   - `comparison_loglog.png` - Log-log comparison
   - `comparison_theoretical.png` - Theoretical fit validation
   - `comparison_relative.png` - Normalized performance
   - `comparison_variance.png` - Standard deviation comparison
   - `comparison_summary.png` - All-in-one comprehensive view
   - `comparison_report.txt` - Statistical summary

## Command Line Options

### scalability_test

```bash
./scalability_test [options]

Options:
  --min-n <size>         Minimum graph size (default: 100)
  --max-n <size>         Maximum graph size (default: 10000)
  --trials <count>       Number of trials per size (default: 5)
  --output-dir <dir>     Output directory (default: current)
  --graph-type <type>    Graph type: sparse, medium, dense, grid
  --no-dijkstra          Skip Dijkstra comparison
  --no-memory            Skip memory measurement
  --verbose              Verbose output
  --help                 Show help message
```

### compare_dijkstra

```bash
./compare_dijkstra [options]

Options:
  --min-n <size>         Minimum graph size (default: 100)
  --max-n <size>         Maximum graph size (default: 5000)
  --trials <count>       Number of trials per size (default: 20)
  --graph-type <type>    Graph type: sparse, medium, dense
  --output-dir <dir>     Output directory (default: current)
  --verbose              Verbose output
  --help                 Show help message
```

**Note:** `compare_dijkstra` runs more trials by default (20 vs 5) for better statistical confidence.

## Graph Types

| Type | Description | Edge Density | Use Case |
|------|-------------|--------------|----------|
| `sparse` | m ≈ 3n | Very low | Best case for breakthrough |
| `medium` | m ≈ 10n | Low | Typical case |
| `dense` | m ≈ n^1.5 | Medium | Stress test |
| `grid` | 2D lattice | Low | Structured graphs |
| `complete` | All edges | Very high | Worst case (use small n) |

## Example Workflows

### 1. Algorithm Comparison (Breakthrough vs Dijkstra)

```bash
# Run comprehensive comparison
./compare_dijkstra --max-n 3000 --trials 20 --graph-type sparse

# Generate comparison plots
python plot_comparison.py comparison_results.csv

# View results
cat comparison_summary.txt
open comparison_*.png
```

This will compare:
- Breakthrough algorithm
- Dijkstra with binary heap
- Dijkstra with Fibonacci heap

Output includes statistical analysis, confidence intervals, and t-tests.

### 2. Basic Scalability Test

```bash
# Test on sparse graphs up to 5000 vertices
./scalability_test --max-n 5000 --trials 5 --graph-type sparse

# Generate plots
python plot_scalability.py scalability_results.csv
```

### 2. Compare Graph Types

```bash
mkdir results
cd results

# Test sparse graphs
../scalability_test --graph-type sparse --output-dir sparse/
python ../plot_scalability.py sparse/scalability_results.csv

# Test dense graphs
../scalability_test --graph-type dense --output-dir dense/
python ../plot_scalability.py dense/scalability_results.csv

# Compare results
```

### 3. Find Crossover Point

```bash
# Run with Dijkstra comparison on medium-density graphs
./scalability_test --max-n 10000 --trials 10 --graph-type medium

# Check scalability_summary.txt for crossover analysis
cat scalability_summary.txt | grep -A 5 "Crossover Analysis"
```

### 4. Memory Profiling

```bash
# Test with memory measurement
./scalability_test --max-n 5000 --measure-memory

# View memory usage
python plot_scalability.py scalability_results.csv
open memory_usage.png
```

### 5. Large-Scale Test

```bash
# Test up to 100,000 vertices (may take a while!)
./scalability_test --min-n 1000 --max-n 100000 --trials 3 \
    --graph-type sparse --output-dir large_scale/

# Generate plots
python plot_scalability.py large_scale/scalability_results.csv
```

## Understanding the Results

### Complexity Validation

The key metric for validating O(m log^(2/3) n) complexity is the **time per complexity unit**:

```
Time per complexity unit = Time / (m * log^(2/3) n)
```

If the algorithm follows O(m log^(2/3) n), this ratio should remain **approximately constant** as graph size increases.

**Interpreting the complexity validation plot:**
- **Flat line** = Perfect O(m log^(2/3) n) complexity
- **Slight upward trend** = Acceptable (constant factors, overhead)
- **Strong upward trend** = Higher complexity than claimed

### R² Value

The R² (coefficient of determination) measures how well the data fits the theoretical model:

- **R² > 0.95**: Excellent fit
- **R² > 0.85**: Good fit
- **R² > 0.70**: Acceptable fit
- **R² < 0.70**: Poor fit (investigate further)

### Speedup Analysis

**Speedup = Dijkstra time / Breakthrough time**

- **Speedup > 1.0**: Breakthrough is faster
- **Speedup = 1.0**: Equal performance
- **Speedup < 1.0**: Dijkstra is faster

The **crossover point** is where speedup crosses 1.0, indicating when the breakthrough algorithm becomes faster than Dijkstra.

### Memory Usage

Memory usage typically grows as:
- **O(n + m)** for the graph structure
- Additional memory for algorithm data structures

Monitor the **memory per vertex** metric to ensure reasonable scaling.

## Tips and Best Practices

### For Accurate Results

1. **Close other applications** to minimize system noise
2. **Run multiple trials** (5-10) for statistical confidence
3. **Use release build** with optimizations enabled
4. **Warm up the cache** by running once before timing
5. **Test on dedicated hardware** if possible

### For Different Hardware

Adjust test parameters based on available memory:

- **8 GB RAM**: max-n ≤ 10,000
- **16 GB RAM**: max-n ≤ 50,000
- **32+ GB RAM**: max-n ≤ 100,000+

### For Publication

1. Run with `--trials 10` for better statistics
2. Test multiple graph types
3. Generate all plots
4. Include R² values and error bars
5. Document hardware specifications

## Interpreting Plots

### Time vs Size (Linear)

Shows absolute performance. Look for:
- Subquadratic growth (curves flatten for large n)
- Gap between Breakthrough and Dijkstra growing

### Time vs Size (Log-Log)

Reveals complexity. Slope indicates:
- Slope ~ 1.0: Linear (O(m))
- Slope ~ 1.5: Superlinear (O(m log n))
- Slope ~ 2.0: Quadratic (O(n²))

### Speedup Plot

Shows practical advantage:
- When does Breakthrough win?
- How much faster?
- Does advantage grow with size?

### Complexity Fit

Validates theoretical claims:
- Points close to line = good fit
- High R² = model explains data well

## Troubleshooting

### Test Runs Too Slowly

- Reduce `--max-n`
- Reduce `--trials`
- Use `--no-dijkstra` to skip comparison
- Use sparse graphs

### Out of Memory

- Reduce `--max-n`
- Use sparse graphs instead of dense
- Check system memory availability

### Plots Don't Generate

```bash
# Install dependencies
pip install matplotlib numpy pandas scipy

# Check Python version (need 3.6+)
python --version
```

### Poor Fit to Complexity Model

Possible causes:
- Graph size too small (constant factors dominate)
- System noise (close other apps, run more trials)
- Implementation issue (verify correctness first)

## Advanced Usage

### Custom Analysis

The CSV output can be analyzed with any tool:

```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('scalability_results.csv')

# Custom analysis
print(df.describe())
print(df.corr())

# Custom plots
plt.scatter(df['m'], df['breakthrough_mean'])
plt.xlabel('Edges')
plt.ylabel('Time (s)')
plt.show()
```

### Batch Testing

```bash
# Test multiple configurations
for type in sparse medium dense; do
    ./scalability_test --graph-type $type --output-dir results_$type/
    python plot_scalability.py results_$type/scalability_results.csv
done
```

### Integration with CI/CD

```bash
# Quick smoke test (runs in ~1 minute)
./scalability_test --max-n 1000 --trials 3 --no-dijkstra

# Check if complexity is reasonable
python -c "
import pandas as pd
df = pd.read_csv('scalability_results.csv')
assert df['time_per_complexity'].std() / df['time_per_complexity'].mean() < 0.5
print('✓ Complexity validation passed')
"
```

## References

### Theoretical Background

- Bernstein (2024): "Single-Source Shortest Paths in O(m^{1+epsilon}) Time"
- Classic Dijkstra: O((m+n) log n)
- Target complexity: O(m * log^(2/3) n)

### Related Work

- [DIMACS Shortest Path Challenge](http://www.dis.uniroma1.it/challenge9/)
- [Algorithm Performance Benchmarking](https://arxiv.org/abs/1811.05382)

## Support

For issues or questions:
1. Check this README
2. Review `scalability_summary.txt` output
3. Examine generated plots
4. Open an issue with performance data attached
