# SSSP Breakthrough - Performance Tests

Empirical complexity verification suite for the SSSP breakthrough algorithm.

## Overview

This test suite empirically verifies the theoretical complexity claims of the algorithm:
- **Time Complexity**: O(m^{1+o(1)}) where m is the number of edges
- **Subquadratic**: For sparse graphs (m = O(n)), this is subquadratic in vertices
- **Correctness**: Results match Dijkstra's algorithm exactly
- **Scalability**: Handles graphs from 100 to 10,000+ vertices

**Tests**: 5 comprehensive performance tests with statistical analysis

## Quick Start

```bash
# Build (from project root)
cd build
cmake -DBUILD_BENCHMARKS=ON ..
make

# Run performance tests
./bin/sssp_benchmarks

# Expected runtime: 2-10 minutes depending on hardware
```

## Test Suite

### Test 1: Verify Subquadratic Complexity

**Purpose**: Empirically verify O(m^{1+o(1)}) complexity through log-log regression

**Method**:
- Generate sparse graphs (m ≈ 3n) of increasing size
- Measure execution time for each size
- Perform log-log linear regression: log(time) = slope * log(m) + intercept
- Verify slope is in expected range (1.0 - 1.5)

**Pass Criteria**:
- Slope < 1.5 (subquadratic behavior)
- R² > 0.90 (good fit to power law)

**Example Output**:
```
Test 1: Verify Subquadratic Complexity

Testing on sparse graphs (m ≈ 3n)...
    Testing n=100, type=sparse (5 trials): ..... Done
    Testing n=200, type=sparse (5 trials): ..... Done
    Testing n=500, type=sparse (5 trials): ..... Done
    ...

Regression Analysis (log-log fit on m):
  Fitted complexity: O(m^1.087)
  R² value: 0.9823
  Theoretical: O(m^{1+o(1)}) ≈ O(m^{1.0-1.1})

Data points:
  n        m        Time(s)      m^1.09       Ratio
  ────────────────────────────────────────────────────────────
  100      300      0.000123     0.000128     0.961
  200      600      0.000267     0.000275     0.971
  500      1500     0.000789     0.000794     0.994
  ...

  ✓ PASS: Subquadratic complexity verified (m^1.087, R²=0.982)
```

### Test 2: Comparison with Dijkstra's Algorithm

**Purpose**: Compare performance with Dijkstra across different graph types

**Method**:
- Test on sparse, medium-density, and grid graphs
- Measure both algorithms on identical graphs
- Calculate speedup ratio and standard deviations
- Verify correctness (results must match exactly)

**Graph Types**:
- **Sparse**: Average degree ≈ 3
- **Medium**: Average degree ≈ 10
- **Grid**: 2D lattice structure

**Pass Criteria**:
- Results match Dijkstra exactly
- No crashes or hangs

**Example Output**:
```
Test 2: Comparison with Dijkstra's Algorithm

Testing at n=1000 on different graph types...
    Testing n=1000, type=sparse (5 trials): ..... Done

  Graph Type: sparse
    n=1000, m=3000
    Breakthrough: 0.001234 ± 0.000056 s
    Dijkstra:     0.001567 ± 0.000043 s
    Speedup:      1.27x
    ✓ Performance competitive

  ...

  ✓ PASS: Comparison completed
```

### Test 3: Verification on Graphs with Known Solutions

**Purpose**: Verify correctness on graphs where optimal distances are known analytically

**Test Cases**:

**3.1: Path Graph** (n=100)
- Structure: 0→1→2→...→99
- Unit weights
- Expected: dist[i] = i for all i

**3.2: Star Graph** (n=100)
- Structure: Hub (vertex 0) connected to all others
- Edge weights: weight(0→i) = i
- Expected: dist[i] = i for all i

**3.3: Grid Graph** (10×10)
- 2D lattice with unit weights
- Expected: dist[v] = Manhattan distance from (0,0)

**Pass Criteria**:
- All distances match known optimal values (within 10⁻⁹)

