# SSSP Breakthrough - Unit Tests

Comprehensive test suite for verifying correctness and robustness of the SSSP breakthrough algorithm.

## Overview

The unit test suite provides:
- ✅ **Algorithm Correctness** - Tests on known graphs with verified solutions
- ✅ **Edge Cases** - Boundary conditions and special scenarios
- ✅ **Data Structures** - MinHeap, graph operations
- ✅ **Memory Management** - Leak detection and stress tests
- ✅ **Solution Validation** - Verification of optimality conditions
- ✅ **Stress Tests** - Performance under load

**Total:** 33 comprehensive tests across 6 test suites

## Quick Start

```bash
# Build
cd build
cmake ..
make

# Run tests
./bin/sssp_tests

# Or use CTest
ctest --output-on-failure
```

## Test Framework

Custom lightweight framework with:
- Color-coded output
- Pass/fail tracking
- Assertion macros
- Test suite organization

### Assertion Macros

```c
ASSERT_TRUE(condition)                    // Condition must be true
ASSERT_FALSE(condition)                   // Condition must be false
ASSERT_NULL(ptr)                          // Pointer must be NULL
ASSERT_NOT_NULL(ptr)                      // Pointer must not be NULL
ASSERT_EQ(a, b)                           // Values must be equal
ASSERT_NEQ(a, b)                          // Values must not be equal
ASSERT_DOUBLE_EQ(a, b, epsilon)           // Floats equal within epsilon
ASSERT_ARRAY_EQ(arr1, arr2, size)         // Arrays must match
```

## Test Suites

### 1. Algorithm Correctness (8 tests)

Tests core SSSP functionality on various graph types:

**test_simple_graph_correctness**
- Small graph with known shortest paths
- Verifies exact distances

**test_single_vertex**
- Minimal graph (n=1)
- Distance to self should be 0

**test_disconnected_graph**
- Multiple components
- Unreachable vertices = ∞

**test_self_loop**
- Graph with self-edges
- Self-loops shouldn't affect paths

**test_negative_weights**
- DAG with negative edges
- Verifies correct handling

**test_comparison_with_dijkstra**
- Random graph
- Results must match Dijkstra (non-negative weights)

**test_various_graph_sizes**
- Sizes: 1, 2, 5, 10, 50, 100 vertices
- Ensures scalability

**test_complete_graph**
- All vertices connected
- All should be reachable

### 2. Edge Cases (6 tests)

Boundary conditions and special scenarios:

**test_empty_graph**
- No edges
- Only source reachable

**test_two_vertices**
- Minimal connected graph
- Basic edge test

**test_linear_chain**
- Path graph: 0→1→2→...→n
- Distance[i] = i for unit weights

**test_all_zero_weights**
- All edges weight 0
- All reachable = distance 0

**test_very_large_weights**
- Weights up to 10¹⁰⁰
- Tests numerical stability

**test_multiple_sources**
- SSSP from each vertex
- Ensures algorithm works from any source

### 3. Data Structures (4 tests)

Tests low-level data structure operations:

**test_heap_operations**
- Insert, extract_min
- Heap property maintained

**test_heap_decrease_key**
- Priority updates
- Correct reordering

**test_graph_creation_destruction**
- Memory allocation/deallocation
- No leaks

**test_edge_addition**
- Adding edges
- Edge count tracking

### 4. Memory Management (3 tests)

Detects memory leaks and validates cleanup:

**test_no_memory_leaks_small**
- 10 trials with small graphs
- Memory tracking

**test_no_memory_leaks_large**
- 5 trials with 100-vertex graphs
- Stress test allocation

**test_repeated_execution**
- Same graph, multiple SSSP calls
- Checks for accumulation errors

### 5. Solution Validation (2 tests)

Verifies optimality conditions:

**test_solution_verification**
- Known graph
- Triangle inequality check

**test_random_graph_verification**
- 5 random graphs
- Validates each solution

### 6. Stress Tests (3 tests)

Performance under demanding conditions:

**test_dense_graph**
- Complete graph (50 vertices)
- O(n²) edges

**test_sparse_large_graph**
- 500 vertices, sparse
- Real-world scale

**test_grid_graph**
- 10×10 grid
- Structured topology

## Test Output

### Success Output
```
╔═══════════════════════════════════════════════════════════════╗
║        SSSP Breakthrough - Comprehensive Unit Tests          ║
╚═══════════════════════════════════════════════════════════════╝

═══════════════════════════════════════════════════════════════
Test Suite: Algorithm Correctness
═══════════════════════════════════════════════════════════════
  Running: Simple graph correctness ... PASS
  Running: Single vertex graph ... PASS
  Running: Disconnected graph ... PASS
  ...

═══════════════════════════════════════════════════════════════
Test Summary
═══════════════════════════════════════════════════════════════

Total tests:  33
Passed:       33
Failed:       0

✓ All tests passed!
```

### Failure Output
```
  Running: Simple graph correctness ... FAIL
    Error: Values not equal
    Location: tests/unit_tests.c:123
```

## Running Specific Tests

To run a subset of tests, modify `main()`:

