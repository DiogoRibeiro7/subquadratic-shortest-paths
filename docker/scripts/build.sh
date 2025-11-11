#!/bin/bash
# Build script for libsssp in Docker containers
# Supports multiple build configurations and sanitizers

set -e  # Exit on error
set -u  # Exit on undefined variable

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_DIR="${PROJECT_DIR:-/workspace/libsssp}"
BUILD_DIR="${BUILD_DIR:-${PROJECT_DIR}/build}"
INSTALL_PREFIX="${INSTALL_PREFIX:-${PROJECT_DIR}/install}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
COMPILER="${COMPILER:-gcc}"
ENABLE_SANITIZERS="${ENABLE_SANITIZERS:-OFF}"
RUN_TESTS="${RUN_TESTS:-ON}"
RUN_VALGRIND="${RUN_VALGRIND:-OFF}"

echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}  libsssp Docker Build Script${NC}"
echo -e "${BLUE}==================================================${NC}"
echo -e "Project directory: ${PROJECT_DIR}"
echo -e "Build directory:   ${BUILD_DIR}"
echo -e "Install prefix:    ${INSTALL_PREFIX}"
echo -e "Build type:        ${BUILD_TYPE}"
echo -e "Compiler:          ${COMPILER}"
echo -e "Sanitizers:        ${ENABLE_SANITIZERS}"
echo -e "Run tests:         ${RUN_TESTS}"
echo -e "Run valgrind:      ${RUN_VALGRIND}"
echo -e "${BLUE}==================================================${NC}\n"

# Set compiler
if [ "$COMPILER" = "clang" ]; then
    export CC=clang
    export CXX=clang++
elif [ "$COMPILER" = "gcc" ]; then
    export CC=gcc
    export CXX=g++
fi

echo -e "${GREEN}Using compiler: $(${CC} --version | head -n1)${NC}\n"

# Clean previous build
if [ -d "${BUILD_DIR}" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "${BUILD_DIR}"
fi

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure CMake options
CMAKE_OPTIONS=(
    -G Ninja
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"
    -DSSSP_BUILD_SHARED=ON
    -DSSSP_BUILD_STATIC=ON
    -DSSSP_BUILD_TESTS="${RUN_TESTS}"
    -DSSSP_BUILD_EXAMPLES=ON
    -DSSSP_BUILD_BENCHMARKS=OFF
    -DSSSP_ENABLE_LTO=ON
)

# Add sanitizer options
if [ "$ENABLE_SANITIZERS" = "ASAN" ]; then
    CMAKE_OPTIONS+=(-DSSSP_ENABLE_ASAN=ON)
    echo -e "${YELLOW}Enabling AddressSanitizer${NC}"
elif [ "$ENABLE_SANITIZERS" = "UBSAN" ]; then
    CMAKE_OPTIONS+=(-DSSSP_ENABLE_UBSAN=ON)
    echo -e "${YELLOW}Enabling UndefinedBehaviorSanitizer${NC}"
elif [ "$ENABLE_SANITIZERS" = "TSAN" ]; then
    CMAKE_OPTIONS+=(-DSSSP_ENABLE_TSAN=ON)
    echo -e "${YELLOW}Enabling ThreadSanitizer${NC}"
elif [ "$ENABLE_SANITIZERS" = "MSAN" ] && [ "$COMPILER" = "clang" ]; then
    CMAKE_OPTIONS+=(-DSSSP_ENABLE_MSAN=ON)
    echo -e "${YELLOW}Enabling MemorySanitizer${NC}"
fi

# Configure
echo -e "\n${GREEN}Configuring CMake...${NC}"
cmake "${CMAKE_OPTIONS[@]}" "${PROJECT_DIR}"

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build
echo -e "\n${GREEN}Building project...${NC}"
cmake --build . --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build succeeded!${NC}"

# Run tests
if [ "$RUN_TESTS" = "ON" ]; then
    echo -e "\n${GREEN}Running tests...${NC}"

    # Run CTest
    if ! ctest --output-on-failure --parallel $(nproc); then
        echo -e "${RED}Tests failed!${NC}"
        exit 1
    fi

    echo -e "${GREEN}All tests passed!${NC}"

    # Run with Valgrind if requested
    if [ "$RUN_VALGRIND" = "ON" ] && command -v valgrind &> /dev/null; then
        echo -e "\n${YELLOW}Running tests with Valgrind...${NC}"

        # Find test executables
        for test_exe in $(find "${BUILD_DIR}/bin" -type f -executable); do
            echo -e "${BLUE}Checking ${test_exe} with Valgrind...${NC}"

            if ! valgrind \
                --leak-check=full \
                --show-leak-kinds=all \
                --track-origins=yes \
                --error-exitcode=1 \
                "${test_exe}"; then
                echo -e "${RED}Valgrind detected issues in ${test_exe}${NC}"
                exit 1
            fi
        done

        echo -e "${GREEN}Valgrind checks passed!${NC}"
    fi
fi

# Install
echo -e "\n${GREEN}Installing to ${INSTALL_PREFIX}...${NC}"
cmake --install .

if [ $? -ne 0 ]; then
    echo -e "${RED}Installation failed!${NC}"
    exit 1
fi

# Verify installation
echo -e "\n${GREEN}Verifying installation...${NC}"
if [ -f "${INSTALL_PREFIX}/include/sssp.h" ]; then
    echo -e "${GREEN}✓ Headers installed${NC}"
else
    echo -e "${RED}✗ Headers missing${NC}"
    exit 1
fi

if [ -f "${INSTALL_PREFIX}/lib/libsssp.so" ] || [ -f "${INSTALL_PREFIX}/lib/libsssp.a" ]; then
    echo -e "${GREEN}✓ Libraries installed${NC}"
else
    echo -e "${RED}✗ Libraries missing${NC}"
    exit 1
fi

if [ -f "${INSTALL_PREFIX}/lib/pkgconfig/libsssp.pc" ]; then
    echo -e "${GREEN}✓ pkg-config file installed${NC}"
else
    echo -e "${RED}✗ pkg-config file missing${NC}"
    exit 1
fi

# Build summary
echo -e "\n${BLUE}==================================================${NC}"
echo -e "${GREEN}Build Summary${NC}"
echo -e "${BLUE}==================================================${NC}"
echo -e "Build type:     ${BUILD_TYPE}"
echo -e "Compiler:       $(${CC} --version | head -n1)"
echo -e "Tests:          ${RUN_TESTS}"
if [ "$RUN_TESTS" = "ON" ]; then
    echo -e "Test results:   ${GREEN}PASSED${NC}"
fi
echo -e "Install prefix: ${INSTALL_PREFIX}"
echo -e "${BLUE}==================================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
