# SSSP Breakthrough - Public API

This directory contains the clean, public API for the SSSP Breakthrough algorithm.

## Files

- **`sssp.h`** - Main public API header file
- **`USAGE.md`** - Comprehensive usage guide with examples
- **`README.md`** - This file

## Quick Integration

### 1. Include the Header

```c
#include <sssp.h>
```

### 2. Use the API

```c
// Create graph
sssp_graph_t* graph = sssp_graph_create(num_vertices);

// Add edges
sssp_graph_add_edge(graph, from, to, weight);

// Solve SSSP
sssp_result_t* result = sssp_solve(graph, source);

// Query results
double distance = sssp_result_get_distance(result, vertex);

// Clean up
sssp_result_destroy(result);
sssp_graph_destroy(graph);
```

### 3. Compile

```bash
gcc -o myapp myapp.c -lsssp -lm
```

## Key Features

### ✓ Clean API
- Opaque types hide implementation details
- Simple, intuitive function names
- Comprehensive error handling

### ✓ Well Documented
- Doxygen-style comments
- Clear parameter descriptions
- Usage examples in header

### ✓ Easy Integration
- Single header file (`sssp.h`)
- C99 compatible
- No external dependencies (except math library)

### ✓ Production Ready
- Proper error codes
- NULL-safe functions
- Memory leak free

## API Overview

### Graph Management
```c
sssp_graph_t* sssp_graph_create(int num_vertices);
sssp_status_t sssp_graph_add_edge(sssp_graph_t* graph, int from, int to, double weight);
void sssp_graph_destroy(sssp_graph_t* graph);
```

### SSSP Solver
```c
sssp_result_t* sssp_solve(sssp_graph_t* graph, int source);
```

### Result Queries
```c
double sssp_result_get_distance(const sssp_result_t* result, int vertex);
int sssp_result_get_predecessor(const sssp_result_t* result, int vertex);
bool sssp_result_is_reachable(const sssp_result_t* result, int vertex);
void sssp_result_destroy(sssp_result_t* result);
```

## Documentation

For detailed usage examples and integration guides, see:
- **[USAGE.md](./USAGE.md)** - Complete usage guide
- **[sssp.h](./sssp.h)** - API reference (in header comments)
- **[../examples/](../examples/)** - Example code

## Error Handling

All functions return appropriate error codes:

```c
typedef enum {
    SSSP_SUCCESS = 0,
    SSSP_ERROR_NULL_POINTER,
    SSSP_ERROR_INVALID_VERTEX,
    SSSP_ERROR_INVALID_EDGE,
    SSSP_ERROR_MEMORY_ALLOCATION,
    SSSP_ERROR_GRAPH_TOO_LARGE,
    SSSP_ERROR_NEGATIVE_CYCLE,
    SSSP_ERROR_INVALID_GRAPH,
    SSSP_ERROR_NOT_SOLVED
} sssp_status_t;
```

Convert to human-readable strings:
```c
const char* msg = sssp_status_to_string(status);
```

## Installation

After building the project:

```bash
sudo make install
```

The header will be installed to:
- Linux/macOS: `/usr/local/include/sssp.h`
- Or your custom CMAKE_INSTALL_PREFIX

## Examples

See the `examples/` directory for complete working examples:
- `simple_example.c` - Basic usage demonstration

## Algorithm Complexity

- **Time**: O(m^{1+o(1)}) where m = number of edges
- **Space**: O(n + m) where n = number of vertices
- **Handles**: Negative weights (but not negative cycles)

## Thread Safety

The current implementation is **not thread-safe**. Each thread should create its own graph instances.

## License

See the project root LICENSE file for licensing information.

## Support

For issues, questions, or contributions:
- See project README.md
- Check TECHNICAL_ANALYSIS.md for algorithm details
- Review examples/ for usage patterns
