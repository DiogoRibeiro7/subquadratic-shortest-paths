# Building libsssp

Comprehensive build instructions for the libsssp library across all supported platforms.

## Table of Contents

- [Quick Start](#quick-start)
- [Prerequisites](#prerequisites)
- [Basic Build](#basic-build)
- [Build Options](#build-options)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Package Manager Integration](#package-manager-integration)
- [Docker Builds](#docker-builds)
- [Advanced Configuration](#advanced-configuration)
- [Troubleshooting](#troubleshooting)

---

## Quick Start

```bash
# Clone the repository
git clone https://github.com/your-org/subquadratic-shortest-paths.git
cd subquadratic-shortest-paths

# Build and install
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
sudo cmake --install .
```

---

## Prerequisites

### Required

- **CMake** ≥ 3.15
- **C Compiler** with C99 support:
  - GCC ≥ 7.0
  - Clang ≥ 6.0
  - MSVC ≥ 2019 (Visual Studio 16.0)
- **Standard C library** (glibc, musl, MSVC CRT)

### Optional

- **Ninja** (recommended build system)
- **Doxygen** (for documentation)
- **Valgrind** (for memory leak detection)
- **Python 3** + **Conan** (for package management)
- **Docker** (for containerized builds)

---

## Basic Build

### Using Make (Unix-like systems)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Using Ninja (all platforms)

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Using Visual Studio (Windows)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## Build Options

Configure the build with CMake options:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DSSSP_BUILD_SHARED=ON \
  -DSSSP_BUILD_STATIC=ON \
  -DSSSP_BUILD_TESTS=ON \
  -DSSSP_BUILD_EXAMPLES=ON
```

### Available Options

| Option | Default | Description |
|--------|---------|-------------|
| `SSSP_BUILD_SHARED` | `ON` | Build shared library (.so/.dylib/.dll) |
| `SSSP_BUILD_STATIC` | `ON` | Build static library (.a/.lib) |
| `SSSP_BUILD_TESTS` | `ON` | Build test suite |
| `SSSP_BUILD_BENCHMARKS` | `OFF` | Build benchmark suite |
| `SSSP_BUILD_EXAMPLES` | `ON` | Build example programs |
| `SSSP_BUILD_DEMOS` | `ON` | Build demo programs |
| `SSSP_BUILD_DOCS` | `OFF` | Build documentation (requires Doxygen) |
| `SSSP_INSTALL` | `ON` | Generate installation targets |
| `SSSP_ENABLE_LTO` | `OFF` | Enable Link-Time Optimization |
| `SSSP_POSITION_INDEPENDENT_CODE` | `ON` | Enable position-independent code (-fPIC) |

### Sanitizer Options

| Option | Description |
|--------|-------------|
| `SSSP_ENABLE_ASAN` | Enable AddressSanitizer (memory errors) |
| `SSSP_ENABLE_UBSAN` | Enable UndefinedBehaviorSanitizer |
| `SSSP_ENABLE_TSAN` | Enable ThreadSanitizer (data races) |
| `SSSP_ENABLE_MSAN` | Enable MemorySanitizer (Clang only) |

**Example with sanitizers:**

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DSSSP_ENABLE_ASAN=ON
```

---

## Platform-Specific Instructions

### Linux

#### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build pkg-config

# Build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Install system-wide
sudo ninja install

# Or install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
ninja install
```

#### Fedora/RHEL/CentOS

```bash
# Install dependencies
sudo dnf install gcc gcc-c++ cmake ninja-build pkgconfig

# Build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

#### Alpine Linux

```bash
# Install dependencies
sudo apk add build-base cmake ninja pkgconfig

# Build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake (via Homebrew)
brew install cmake ninja pkg-config

# Build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

#### Building Universal Binaries (x86_64 + ARM64)

```bash
cmake .. \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Windows

#### Visual Studio 2022

```powershell
# Open Developer Command Prompt for VS 2022
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Install
cmake --install . --prefix C:\Program Files\libsssp
```

#### MinGW-w64

```bash
# Install MSYS2 and MinGW-w64
# From MSYS2 shell:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja

mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
ninja install
```

---

## Package Manager Integration

### Conan

Create `conanfile.txt`:

```ini
[requires]
libsssp/1.0.0

[generators]
CMakeDeps
CMakeToolchain
```

Build your project:

```bash
conan install . --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```

### vcpkg

```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

# Install libsssp
./vcpkg/vcpkg install libsssp

# Use in your CMake project
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### pkg-config

After installation, use with pkg-config:

```bash
# Get compilation flags
pkg-config --cflags libsssp

# Get linker flags
pkg-config --libs libsssp

# Use in Makefile
CFLAGS += $(shell pkg-config --cflags libsssp)
LDFLAGS += $(shell pkg-config --libs libsssp)
```

---

## Docker Builds

### Build All Platforms

```bash
cd docker
docker-compose up --build
```

### Build Specific Platform

```bash
# Ubuntu with GCC
docker-compose up ubuntu-22-gcc

# Alpine Linux
docker-compose up alpine-gcc

# With sanitizers
docker-compose up ubuntu-22-asan
```

### Custom Docker Build

```bash
# Build Ubuntu image
docker build -t libsssp:ubuntu -f docker/Dockerfile.ubuntu .

# Run build
docker run --rm \
  -e BUILD_TYPE=Release \
  -e COMPILER=gcc \
  -e RUN_TESTS=ON \
  libsssp:ubuntu
```

---

## Advanced Configuration

### Custom Installation Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/libsssp
cmake --install .
```

### Shared Library Only

```bash
cmake .. -DSSSP_BUILD_SHARED=ON -DSSSP_BUILD_STATIC=OFF
```

### Debug Build with Sanitizers

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSSSP_ENABLE_ASAN=ON \
  -DSSSP_ENABLE_UBSAN=ON
```

### Cross-Compilation

#### For ARM64 on x86_64

```bash
cmake .. \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_FIND_ROOT_PATH=/usr/aarch64-linux-gnu
```

#### For Windows from Linux (MinGW)

```bash
cmake .. \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
```

### Link-Time Optimization

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DSSSP_ENABLE_LTO=ON
```

### Building Documentation

```bash
sudo apt-get install doxygen graphviz  # Install Doxygen

cmake .. -DSSSP_BUILD_DOCS=ON
cmake --build . --target docs

# Open documentation
xdg-open build/docs/html/index.html
```

---

## Running Tests

### Run All Tests

```bash
cd build
ctest --output-on-failure --parallel $(nproc)
```

### Run Specific Test

```bash
ctest -R APITests -V
```

### Run with Valgrind

```bash
ctest -T memcheck
```

### Run Benchmarks

```bash
cmake .. -DSSSP_BUILD_BENCHMARKS=ON
cmake --build .

# Run all benchmarks
ctest -L benchmark
```

---

## Creating Packages

### Debian Package

```bash
cd build
cpack -G DEB
sudo dpkg -i libsssp-1.0.0-Linux-x64.deb
```

### RPM Package

```bash
cd build
cpack -G RPM
sudo rpm -i libsssp-1.0.0-Linux-x64.rpm
```

### Windows Installer (NSIS)

```powershell
cd build
cpack -G NSIS
# Run the generated installer
```

### Tarball Archive

```bash
cd build
cpack -G TGZ
```

---

## Integration Examples

### CMake Project

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject C)

# Find libsssp
find_package(libsssp 1.0 REQUIRED)

# Create executable
add_executable(my_app main.c)

# Link against libsssp
target_link_libraries(my_app PRIVATE libsssp::libsssp)
```

### Makefile Project

```makefile
CC = gcc
CFLAGS = $(shell pkg-config --cflags libsssp)
LDFLAGS = $(shell pkg-config --libs libsssp)

my_app: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
```

### Direct Compilation

```bash
gcc -o my_app main.c -I/usr/local/include -L/usr/local/lib -lsssp -lm
```

---

## Troubleshooting

### CMake can't find the compiler

**Solution:** Set CC environment variable:
```bash
export CC=gcc
export CXX=g++
cmake ..
```

### Missing math library error

**Solution:** On some systems, explicitly link math library:
```bash
cmake .. -DMATH_LIBRARY=/usr/lib/x86_64-linux-gnu/libm.so
```

### Permission denied during installation

**Solution:** Use sudo or change installation prefix:
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
```

### Tests fail with sanitizers

**Solution:** Sanitizers need proper runtime environment:
```bash
export ASAN_OPTIONS=detect_leaks=1:symbolize=1
export UBSAN_OPTIONS=print_stacktrace=1
```

### Windows DLL not found

**Solution:** Add install directory to PATH or copy DLL:
```powershell
$env:PATH = "C:\Program Files\libsssp\bin;$env:PATH"
```

### Cross-compilation failures

**Solution:** Ensure toolchain file is correct:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
```

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [API Documentation](docs/api.md)
- [Examples](examples/)
- [Contributing Guide](CONTRIBUTING.md)

---

## Build Matrix

Tested platforms and compilers:

| Platform | Compiler | Architectures | Status |
|----------|----------|---------------|--------|
| Ubuntu 20.04 | GCC 9+ | x86_64, ARM64 | ✅ |
| Ubuntu 22.04 | GCC 11+ | x86_64, ARM64 | ✅ |
| Ubuntu 22.04 | Clang 14+ | x86_64 | ✅ |
| Fedora 39 | GCC 13+ | x86_64 | ✅ |
| Alpine 3.19 | GCC 13+ | x86_64 | ✅ |
| macOS 12+ | Clang (Xcode) | x86_64, ARM64 | ✅ |
| Windows 10+ | MSVC 2019+ | x86_64 | ✅ |
| Windows 10+ | MinGW-w64 | x86_64 | ✅ |

---

**For more information, see the [README](README.md) or open an issue on GitHub.**
