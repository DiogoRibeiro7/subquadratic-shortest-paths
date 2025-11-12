# Installation Guide

This guide provides detailed instructions for installing the SSSP Breakthrough Algorithm library on various platforms.

## Table of Contents

- [System Requirements](#system-requirements)
- [Quick Install](#quick-install)
- [Platform-Specific Instructions](#platform-specific-instructions)
  - [Linux](#linux)
  - [Windows](#windows)
  - [macOS](#macos)
- [Build from Source](#build-from-source)
- [Build Options](#build-options)
- [Docker Installation](#docker-installation)
- [Package Managers](#package-managers)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

---

## System Requirements

### Minimum Requirements

- **C Compiler**: C99-compliant (GCC 7+, Clang 9+, MSVC 2019+)
- **CMake**: Version 3.15 or later
- **Memory**: 4 GB RAM minimum
- **Disk Space**: 100 MB for source + build

### Recommended Requirements

- **C Compiler**: GCC 11+, Clang 12+, or MSVC 2022+
- **CMake**: Version 3.20 or later
- **Memory**: 8 GB RAM or more
- **Disk Space**: 500 MB for full build with tests and examples

### Dependencies

**Required:**
- Math library (libm) - Usually included with compiler on Unix systems
- C standard library

**Optional:**
- Doxygen 1.9+ (for documentation generation)
- GraphViz (for DOT file visualization)
- Python 3.7+ (for benchmark scripts)

---

## Quick Install

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake git

# Clone and build
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

### macOS

```bash
# Install dependencies (using Homebrew)
brew install cmake

# Clone and build
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
sudo make install
```

### Windows (MSYS2/MinGW)

```bash
# In MSYS2 terminal
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake git make

# Clone and build
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make install
```

---

## Platform-Specific Instructions

### Linux

#### Ubuntu / Debian / Mint

```bash
# 1. Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libm-dev

# 2. Optional: Install documentation tools
sudo apt-get install -y doxygen graphviz

# 3. Clone repository
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths

# 4. Create build directory
mkdir build && cd build

# 5. Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release \
      -DSSSP_BUILD_TESTS=ON \
      -DSSSP_BUILD_EXAMPLES=ON \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ..

# 6. Build
make -j$(nproc)

# 7. Run tests (optional)
ctest --output-on-failure

# 8. Install
sudo make install

# 9. Update library cache
sudo ldconfig
```

#### Fedora / RHEL / CentOS

```bash
# Install dependencies
sudo dnf install gcc gcc-c++ cmake git make

# Or for older versions:
sudo yum install gcc gcc-c++ cmake3 git make

# Follow steps 3-9 from Ubuntu instructions
```

#### Arch Linux

```bash
# Install dependencies
sudo pacman -S base-devel cmake git

# Follow steps 3-9 from Ubuntu instructions
```

### Windows

#### Option 1: Visual Studio (Recommended)

**Prerequisites:**
- Visual Studio 2019 or later with C++ desktop development workload
- CMake 3.15+ (included with Visual Studio or install separately)
- Git for Windows

**Steps:**

1. **Open Visual Studio Developer Command Prompt**
   - Start → Visual Studio 2022 → Developer Command Prompt

2. **Clone repository:**
   ```cmd
   git clone https://github.com/your-org/subquadratic-shortest-paths.git
   cd subquadratic-shortest-paths
   ```

3. **Create build directory:**
   ```cmd
   mkdir build
   cd build
   ```

4. **Configure with CMake:**
   ```cmd
   cmake -G "Visual Studio 17 2022" -A x64 ^
         -DCMAKE_BUILD_TYPE=Release ^
         -DSSSP_BUILD_TESTS=ON ^
         -DSSSP_BUILD_EXAMPLES=ON ^
         ..
   ```

5. **Build:**
   ```cmd
   cmake --build . --config Release --parallel
   ```

6. **Install (run as Administrator):**
   ```cmd
   cmake --install . --config Release
   ```

7. **Run tests:**
   ```cmd
   ctest -C Release --output-on-failure
   ```

#### Option 2: MSYS2/MinGW-w64

**Prerequisites:**
- [MSYS2](https://www.msys2.org/) installed

**Steps:**

1. **Open MSYS2 MINGW64 terminal**

2. **Update MSYS2:**
   ```bash
   pacman -Syu
   ```

3. **Install dependencies:**
   ```bash
   pacman -S --needed \
       mingw-w64-x86_64-gcc \
       mingw-w64-x86_64-cmake \
       mingw-w64-x86_64-make \
       git
   ```

4. **Follow Linux build steps** (use `mingw32-make` instead of `make`)

#### Option 3: Windows Subsystem for Linux (WSL)

1. **Install WSL2** (if not already installed):
   ```powershell
   wsl --install -d Ubuntu
   ```

2. **Follow Ubuntu Linux instructions** inside WSL

### macOS

#### Using Xcode Command Line Tools

**Prerequisites:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**Installation:**

1. **Install CMake:**
   ```bash
   brew install cmake
   ```

2. **Clone repository:**
   ```bash
   git clone https://github.com/your-org/subquadratic-shortest-paths.git
   cd subquadratic-shortest-paths
   ```

3. **Build:**
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/usr/local \
         ..
   make -j$(sysctl -n hw.ncpu)
   ```

4. **Test:**
   ```bash
   ctest --output-on-failure
   ```

5. **Install:**
   ```bash
   sudo make install
   ```

---

## Build from Source

### Standard Build

```bash
# Clone repository
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths

# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --parallel

# Test
ctest --output-on-failure

# Install
sudo cmake --install .
```

### Development Build

For development with debugging and sanitizers:

```bash
mkdir build-debug && cd build-debug

cmake -DCMAKE_BUILD_TYPE=Debug \
      -DSSSP_BUILD_TESTS=ON \
      -DSSSP_BUILD_EXAMPLES=ON \
      -DSSSP_ENABLE_ASAN=ON \
      -DSSSP_ENABLE_UBSAN=ON \
      ..

cmake --build . --parallel

# Run tests with sanitizers
ctest --output-on-failure
```

### Optimized Release Build

For maximum performance:

```bash
mkdir build-release && cd build-release

cmake -DCMAKE_BUILD_TYPE=Release \
      -DSSSP_ENABLE_LTO=ON \
      -DCMAKE_C_FLAGS="-O3 -march=native" \
      ..

cmake --build . --parallel
```

---

## Build Options

All CMake options can be set with `-D<OPTION>=<VALUE>`:

### Core Options

| Option | Default | Description |
|--------|---------|-------------|
| `SSSP_BUILD_SHARED` | ON | Build shared library (.so, .dylib, .dll) |
| `SSSP_BUILD_STATIC` | ON | Build static library (.a, .lib) |
| `SSSP_BUILD_TESTS` | ON | Build test suite |
| `SSSP_BUILD_EXAMPLES` | ON | Build example programs |
| `SSSP_BUILD_DOCS` | OFF | Build documentation (requires Doxygen) |
| `SSSP_INSTALL` | ON | Generate install targets |

### Performance Options

| Option | Default | Description |
|--------|---------|-------------|
| `SSSP_ENABLE_LTO` | OFF | Enable Link-Time Optimization |
| `CMAKE_BUILD_TYPE` | Release | Build type: Debug, Release, RelWithDebInfo |
| `SSSP_POSITION_INDEPENDENT_CODE` | ON | Enable -fPIC for shared libraries |

### Debugging Options

| Option | Default | Description |
|--------|---------|-------------|
| `SSSP_ENABLE_ASAN` | OFF | Enable AddressSanitizer |
| `SSSP_ENABLE_UBSAN` | OFF | Enable UndefinedBehaviorSanitizer |
| `SSSP_ENABLE_TSAN` | OFF | Enable ThreadSanitizer |
| `SSSP_ENABLE_MSAN` | OFF | Enable MemorySanitizer |
| `SSSP_ENABLE_COVERAGE` | OFF | Enable code coverage instrumentation |

### Installation Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_INSTALL_PREFIX` | /usr/local | Installation prefix |

### Example Configurations

```bash
# Minimal build (library only)
cmake -DSSSP_BUILD_TESTS=OFF -DSSSP_BUILD_EXAMPLES=OFF ..

# Static library only
cmake -DSSSP_BUILD_SHARED=OFF -DSSSP_BUILD_STATIC=ON ..

# With documentation
cmake -DSSSP_BUILD_DOCS=ON ..

# Debug with all sanitizers
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DSSSP_ENABLE_ASAN=ON \
      -DSSSP_ENABLE_UBSAN=ON \
      -DSSSP_ENABLE_TSAN=OFF \
      ..
```

---

## Docker Installation

### Pull Pre-built Image

```bash
docker pull ghcr.io/your-org/sssp:latest
docker run -it ghcr.io/your-org/sssp:latest
```

### Build Docker Image

```bash
# From repository root
docker build -t sssp:local .

# Run container
docker run -it sssp:local bash

# Inside container, run examples
/app/build/bin/examples/simple_example
```

### Docker Compose

```yaml
version: '3.8'
services:
  sssp:
    image: ghcr.io/your-org/sssp:latest
    volumes:
      - ./data:/data
    working_dir: /data
```

---

## Package Managers

### Using vcpkg

```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

# Install sssp
./vcpkg/vcpkg install sssp

# Use in CMake project
find_package(sssp CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE libsssp::sssp)
```

### Using Conan

```python
# conanfile.txt
[requires]
sssp/1.0.0

[generators]
cmake
```

```bash
# Install
conan install . --build=missing

# Use in CMake
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
target_link_libraries(myapp ${CONAN_LIBS})
```

---

## Verification

### Test Installation

Create `test.c`:
```c
#include <sssp.h>
#include <stdio.h>

int main(void) {
    printf("Library Version: %s\n", sssp_get_version());
    printf("Algorithm: %s\n", sssp_get_info());

    sssp_graph_t* g = sssp_graph_create(10);
    if (g) {
        printf("✓ Library working correctly!\n");
        sssp_graph_destroy(g);
        return 0;
    }
    return 1;
}
```

Compile and run:
```bash
# Linux/macOS
gcc -o test test.c -lsssp -lm
./test

# Windows (Visual Studio)
cl test.c sssp.lib
test.exe
```

Expected output:
```
Library Version: 1.0.0
Algorithm: O(m log^(2/3) n) Breakthrough SSSP
✓ Library working correctly!
```

### Run Test Suite

```bash
cd build
ctest --output-on-failure --verbose
```

Expected: 4-6 tests passing with detailed output.

### Run Examples

```bash
# Simple example
./bin/examples/simple_example

# Performance test
./bin/examples/performance_test

# Graph generators
./bin/examples/graph_generators --help
```

---

## Troubleshooting

### Common Issues

#### 1. CMake Not Found

**Problem:** `cmake: command not found`

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
```

#### 2. Compiler Not Found

**Problem:** `No CMAKE_C_COMPILER could be found`

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
# Install Visual Studio with C++ workload
```

#### 3. Math Library Not Found

**Problem:** `undefined reference to 'log'` or `cannot find -lm`

**Solution:**
- On Linux: Math library should be auto-detected
- On Windows/MSYS2: Install `mingw-w64-x86_64-gcc`
- Add explicitly: `target_link_libraries(myapp PRIVATE m)`

#### 4. Permission Denied on Install

**Problem:** `Permission denied` when running `make install`

**Solution:**
```bash
# Use sudo
sudo make install

# Or install to user directory
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make install
# Add to PATH: export PATH=$HOME/.local/bin:$PATH
```

#### 5. Tests Failing

**Problem:** Some tests fail during `ctest`

**Solution:**
```bash
# Run tests with verbose output
ctest --output-on-failure --verbose

# Run specific test
ctest -R APITests --verbose

# Check which tests are enabled
ctest -N
```

#### 6. Linker Errors

**Problem:** `undefined reference to sssp_solve`

**Solution:**
```bash
# Ensure library is installed
sudo ldconfig  # Linux

# Check library path
echo $LD_LIBRARY_PATH  # Linux
echo $DYLD_LIBRARY_PATH  # macOS

# Link explicitly
gcc -o myapp myapp.c -L/usr/local/lib -lsssp -lm
```

#### 7. Windows DLL Not Found

**Problem:** `The code execution cannot proceed because sssp.dll was not found`

**Solution:**
```cmd
# Copy DLL to executable directory
copy build\lib\sssp.dll build\bin\

# Or add to PATH
set PATH=%PATH%;C:\path\to\sssp\build\lib
```

### Platform-Specific Issues

#### macOS: Architecture Mismatch

**Problem:** `building for macOS-x86_64 but attempting to link with file built for macOS-arm64`

**Solution:**
```bash
# Build for current architecture
cmake -DCMAKE_OSX_ARCHITECTURES=$(uname -m) ..

# Or build universal binary
cmake -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" ..
```

#### Windows: MSVC Runtime Mismatch

**Problem:** `error LNK2038: mismatch detected for 'RuntimeLibrary'`

**Solution:**
```cmd
# Use consistent runtime library
cmake -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreadedDLL" ..
```

### Getting Help

If you encounter issues not covered here:

1. **Check existing issues**: [GitHub Issues](https://github.com/your-org/subquadratic-shortest-paths/issues)
2. **Search documentation**: [Full documentation](https://docs.sssp-breakthrough.org)
3. **Ask for help**: Open a new issue with:
   - Your platform and compiler version
   - CMake version (`cmake --version`)
   - Full error message
   - Steps to reproduce

---

## Next Steps

After successful installation:

1. **Read the [API Reference](API_REFERENCE.md)** to understand available functions
2. **Check [EXAMPLES.md](EXAMPLES.md)** for usage patterns
3. **Run the examples** in `examples/` directory
4. **Integrate into your project** - see integration examples below

### Integration Example

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject C)

find_package(sssp REQUIRED)

add_executable(myapp main.c)
target_link_libraries(myapp PRIVATE libsssp::sssp)
```

**main.c:**
```c
#include <sssp.h>

int main(void) {
    // Your code here
    return 0;
}
```

**Build:**
```bash
mkdir build && cd build
cmake ..
make
```

---

**Last Updated**: November 2025
**Version**: 1.0.0