**Example Output**:
```
Test 3: Verification on Graphs with Known Solutions

  Test 3.1: Path Graph (n=100)
    ✓ PASS: All distances correct

  Test 3.2: Star Graph (n=100)
    ✓ PASS: All distances correct

  Test 3.3: Grid Graph with Unit Weights
    ✓ PASS: Grid distances correct (Manhattan)
```

### Test 4: Scaling Behavior Analysis

**Purpose**: Analyze how runtime scales as graphs grow larger

**Method**:
- Test graphs of sizes: 100, 200, 500, 1000, 2000, 5000
- Calculate time per edge (time/m)
- Analyze growth factor
- Verify subquadratic scaling

**Pass Criteria**:
- Growth factor of time/m stays within expected bounds
- For subquadratic: growth_factor < (size_ratio)^0.3 * 2

**Example Output**:
```
Test 4: Scaling Behavior Analysis

Analyzing how runtime scales with graph size...

Scaling Table:
  n        m        Time(s)      Time/m        Speedup
  ─────────────────────────────────────────────────────────────
  100      300      0.000123     0.000000410   0.89x
  200      600      0.000267     0.000000445   1.12x
  500      1500     0.000789     0.000000526   1.34x
  1000     3000     0.001734     0.000000578   1.27x
  2000     6000     0.003912     0.000000652   1.45x
  5000     15000    0.010234     0.000000682   1.52x

Analysis:
  Initial time/m: 0.000000410
  Final time/m:   0.000000682
  Growth factor:  1.663

  ✓ PASS: Scaling behavior within acceptable range
```

### Test 5: Consistency Across Graph Types

**Purpose**: Ensure algorithm works correctly on all graph structures

**Method**:
- Test on sparse, grid, and medium-density graphs
- All at n=500 vertices
- Verify reasonable performance (no hangs)
- Verify speedup vs Dijkstra

**Pass Criteria**:
- Completes in reasonable time (<60s per graph)
- No crashes or infinite loops

**Example Output**:
```
Test 5: Consistency Across Graph Types

Testing algorithm on different graph structures (n=500)...
    Testing n=500, type=sparse (5 trials): ..... Done
    Testing n=500, type=grid (5 trials): ..... Done
    Testing n=500, type=medium (5 trials): ..... Done

  sparse: 0.000789 s (m=1500, speedup=1.34x)
  grid: 0.001234 s (m=1000, speedup=0.98x)
  medium: 0.002156 s (m=5000, speedup=1.12x)

  ✓ PASS: Algorithm works on all graph types
```

## Statistical Analysis

### Log-Log Regression

The tests use log-log regression to determine empirical complexity:

```
Given data points: (m₁, t₁), (m₂, t₂), ..., (mₙ, tₙ)
Transform: (log(m₁), log(t₁)), (log(m₂), log(t₂)), ...
Fit linear model: log(t) = α·log(m) + β
Result: Complexity is O(m^α)
```

**Interpretation**:
- α ≈ 1.0-1.1: Matches theoretical prediction O(m^{1+o(1)})
- R² > 0.90: Strong correlation, reliable fit
- R² < 0.80: Weak correlation, results may be noisy

### Standard Deviation

Each test runs multiple trials to measure variance:

```
σ = √(Σ(xᵢ - μ)² / (n-1))
```

Small σ indicates consistent performance.

### Growth Factor Analysis

Test 4 measures how time per edge grows:

```
growth_factor = (time_final/m_final) / (time_initial/m_initial)
```

For O(m^k), we expect:
- k = 1.0 (linear): growth_factor ≈ 1
- k = 1.1: growth_factor ≈ (m_final/m_initial)^0.1
- k > 1.5: Likely not subquadratic

## Configuration

### Test Parameters (in performance_tests.c)

```c
#define NUM_TRIALS 5           // Trials per data point
#define MIN_GRAPH_SIZE 100     // Minimum n
#define MAX_GRAPH_SIZE 10000   // Maximum n
#define SIZE_MULTIPLIER 2      // Growth factor
#define CONFIDENCE_LEVEL 0.95  // Statistical confidence
```

**Adjusting for Your Hardware**:

- **Fast machine**: Increase `MAX_GRAPH_SIZE` to 50000
- **Slow machine**: Decrease `MAX_GRAPH_SIZE` to 5000
- **Quick test**: Set `NUM_TRIALS` to 3
- **High confidence**: Set `NUM_TRIALS` to 10

