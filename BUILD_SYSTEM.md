# Build System Documentation

Complete documentation of the production-grade CMake build system for libsssp.

## Overview

The libsssp build system is designed for:
- **Cross-platform compilation** (Linux, macOS, Windows)
- **Multiple compiler support** (GCC, Clang, MSVC)
- **Flexible configuration** (shared/static libraries, sanitizers, LTO)
- **Package distribution** (DEB, RPM, DMG, NSIS, Conan, vcpkg)
- **CI/CD integration** (GitHub Actions, Docker)
- **Easy integration** (CMake, pkg-config, package managers)

## Architecture

### Directory Structure

```
.
├── CMakeLists.txt              # Main build configuration
├── VERSION                     # Version file (1.0.0)
├── cmake/                      # CMake modules
│   ├── CompilerWarnings.cmake  # Compiler warning configuration
│   ├── Sanitizers.cmake        # Sanitizer support
│   ├── Packaging.cmake         # CPack configuration
│   ├── libssspConfig.cmake.in  # CMake package config template
│   ├── libsssp.pc.in          # pkg-config template
│   └── toolchain-*.cmake       # Cross-compilation toolchains
├── conanfile.py                # Conan package recipe
├── vcpkg.json                  # vcpkg manifest
├── portfile.cmake              # vcpkg portfile
├── docker/                     # Docker build environments
│   ├── Dockerfile.*            # Platform-specific Dockerfiles
│   ├── docker-compose.yml      # Multi-platform orchestration
│   └── scripts/                # Build scripts
│       ├── build.sh            # Unix build script
│       └── build.ps1           # Windows build script
├── .github/workflows/          # CI/CD pipelines
│   └── build-and-test.yml     # GitHub Actions workflow
└── docs/                       # Documentation
    ├── BUILD.md                # Build instructions
    ├── PACKAGING.md            # Packaging guide
    └── QUICKSTART.md           # Quick start guide
```

## Build Targets

### Library Targets

| Target | Type | Output | Description |
|--------|------|--------|-------------|
| `sssp_shared` | SHARED | `libsssp.so/.dylib/.dll` | Shared library |
| `sssp_static` | STATIC | `libsssp.a/.lib` | Static library |
| `libsssp::shared` | ALIAS | - | Namespaced alias for shared |
| `libsssp::static` | ALIAS | - | Namespaced alias for static |
| `libsssp::libsssp` | ALIAS | - | Default library (shared or static) |

### Executable Targets

| Target | Source | Description |
|--------|--------|-------------|
| `demo_api` | `demo_api.c` | API demonstration |
| `sssp_demo` | `src/sssp_demo.c` | SSSP demo program |
| `test_api` | `src/test_api.c` | API test suite |
| `test_debug` | `src/test_debug.c` | Debug tests |
| `test_medium_graph` | `src/test_medium_graph.c` | Medium graph tests |
| `test_large_graph` | `src/test_large_graph.c` | Large graph tests |

### Custom Targets

| Target | Command | Description |
|--------|---------|-------------|
| `docs` | `make docs` | Generate Doxygen documentation |
| `package` | `make package` | Create CPack packages |
| `package_source` | `make package_source` | Create source tarball |

## Configuration Options

### Library Options

```cmake
option(SSSP_BUILD_SHARED "Build shared library" ON)
option(SSSP_BUILD_STATIC "Build static library" ON)
option(SSSP_POSITION_INDEPENDENT_CODE "Enable -fPIC" ON)
option(SSSP_ENABLE_LTO "Enable Link-Time Optimization" OFF)
```

### Build Feature Options

```cmake
option(SSSP_BUILD_TESTS "Build test suite" ON)
option(SSSP_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(SSSP_BUILD_EXAMPLES "Build examples" ON)
option(SSSP_BUILD_DEMOS "Build demos" ON)
option(SSSP_BUILD_DOCS "Build documentation" OFF)
option(SSSP_INSTALL "Generate install targets" ON)
```

### Sanitizer Options

```cmake
option(SSSP_ENABLE_ASAN "AddressSanitizer" OFF)
option(SSSP_ENABLE_UBSAN "UndefinedBehaviorSanitizer" OFF)
option(SSSP_ENABLE_TSAN "ThreadSanitizer" OFF)
option(SSSP_ENABLE_MSAN "MemorySanitizer" OFF)  # Clang only
```

## Compiler Configuration

### Warning Flags

The build system enables comprehensive warnings via `cmake/CompilerWarnings.cmake`:

