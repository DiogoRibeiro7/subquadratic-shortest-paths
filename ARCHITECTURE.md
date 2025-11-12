# Repository Architecture

This document provides a comprehensive guide to the structure and organization of the SSSP Breakthrough Algorithm repository.

## Table of Contents

- [Overview](#overview)
- [Directory Structure](#directory-structure)
- [Component Architecture](#component-architecture)
- [Data Flow](#data-flow)
- [Build System](#build-system)
- [Testing Framework](#testing-framework)
- [Documentation Organization](#documentation-organization)
- [Navigation Guide](#navigation-guide)

--------------------------------------------------------------------------------

## Overview

The repository implements the breakthrough O(m log^(2/3) n) single-source shortest path algorithm with a clean separation between:

- **Public API**: Clean C99 interface for library users
- **Core Implementation**: Sophisticated algorithm implementation
- **Supporting Tools**: Testing, benchmarking, and examples
- **Documentation**: Comprehensive guides and references

### Design Principles

1. **Clean API**: Public interface hides implementation complexity
2. **Modularity**: Separate components with well-defined interfaces
3. **Testability**: Comprehensive test suite covering all components
4. **Documentation**: Every component thoroughly documented
5. **Portability**: Cross-platform support (Linux, Windows, macOS)

--------------------------------------------------------------------------------

## Directory Structure

```
subquadratic-shortest-paths/
│
├── include/                   # Public API headers
│   ├── sssp.h                # Main public API (ONLY header users need)
│   ├── README.md             # API usage guide
│   └── USAGE.md              # Quick reference guide
│
├── src/                      # Implementation source files
│   ├── complete_implementation.c  # Core algorithm implementation
│   ├── sssp_api.c            # Public API implementation
│   ├── sssp_breakthrough.h   # Internal algorithm structures
│   ├── utils.c               # Utility functions (I/O, validation)
│   ├── utils.h               # Utility function headers
│   ├── sssp_demo.c           # Demo application
│   ├── test_api.c            # Comprehensive API test suite (69 tests)
│   ├── test_debug.c          # Debug and correctness tests
│   ├── test_medium_graph.c   # Medium-scale graph tests (n=1000)
│   ├── test_large_graph.c    # Large-scale graph tests (n=2000+)
│   └── UTILS_README.md       # Utility function documentation
│
├── examples/                 # Example programs demonstrating library usage
│   ├── simple_example.c      # Minimal usage example
│   ├── basic_usage.c         # Basic API demonstration
│   ├── performance_test.c    # Performance comparison vs Dijkstra
│   ├── graph_generators.c    # Random graph generation utilities
│   ├── utils_demo.c          # Utility function demonstrations
│   ├── README.md             # Examples overview
│   ├── GENERATORS_README.md  # Graph generator documentation
│   └── PERFORMANCE_README.md # Performance testing guide
│
├── tests/                    # Test suite
│   ├── unit_tests.c          # Unit tests for individual components
│   ├── performance_tests.c   # Performance benchmarking suite
│   ├── README.md             # Testing documentation
│   ├── PERFORMANCE_README.md # Performance testing guide
│   └── test_graphs/          # Test graph data
│       ├── graph_parser.c    # Graph file parser
│       ├── test_runner_example.c  # Test runner example
│       ├── README.md         # Test graph documentation
│       └── GRAPH_INDEX.md    # Index of available test graphs
│
├── benchmarks/               # Benchmarking suite
│   ├── scalability_test.c    # Scalability benchmarks
│   ├── compare_dijkstra.c    # Comparison with Dijkstra
│   ├── README.md             # Benchmarking guide
│   └── results/              # Benchmark results
│       └── README.md         # Results documentation
│
├── docs/                     # Additional documentation
│   ├── ALGORITHM_OVERVIEW.md # High-level algorithm explanation
│   ├── IMPLEMENTATION_NOTES.md # Implementation details
│   └── PERFORMANCE_GUIDE.md  # Performance tuning guide
│
├── cmake/                    # CMake build system configuration
│   ├── CompilerWarnings.cmake # Compiler warning configuration
│   ├── Sanitizers.cmake      # Sanitizer support
│   ├── Packaging.cmake       # CPack configuration
│   ├── libssspConfig.cmake.in # CMake package config
│   └── libsssp.pc.in         # pkg-config file template
│
├── docker/                   # Docker configuration
│   ├── Dockerfile            # Container definition
│   └── scripts/              # Container scripts
│
├── paper/                    # Research paper supplementary materials
│   └── supplementary_proofs.md # Additional mathematical proofs
│
├── assets/                   # Media assets
│   └── README.md             # Asset documentation
│
├── .github/                  # GitHub configuration
│   └── workflows/            # CI/CD workflows
│       └── ci.yml            # Continuous integration
│
├── build/                    # Build output directory (generated)
│   ├── bin/                  # Compiled executables
│   │   ├── examples/         # Example executables
│   │   └── tests/            # Test executables
│   └── lib/                  # Compiled libraries (libsssp.so/.a)
│
├── CMakeLists.txt            # Main CMake configuration
├── VERSION                   # Version number file
│
├── README.md                 # Main project README (START HERE)
├── API_REFERENCE.md          # Complete API documentation
├── EXAMPLES.md               # Usage examples and patterns
├── TECHNICAL_ANALYSIS.md     # Algorithm deep dive
├── INSTALL.md                # Installation guide
├── CONTRIBUTING.md           # Contribution guidelines
├── CHANGELOG.md              # Version history
├── ARCHITECTURE.md           # This file
├── LICENSE                   # MIT license
└── CITATION.md               # Citation information
```

--------------------------------------------------------------------------------

## Component Architecture

### 1\. Public API Layer (`include/sssp.h`)

**Purpose**: Clean, stable interface for library users

**Key Components**:

- Graph management functions
- SSSP solver
- Result querying functions
- Graph validation
- File I/O
- Utility functions

**Design Pattern**: Opaque types with accessor functions

```c
typedef struct sssp_graph sssp_graph_t;    // Opaque
typedef struct sssp_result sssp_result_t;  // Opaque
```

**Location**: `include/sssp.h:1-511`

--------------------------------------------------------------------------------

### 2\. Core Algorithm Implementation (`src/complete_implementation.c`)

**Purpose**: Implements the breakthrough O(m log^(2/3) n) algorithm

**Key Components**:

1. **FindPivots Algorithm**: Frontier reduction
2. **BaseCase Handler**: Small graph handling with Dijkstra
3. **BMSSP Recursive Algorithm**: Main divide-and-conquer logic
4. **Block Data Structure**: O(1) amortized operations

**Internal Structures**:

```c
typedef struct {
    Block* head;           // Block-based linked list
    Block* tail;
    int total_size;
    int num_blocks;
} BlockList;

typedef struct {
    int vertex;
    double distance;
    int batch_id;
} VertexDistance;
```

**Algorithm Parameters**:

- `k = ⌊log^(1/3)(n)⌋`: Frontier reduction factor
- `t = ⌊log^(2/3)(n)⌋`: Batch size exponent
- `L = ⌈log(n)/t⌉`: Recursion levels

**Location**: `src/complete_implementation.c`

--------------------------------------------------------------------------------

### 3\. API Implementation (`src/sssp_api.c`)

**Purpose**: Bridges public API to internal implementation

**Responsibilities**:

- Input validation
- Memory management
- Error handling
- Format conversions
- Result packaging

**Key Functions**:

- `sssp_graph_create()`: Allocates and initializes graph
- `sssp_graph_add_edge()`: Edge insertion with validation
- `sssp_solve()`: Main entry point, calls internal algorithm
- `sssp_result_*()`: Result accessor functions

**Location**: `src/sssp_api.c`

--------------------------------------------------------------------------------

### 4\. Utility Functions (`src/utils.c`)

**Purpose**: Supporting functionality for I/O, validation, and helpers

**Categories**:

1. **Graph I/O**:

  - Edge list format reader/writer
  - DIMACS format reader
  - DOT format exporter

2. **Validation**:

  - Negative cycle detection (Bellman-Ford)
  - Graph structure validation
  - Solution verification

3. **Helpers**:

  - Min-heap implementation (for Dijkstra fallback)
  - Path reconstruction
  - Distance/predecessor management

**Location**: `src/utils.c`, `src/utils.h`

--------------------------------------------------------------------------------

## Data Flow

### Typical Usage Flow

```
User Application
      │
      ├─→ sssp_graph_create(n)
      │        │
      │        └─→ Allocates graph structure
      │
      ├─→ sssp_graph_add_edge(graph, u, v, w) (multiple times)
      │        │
      │        └─→ Builds adjacency list
      │
      ├─→ sssp_graph_validate(graph) [optional]
      │        │
      │        └─→ Checks for negative cycles
      │
      ├─→ sssp_solve(graph, source)
      │        │
      │        ├─→ Validates input
      │        ├─→ Chooses algorithm (Dijkstra vs Breakthrough)
      │        ├─→ Runs algorithm
      │        │        │
      │        │        ├─→ FindPivots (frontier reduction)
      │        │        ├─→ BMSSP (recursive calls)
      │        │        └─→ Relaxation operations
      │        │
      │        └─→ Packages result
      │
      ├─→ sssp_result_get_distance(result, v) (query results)
      ├─→ sssp_result_get_path(result, v)
      ├─→ sssp_result_verify(result) [optional]
      │
      ├─→ sssp_result_destroy(result)
      └─→ sssp_graph_destroy(graph)
```

### Internal Algorithm Flow

```
sssp_solve()
    │
    ├─→ n < SMALL_GRAPH_THRESHOLD?
    │   ├─→ YES: Run Dijkstra (O(m + n log n))
    │   └─→ NO:  Run Breakthrough Algorithm
    │
    └─→ BMSSP(graph, sources, bound, level)
            │
            ├─→ level == 0?
            │   ├─→ YES: BaseCase (mini-Dijkstra)
            │   └─→ NO:  Continue recursion
            │
            ├─→ FindPivots(U, k)
            │        │
            │        └─→ Reduces |U| to O(|U|/k) pivots
            │
            ├─→ Partition sources into batches (size 2^(lt))
            │
            ├─→ FOR each batch:
            │   ├─→ Recursive BMSSP call
            │   ├─→ BatchPrepend results to data structure
            │   └─→ Update distances
            │
            └─→ Return updated distances
```

--------------------------------------------------------------------------------

## Build System

### CMake Configuration Hierarchy

```
CMakeLists.txt (root)
    │
    ├─→ Project configuration
    ├─→ Build options (SSSP_BUILD_*)
    ├─→ Compiler flags
    │
    ├─→ include(cmake/CompilerWarnings.cmake)
    ├─→ include(cmake/Sanitizers.cmake)
    ├─→ include(cmake/Packaging.cmake)
    │
    ├─→ Library targets
    │   ├─→ sssp_shared (libsssp.so)
    │   └─→ sssp_static (libsssp.a)
    │
    ├─→ Executable targets
    │   ├─→ sssp_demo
    │   ├─→ Examples (examples/*.c)
    │   ├─→ Tests (src/test_*.c, tests/*.c)
    │   └─→ Benchmarks (benchmarks/*.c)
    │
    └─→ Installation targets
        ├─→ Headers → /usr/local/include/
        ├─→ Libraries → /usr/local/lib/
        ├─→ CMake config → /usr/local/lib/cmake/libsssp/
        └─→ pkg-config → /usr/local/lib/pkgconfig/
```

### Build Options

Option                | Default | Description
--------------------- | ------- | --------------------------
`SSSP_BUILD_SHARED`   | ON      | Build shared library
`SSSP_BUILD_STATIC`   | ON      | Build static library
`SSSP_BUILD_TESTS`    | ON      | Build test suite
`SSSP_BUILD_EXAMPLES` | ON      | Build examples
`SSSP_BUILD_DOCS`     | OFF     | Build Doxygen docs
`SSSP_ENABLE_LTO`     | OFF     | Link-time optimization
`SSSP_ENABLE_ASAN`    | OFF     | AddressSanitizer
`SSSP_ENABLE_UBSAN`   | OFF     | UndefinedBehaviorSanitizer

### Build Targets

```bash
make                  # Build library and demos
make test_api         # Build API test suite
make sssp_demo        # Build demo application
make examples         # Build all examples
make tests            # Build all tests
make docs             # Generate documentation (if enabled)
make install          # Install library system-wide
make package          # Create distribution package
```

--------------------------------------------------------------------------------

## Testing Framework

### Test Categories

1. **Unit Tests** (`tests/unit_tests.c`)

  - Individual function testing
  - Edge case coverage
  - Error handling validation

2. **API Tests** (`src/test_api.c`)

  - 69 comprehensive API tests
  - All public functions tested
  - Input validation coverage

3. **Integration Tests** (`src/test_debug.c`, `src/test_medium_graph.c`)

  - End-to-end functionality
  - Medium-scale graphs (n=1000)
  - Algorithm correctness verification

4. **Stress Tests** (`src/test_large_graph.c`)

  - Large-scale graphs (n=2000+)
  - Memory limits
  - Performance validation

5. **Performance Tests** (`tests/performance_tests.c`)

  - Algorithm comparison
  - Scalability analysis
  - Timing measurements

### Running Tests

```bash
# Build and run all tests
cd build
make
ctest --output-on-failure

# Run specific test
./bin/test_api
./bin/test_medium_graph

# Run with CTest labels
ctest -L unit           # Unit tests only
ctest -L integration    # Integration tests
ctest -L stress         # Stress tests
```

### Test Success Criteria

- All 69 API tests pass
- Solution verification succeeds on all graphs
- No memory leaks (verified with Valgrind/ASan)
- Performance within expected bounds

--------------------------------------------------------------------------------

## Documentation Organization

### User Documentation (Start Here)

1. **README.md** - Project overview, quick start
2. **INSTALL.md** - Installation instructions
3. **EXAMPLES.md** - Usage patterns and examples
4. **API_REFERENCE.md** - Complete API documentation

### Developer Documentation

1. **ARCHITECTURE.md** (this file) - Repository structure
2. **TECHNICAL_ANALYSIS.md** - Algorithm deep dive
3. **CONTRIBUTING.md** - Development guidelines
4. **docs/IMPLEMENTATION_NOTES.md** - Implementation details

### Reference Documentation

1. **CHANGELOG.md** - Version history
2. **CITATION.md** - Citation information
3. **API_REFERENCE.md** - Function reference

### In-Code Documentation

- All public API functions have Doxygen comments
- Algorithm implementation includes detailed inline comments
- Complex sections reference paper sections

--------------------------------------------------------------------------------

## Navigation Guide

### I Want To...

#### Use the Library in My Project

1. Start with **README.md** for overview
2. Read **INSTALL.md** for installation
3. Follow **EXAMPLES.md** for usage patterns
4. Reference **API_REFERENCE.md** for specific functions

#### Understand the Algorithm

1. Read **README.md** for high-level overview
2. Study **TECHNICAL_ANALYSIS.md** for algorithm details
3. Check **docs/ALGORITHM_OVERVIEW.md** for summary
4. Read the original paper (linked in README)

#### Contribute to the Project

1. Read **CONTRIBUTING.md** for guidelines
2. Review **ARCHITECTURE.md** (this file) for structure
3. Study **docs/IMPLEMENTATION_NOTES.md** for details
4. Look at existing code in `src/` directory

#### Run Performance Benchmarks

1. Check **examples/PERFORMANCE_README.md**
2. Build with `cmake -DSSSP_BUILD_EXAMPLES=ON`
3. Run `./bin/examples/performance_test`
4. See **benchmarks/README.md** for advanced testing

#### Add a New Test

1. Read **tests/README.md** for test structure
2. Look at `src/test_api.c` for examples
3. Add test to `tests/` directory
4. Update `CMakeLists.txt` to include new test
5. Run with `ctest`

#### Debug an Issue

1. Build with `cmake -DCMAKE_BUILD_TYPE=Debug -DSSSP_ENABLE_ASAN=ON`
2. Run `./bin/test_debug` for correctness checks
3. Use `sssp_result_verify()` to validate solutions
4. Check `src/test_debug.c` for debugging techniques

--------------------------------------------------------------------------------

## Component Dependencies

```
User Application
       │
       ↓
  include/sssp.h (Public API)
       │
       ↓
  src/sssp_api.c (API Implementation)
       │
       ├─→ src/complete_implementation.c (Core Algorithm)
       │       │
       │       └─→ src/sssp_breakthrough.h (Internal Structures)
       │
       └─→ src/utils.c (Utilities)
               │
               └─→ src/utils.h (Utility Headers)
```

### External Dependencies

- **Math library** (libm): Standard math functions
- **CMake** (3.15+): Build system
- **C99 compiler**: GCC, Clang, or MSVC
- **Optional**: Doxygen (documentation), Valgrind (memory testing)

--------------------------------------------------------------------------------

## File Naming Conventions

Pattern       | Purpose                 | Example
------------- | ----------------------- | ------------------
`*.c`         | C source files          | `sssp_api.c`
`*.h`         | C header files          | `sssp.h`
`*_test.c`    | Test files              | `test_api.c`
`*.md`        | Markdown documentation  | `README.md`
`*_README.md` | Directory-specific docs | `UTILS_README.md`
`*.in`        | Template files          | `libsssp.pc.in`
`*.cmake`     | CMake modules           | `Sanitizers.cmake`

--------------------------------------------------------------------------------

## Key Files Quick Reference

File                            | Purpose           | Users        | Developers
------------------------------- | ----------------- | ------------ | --------------------------
`README.md`                     | Project overview  | ✅ Read first | ✅ Read first
`include/sssp.h`                | Public API        | ✅ Essential  | ✅ Essential
`API_REFERENCE.md`              | API docs          | ✅ Reference  | ⚠️ Optional
`EXAMPLES.md`                   | Usage examples    | ✅ Essential  | ⚠️ Optional
`TECHNICAL_ANALYSIS.md`         | Algorithm details | ⚠️ Optional  | ✅ Essential
`ARCHITECTURE.md`               | This file         | ❌ Skip       | ✅ Essential
`CONTRIBUTING.md`               | Development guide | ❌ Skip       | ✅ Read before contributing
`src/complete_implementation.c` | Core algorithm    | ❌ Skip       | ✅ Study carefully
`CMakeLists.txt`                | Build config      | ❌ Skip       | ✅ Understand

**Legend**:

- ✅ Essential
- ⚠️ Recommended
- ❌ Not needed

--------------------------------------------------------------------------------

## Version Control Organization

### Branch Structure

- `main`: Production-ready releases
- `develop`: Active development
- `feature/*`: Feature branches
- `bugfix/*`: Bug fix branches
- `release/*`: Release preparation

### Git Workflow

1. Fork repository
2. Create feature branch from `develop`
3. Implement changes
4. Add tests
5. Update documentation
6. Submit pull request to `develop`

See **CONTRIBUTING.md** for detailed workflow.

--------------------------------------------------------------------------------

## Platform-Specific Notes

### Linux

- Primary development platform
- All features fully supported
- Standard Unix directory layout

### Windows

- MSVC and MinGW supported
- DLL export/import handled automatically
- Path separators handled by CMake

### macOS

- Apple Clang supported
- Standard Unix conventions
- Homebrew-friendly

--------------------------------------------------------------------------------

## Future Architecture Plans

### Planned Improvements

1. **Thread-safe solver**: Allow concurrent SSSP on same graph
2. **GPU acceleration**: CUDA/OpenCL implementation
3. **Python bindings**: Cython/cffi wrapper
4. **Streaming API**: Process graphs that don't fit in memory
5. **Incremental updates**: Dynamic graph support

See **CONTRIBUTING.md** for roadmap details.

--------------------------------------------------------------------------------

**Last Updated**: 2025-11-12 **Version**: 1.0.0

For questions about architecture, see **CONTRIBUTING.md** or open an issue on GitHub.