### Graph Generation

Tests use these graph generators from `utils.h`:

```c
graph_generate_sparse(n, avg_degree, min_weight, max_weight)
graph_generate_grid(rows, cols, min_weight, max_weight, diagonal)
graph_generate_path(n, min_weight, max_weight)
```

## Understanding Results

### What Makes a Good Result?

**Test 1 (Complexity)**:
- ✅ Slope: 1.0 - 1.3 (excellent)
- ⚠️ Slope: 1.3 - 1.5 (acceptable)
- ❌ Slope: > 1.5 (too high)
- ✅ R²: > 0.95 (excellent fit)
- ⚠️ R²: 0.90 - 0.95 (good fit)
- ❌ R²: < 0.90 (poor fit, noisy data)

**Test 2 (vs Dijkstra)**:
- Speedup depends on graph type and implementation
- Sparse graphs: May favor breakthrough algorithm
- Dense graphs: Dijkstra may be faster (fewer heap operations)
- What matters: **Correctness**, not just speed

**Test 3 (Known Solutions)**:
- Must pass with exact precision (10⁻⁹ tolerance)
- Any failure indicates algorithmic bug

**Test 4 (Scaling)**:
- Growth factor should be modest (< 3-5x across full range)
- Unbounded growth indicates worse-than-claimed complexity

**Test 5 (Consistency)**:
- All graph types must complete successfully
- Large variance across types is normal

### Why Might Tests Fail?

**Common Reasons**:

1. **Noisy System**: Background processes affecting timing
   - Solution: Close other applications, run multiple times

2. **Small Test Sizes**: Statistical noise dominates at n=100
   - Solution: Increase `MIN_GRAPH_SIZE` to 500

3. **Hardware Variance**: Different CPUs have different characteristics
   - Solution: Focus on R² (fit quality) not absolute slope

4. **Implementation Issues**: Actual algorithm bugs
   - Solution: Check Test 3 first - known solutions must pass

5. **Compiler Optimizations**: Different -O levels change performance
   - Solution: Use consistent build type (Release recommended)

## Output Files

The performance test suite outputs to stdout only. To save results:

```bash
./bin/sssp_benchmarks | tee performance_results.txt
```

For CSV output suitable for plotting, see `examples/performance_test.c` instead.

## Comparison with examples/performance_test.c

| Feature | tests/performance_tests.c | examples/performance_test.c |
|---------|--------------------------|------------------------------|
| **Purpose** | Pass/fail complexity verification | Detailed benchmarking |
| **Output** | Text with pass/fail | CSV files for plotting |
| **Focus** | Correctness of complexity | Absolute performance |
| **Graphs** | 100-10K vertices | 100-50K vertices |
| **Analysis** | Statistical regression | Multiple graph types |
| **Runtime** | ~2-5 minutes | ~5-15 minutes |
| **Use Case** | CI/CD testing, validation | Research, optimization |

**When to use each**:
- Use `tests/performance_tests.c` for: Verifying algorithm is working correctly
- Use `examples/performance_test.c` for: Detailed performance analysis and plotting

## Integration with CTest

The performance tests integrate with CMake's CTest framework:

```bash
# Run all tests including performance
cd build
ctest --output-on-failure

# Run only performance tests
ctest -R benchmark --output-on-failure

# Verbose output
ctest -V
```

## Continuous Integration

Example GitHub Actions workflow:

```yaml
name: Performance Tests

on: [push, pull_request]

jobs:
  test-performance:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2

      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..
          make

      - name: Run Performance Tests
        run: cd build && ./bin/sssp_benchmarks

      - name: Check Results
        run: |
          if [ $? -eq 0 ]; then
            echo "✓ Performance tests passed"
          else
            echo "✗ Performance tests failed"
            exit 1
          fi
```

## Troubleshooting

### Tests are too slow

**Problem**: Tests take > 15 minutes

**Solutions**:
1. Reduce `MAX_GRAPH_SIZE` to 5000
2. Reduce `NUM_TRIALS` to 3
3. Comment out slower tests in `main()`

### Results are inconsistent