#### GCC/Clang Common
- `-Wall -Wextra -Wpedantic`
- `-Wshadow -Wcast-align -Wunused`
- `-Wformat=2 -Wformat-security`
- `-Wnull-dereference`
- `-Wdouble-promotion`
- `-Wimplicit-fallthrough`

#### GCC-Specific
- `-Wlogical-op`
- `-Wduplicated-cond`
- `-Wduplicated-branches`

#### Clang-Specific
- `-Wconversion`
- `-Wsign-conversion`

#### MSVC
- `/W4` (warning level 4)
- `/permissive-` (standards conformance)
- Multiple specific warnings enabled

### Optimization Flags

#### Debug Build
```cmake
-O0 -g3          # No optimization, full debug info
```

#### Release Build
```cmake
-O3 -DNDEBUG -march=native  # GCC/Clang
/O2 /GL                      # MSVC
```

### Sanitizer Flags

When enabled, sanitizers add:
```cmake
-fsanitize=address           # ASAN
-fsanitize=undefined         # UBSAN
-fsanitize=thread            # TSAN
-fsanitize=memory            # MSAN (Clang only)
-fno-omit-frame-pointer      # Better stack traces
-O1 -g                       # Debug info with minimal optimization
```

## Installation Layout

Default installation follows GNU standards:

```
${CMAKE_INSTALL_PREFIX}/
├── include/
│   └── sssp.h                      # Public API header
├── lib/
│   ├── libsssp.so.1.0.0           # Shared library (versioned)
│   ├── libsssp.so.1 -> libsssp.so.1.0.0
│   ├── libsssp.so -> libsssp.so.1
│   ├── libsssp.a                   # Static library
│   ├── pkgconfig/
│   │   └── libsssp.pc             # pkg-config file
│   └── cmake/libsssp/
│       ├── libssspConfig.cmake    # CMake package config
│       ├── libssspConfigVersion.cmake
│       └── libssspTargets.cmake   # Imported targets
├── bin/
│   └── libsssp.dll                 # Windows DLL
└── share/
    └── doc/libsssp/
        ├── README.md
        └── CITATION.md
```

## Package Generation

### CPack Generators

Platform-specific generators are automatically selected:

| Platform | Generators | Output |
|----------|-----------|--------|
| Linux | DEB, RPM, TGZ, TBZ2 | `.deb`, `.rpm`, `.tar.gz` |
| macOS | DragNDrop, productbuild, TGZ | `.dmg`, `.pkg`, `.tar.gz` |
| Windows | NSIS, WIX, ZIP, NuGet | `.exe`, `.msi`, `.zip`, `.nupkg` |

### Package Components

```cmake
CPACK_COMPONENTS_ALL = libraries headers development documentation
```

- **libraries**: Runtime shared libraries
- **headers**: Public header files
- **development**: Static libraries, CMake/pkg-config files
- **documentation**: Documentation and examples

## Cross-Compilation

### Toolchain Files

Example ARM64 Linux toolchain (`cmake/toolchain-arm64-linux.cmake`):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
```

Usage:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64-linux.cmake
```

### Common Targets

- **ARM64 Linux**: `aarch64-linux-gnu-gcc`
- **ARMv7 Linux**: `arm-linux-gnueabihf-gcc`
- **Windows from Linux**: `x86_64-w64-mingw32-gcc`
- **macOS Universal**: `-DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"`

## Docker Integration

### Multi-Platform Testing

The `docker-compose.yml` defines services for:
- Ubuntu 20.04, 22.04 (GCC, Clang)
- Alpine Linux (musl libc)
- Fedora (latest)
- Sanitizer builds (ASAN, UBSAN, TSAN)

### Build Environment Variables

```bash
BUILD_TYPE=Release          # Debug, Release, RelWithDebInfo
COMPILER=gcc                # gcc, clang
ENABLE_SANITIZERS=OFF       # OFF, ASAN, UBSAN, TSAN, MSAN
RUN_TESTS=ON                # ON, OFF
RUN_VALGRIND=OFF            # ON, OFF
```

## CI/CD Integration

### GitHub Actions Workflow

`.github/workflows/build-and-test.yml` provides:

1. **Matrix builds**:
   - Ubuntu 20.04, 22.04
   - macOS (x86_64, ARM64, universal)
   - Windows (MSVC, MinGW)
   - Sanitizer builds

2. **Automated testing**:
   - Unit tests
   - Integration tests
   - Memory leak detection (Valgrind)

