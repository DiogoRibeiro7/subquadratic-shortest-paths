# Quick Start Guide

Get up and running with libsssp in 5 minutes.

## For Users

### Install from Package Manager

#### Ubuntu/Debian
```bash
# Add repository (if published)
sudo add-apt-repository ppa:libsssp/stable
sudo apt update
sudo apt install libsssp-dev
```

#### macOS (Homebrew)
```bash
brew install libsssp
```

#### Windows (vcpkg)
```powershell
vcpkg install libsssp
```

### Build from Source

```bash
# 1. Clone repository
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths

# 2. Build and install
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
sudo cmake --install .
```

### Use in Your Project

#### CMake
```cmake
find_package(libsssp 1.0 REQUIRED)
target_link_libraries(your_app PRIVATE libsssp::libsssp)
```

#### pkg-config
```bash
gcc myapp.c $(pkg-config --cflags --libs libsssp) -o myapp
```

### Example Code

```c
#include <sssp.h>
#include <stdio.h>

int main() {
    // Create graph with 5 vertices
    sssp_graph_t* graph = sssp_graph_create(5);

    // Add edges: (from, to, weight)
    sssp_graph_add_edge(graph, 0, 1, 4.0);
    sssp_graph_add_edge(graph, 0, 2, 1.0);
    sssp_graph_add_edge(graph, 2, 1, 2.0);
    sssp_graph_add_edge(graph, 1, 3, 1.0);
    sssp_graph_add_edge(graph, 2, 3, 5.0);
    sssp_graph_add_edge(graph, 3, 4, 3.0);

    // Compute shortest paths from vertex 0
    sssp_result_t* result = sssp_solve(graph, 0);

    if (result) {
        // Print distances
        for (int v = 0; v < 5; v++) {
            double dist = sssp_result_get_distance(result, v);
            if (dist < SSSP_INFINITY) {
                printf("Distance to %d: %.2f\n", v, dist);
            } else {
                printf("Vertex %d is unreachable\n", v);
            }
        }

        sssp_result_destroy(result);
    }

    sssp_graph_destroy(graph);
    return 0;
}
```

Compile and run:
```bash
gcc example.c -lsssp -o example
./example
```

---

## For Developers

### Quick Build

```bash
# Debug build with tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSSSP_BUILD_TESTS=ON
cmake --build build --parallel
cd build && ctest --output-on-failure
```

### With Sanitizers

```bash
# AddressSanitizer
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSSSP_ENABLE_ASAN=ON
cmake --build build && cd build && ctest

# UndefinedBehaviorSanitizer
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSSSP_ENABLE_UBSAN=ON
cmake --build build && cd build && ctest
```

### Docker Build

```bash
# Build all platforms
docker-compose -f docker/docker-compose.yml up

# Single platform
docker-compose -f docker/docker-compose.yml up ubuntu-22-gcc
```

---

## Common Tasks

### Run Examples
```bash
cmake -B build -DSSSP_BUILD_EXAMPLES=ON
cmake --build build
./build/bin/examples/*
```

### Run Benchmarks
```bash
cmake -B build -DSSSP_BUILD_BENCHMARKS=ON
cmake --build build
./build/bin/benchmarks/*
```

### Generate Documentation
```bash
cmake -B build -DSSSP_BUILD_DOCS=ON
cmake --build build --target docs
open build/docs/html/index.html
```

### Create Package
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && cpack -G TGZ
```

---

## Next Steps

- Read the full [Build Guide](BUILD.md)
- Check out [API Documentation](docs/API.md)
- See more [Examples](examples/)
- Read the [Paper](paper/2504.17033v2.pdf)

## Getting Help

- 📖 Documentation: See [docs/](docs/)
- 🐛 Issues: [GitHub Issues](https://github.com/your-org/subquadratic-shortest-paths/issues)
- 💬 Discussions: [GitHub Discussions](https://github.com/your-org/subquadratic-shortest-paths/discussions)

## Performance

The breakthrough algorithm achieves **O(m log^{2/3} n)** time complexity:

- 🚀 **1.93× faster** than Dijkstra on n=1,000
- 🚀 **2.05× faster** on n=10,000
- 🚀 **2.14× faster** on n=100,000

See [IMPLEMENTATION_VERIFICATION.md](IMPLEMENTATION_VERIFICATION.md) for details.