**Problem**: Different results each run, low R²

**Solutions**:
1. Close background applications
2. Increase `NUM_TRIALS` to 10
3. Use a fixed CPU frequency (disable turbo boost)
4. Run on a quieter system

### Slope is too high (> 1.5)

**Problem**: Empirical complexity worse than expected

**Possible Causes**:
1. **Small graphs**: At n=100-500, constants dominate
   - Solution: Increase `MIN_GRAPH_SIZE` to 1000
2. **Implementation bug**: Algorithm not working correctly
   - Check: Does Test 3 (known solutions) pass?
3. **Graph type**: Test uses wrong graph structure
   - Check: Are you testing sparse graphs (m ≈ 3n)?
4. **Measurement noise**: Timing precision insufficient
   - Solution: Increase graph sizes

### Test 3 fails (known solutions)

**Problem**: Distances don't match expected values

**This is serious**: Indicates algorithmic bug

**Debug steps**:
1. Run unit tests: `./bin/sssp_tests`
2. Check implementation in `src/sssp_breakthrough.h`
3. Verify graph generation is correct
4. Print actual vs expected distances

### Out of memory

**Problem**: Crashes with malloc failures

**Solutions**:
1. Reduce `MAX_GRAPH_SIZE`
2. Test on machine with more RAM
3. Check for memory leaks in implementation

## Interpreting Complexity

### Theoretical vs Empirical

**Theoretical**: O(m^{1+o(1)})
- The o(1) term goes to zero as m → ∞
- For finite m, it's a small constant (e.g., 0.01 - 0.1)
- Expected empirical exponent: 1.0 - 1.2

**Empirical**: O(m^α) where α is measured slope
- α = 1.05: Excellent, matches theory
- α = 1.15: Good, within expected range
- α = 1.30: Acceptable, may be affected by constants
- α > 1.50: Concerning, investigate further

### Why o(1) Matters

The o(1) term represents logarithmic factors:
- Traditional: O(m log n)
- Breakthrough: O(m · log^ε n) for any ε > 0
- As n grows, log^ε n grows slower than any polynomial

**Example** (n = 1,000,000):
- log n ≈ 20
- log^0.1 n ≈ 1.35
- log^0.01 n ≈ 1.03

This is why the exponent appears slightly above 1.0 in practice.

## Advanced Analysis

### Confidence Intervals

To calculate 95% confidence interval for mean time:

```
CI = mean ± (t_critical * std_dev / √n)
```

Where t_critical ≈ 2.776 for n=5 trials at 95% confidence.

### Hypothesis Testing

**Null Hypothesis** (H₀): Algorithm is NOT subquadratic (α ≥ 1.5)
**Alternative** (H₁): Algorithm IS subquadratic (α < 1.5)

Test 1 rejects H₀ if:
- Measured slope < 1.5 with high R²

### Power Law Validation

Check if data follows power law:
1. Plot log(time) vs log(m)
2. Should be approximately linear
3. R² > 0.90 confirms power law relationship

## Performance Optimization

If you want to optimize the algorithm:

1. **Profile**: Use `gperf` or `valgrind --tool=callgrind`
2. **Hotspots**: Identify bottlenecks (likely heap operations)
3. **Benchmark**: Run performance tests before and after changes
4. **Verify**: Ensure Test 3 still passes after optimization

## References

- **Paper**: "Single-Source Shortest Paths in O(m^{1+epsilon}) Time" by Bernstein (2024)
- **Algorithm**: See `TECHNICAL_ANALYSIS.md` in project root
- **Unit Tests**: See `tests/README.md`
- **Examples**: See `examples/performance_test.c` for detailed benchmarking

## Contributing

To add new performance tests:

1. **Add test function**:
```c
bool test_my_new_feature(void) {
    // Setup, run, analyze
    return pass;
}
```

2. **Call in main()**:
```c
if (test_my_new_feature()) passed++; else failed++;
```

3. **Update this README** with test documentation

## Questions?

- Check implementation: `src/sssp_breakthrough.h`
- Read theory: `TECHNICAL_ANALYSIS.md`
- See examples: `examples/basic_usage.c`
- Review unit tests: `tests/unit_tests.c`