```c
int main(void) {
    // Comment out test suites you don't want to run

    TEST_SUITE("Algorithm Correctness");
    test_simple_graph_correctness();
    // ... other tests

    // TEST_SUITE("Edge Cases");
    // test_empty_graph();
    // ... (commented out)

    print_test_summary();
    return (tests_failed == 0) ? 0 : 1;
}
```

## Adding New Tests

### 1. Write Test Function

```c
void test_my_new_feature(void) {
    TEST("My new feature");

    // Setup
    Graph* g = create_graph(5);
    ASSERT_NOT_NULL(g);

    // Execute
    sssp_breakthrough(g, 0);

    // Verify
    ASSERT_DOUBLE_EQ(g->dist[0], 0.0, 1e-9);

    // Cleanup
    free_graph(g);
    PASS();
}
```

### 2. Add to Test Suite

```c
TEST_SUITE("My Test Suite");
test_my_new_feature();
```

### 3. Rebuild and Run

```bash
make
./bin/sssp_tests
```

## Test Coverage

### Graph Types Tested
- ✓ Simple graphs (hand-crafted)
- ✓ Random graphs
- ✓ Complete graphs
- ✓ Sparse graphs
- ✓ Grid graphs
- ✓ Path graphs
- ✓ Disconnected graphs

### Weight Scenarios
- ✓ Positive weights
- ✓ Negative weights
- ✓ Zero weights
- ✓ Very large weights (10¹⁰⁰)
- ✓ Mixed weights

### Graph Sizes
- ✓ Tiny (1-2 vertices)
- ✓ Small (5-10 vertices)
- ✓ Medium (50-100 vertices)
- ✓ Large (500 vertices)

### Edge Cases
- ✓ Empty graphs (no edges)
- ✓ Single vertex
- ✓ Self-loops
- ✓ Disconnected components
- ✓ Multiple sources

## Continuous Integration

### GitHub Actions Example

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make
      - name: Run Tests
        run: cd build && ./bin/sssp_tests
```

### Test on Multiple Platforms

```yaml
strategy:
  matrix:
    os: [ubuntu-latest, macos-latest]

runs-on: ${{ matrix.os }}
```

## Debugging Failed Tests

### 1. Identify Failure
```
  Running: Simple graph correctness ... FAIL
    Error: Values not equal
    Location: tests/unit_tests.c:123
```

### 2. Add Debug Output

```c
void test_simple_graph_correctness(void) {
    TEST("Simple graph correctness");

    Graph* g = create_test_graph_simple();
    sssp_breakthrough(g, 0);

    // Debug: print actual distances
    printf("\n  Actual distances: ");
    for (int i = 0; i < g->n; i++) {
        printf("%.2f ", g->dist[i]);
    }
    printf("\n");

    // ... assertions ...
}
```

### 3. Run with Debugger

```bash
gdb ./bin/sssp_tests
(gdb) run
(gdb) bt  # backtrace on failure
```

### 4. Enable Memory Checking

```bash
valgrind --leak-check=full ./bin/sssp_tests
```

## Performance Benchmarks

While not the primary focus, tests also serve as micro-benchmarks:

```c
void test_performance_benchmark(void) {
    TEST("Performance benchmark");

    Timer* timer = timer_create();
    Graph* g = graph_generate_sparse(1000, 3, 1.0, 100.0);

    timer_start(timer);
    sssp_breakthrough(g, 0);
    double elapsed = timer_stop(timer);

    printf("\n  Time: %.3f ms\n", elapsed * 1000);

    ASSERT_TRUE(elapsed < 1.0);  // Should complete in <1s

    timer_destroy(timer);
    free_graph(g);
    PASS();
}
```

## Known Limitations

1. **Floating-Point Precision**
   - Uses epsilon (10⁻⁹) for comparisons
   - Very large weights may have rounding errors

2. **Memory Leak Detection**
   - Basic detection, not comprehensive
   - Use Valgrind for detailed analysis

3. **Platform Dependencies**
   - Color codes work on Unix terminals
   - May not display on Windows CMD (use WSL)

4. **Test Independence**
   - Tests should be independent
   - Order shouldn't matter
   - Currently sequential execution

## Troubleshooting

### Tests fail on Windows
- Use WSL or disable color codes
- Check line endings (CRLF vs LF)

### Memory leak warnings
- Run with Valgrind for details
- Check graph cleanup in test

### Random test failures
- Set fixed seed: `utils_set_random_seed(42)`
- Check for race conditions (none expected)

### Slow test execution
- Comment out stress tests
- Reduce graph sizes in tests

## Future Enhancements

Planned additions:
- [ ] Property-based testing (randomized inputs)
- [ ] Negative cycle detection tests
- [ ] Parallel test execution
- [ ] Code coverage analysis
- [ ] Performance regression detection
- [ ] Fuzzing integration

## References

- Test framework design inspired by minimal unit test frameworks
- Graph test cases based on standard SSSP test suites
- Memory testing uses utils.h tracking functionality

## Contributing

When adding tests:
1. Follow existing naming convention: `test_<feature_name>`
2. Use descriptive test names
3. Always clean up resources
4. Add tests to appropriate suite
5. Document complex test scenarios

## See Also

- `examples/basic_usage.c` - Integration testing
- `examples/performance_test.c` - Performance testing
- `src/sssp_breakthrough.h` - Algorithm implementation
- `TECHNICAL_ANALYSIS.md` - Algorithm theory