3. **Artifact upload**:
   - Binaries
   - Packages
   - Documentation

4. **Package creation**:
   - Platform-specific packages
   - Conan packages
   - Source tarballs

### Build Status Badges

Add to README.md:
```markdown
![Build](https://github.com/your-org/subquadratic-shortest-paths/workflows/Build%20and%20Test/badge.svg)
```

## Package Manager Integration

### CMake (find_package)

```cmake
find_package(libsssp 1.0 REQUIRED)
target_link_libraries(myapp PRIVATE libsssp::libsssp)
```

### pkg-config

```bash
pkg-config --cflags libsssp  # Get include flags
pkg-config --libs libsssp    # Get linker flags
pkg-config --modversion libsssp  # Get version
```

### Conan

```python
[requires]
libsssp/1.0.0

[generators]
CMakeDeps
CMakeToolchain
```

### vcpkg

```bash
vcpkg install libsssp
```

CMake integration:
```cmake
find_package(libsssp CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE libsssp::libsssp)
```

## Version Management

### Version File

`VERSION` contains: `1.0.0`

### CMake Version Variables

```cmake
${PROJECT_VERSION}         # 1.0.0
${PROJECT_VERSION_MAJOR}   # 1
${PROJECT_VERSION_MINOR}   # 0
${PROJECT_VERSION_PATCH}   # 0
```

### Library Versioning (SOVERSION)

Shared library versioning:
```cmake
VERSION 1.0.0
SOVERSION 1
```

Generates:
- `libsssp.so.1.0.0` (full version)
- `libsssp.so.1` (major version symlink)
- `libsssp.so` (development symlink)

## Advanced Features

### Link-Time Optimization

```bash
cmake .. -DSSSP_ENABLE_LTO=ON
```

Enables whole-program optimization for maximum performance.

### Position-Independent Code

```bash
cmake .. -DSSSP_POSITION_INDEPENDENT_CODE=ON
```

Required for shared libraries on most platforms.

### Custom Install Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/libsssp
```

### Building Only Shared or Static

```bash
# Shared only
cmake .. -DSSSP_BUILD_SHARED=ON -DSSSP_BUILD_STATIC=OFF

# Static only
cmake .. -DSSSP_BUILD_SHARED=OFF -DSSSP_BUILD_STATIC=ON
```

### Debug Build with Sanitizers

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSSSP_ENABLE_ASAN=ON \
  -DSSSP_ENABLE_UBSAN=ON
```

## Troubleshooting

### Common Issues

#### CMake version too old
**Error**: `CMake 3.15 or higher is required`
**Solution**: Upgrade CMake or use `snap install cmake --classic`

#### Compiler not found
**Error**: `Could not find compiler`
**Solution**: Set CC/CXX environment variables:
```bash
export CC=gcc-11 CXX=g++-11
```

#### Math library not found
**Error**: `undefined reference to 'pow'`
**Solution**: System has math library built into libc (normal on modern systems)

#### Sanitizer conflicts
**Error**: `ThreadSanitizer is incompatible with AddressSanitizer`
**Solution**: Only enable one sanitizer at a time (except ASAN+UBSAN)

### Build System Debugging

Enable verbose output:
```bash
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build . -- VERBOSE=1
```

Print all CMake variables:
```bash
cmake .. -LAH
```

## Best Practices

### For Library Developers

1. **Always run tests**: `ctest` after building
2. **Use sanitizers**: Catch bugs early with ASAN/UBSAN
3. **Check cross-platform**: Use Docker for multi-platform testing
4. **Verify installation**: Test installed library in separate project
5. **Update version**: Bump VERSION file for releases

### For Library Users

1. **Use find_package**: Preferred CMake integration method
2. **Link privately**: `target_link_libraries(app PRIVATE libsssp::libsssp)`
3. **Check version**: Specify minimum version in `find_package`
4. **Use pkg-config**: For non-CMake build systems
5. **Read documentation**: Check BUILD.md for platform-specific notes

## References

- [CMake Documentation](https://cmake.org/documentation/)
- [CPack Documentation](https://cmake.org/cmake/help/latest/module/CPack.html)
- [Conan Documentation](https://docs.conan.io/)
- [vcpkg Documentation](https://vcpkg.io/)
- [GNU Coding Standards](https://www.gnu.org/prep/standards/)

---

**Last Updated**: January 2025
**CMake Minimum Version**: 3.15
**Supported Platforms**: Linux, macOS, Windows
