cmake_minimum_required(VERSION 3.10)
project(SSSP_Breakthrough VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options for build configuration
option(BUILD_DEMO "Build the demo executable" ON)
option(BUILD_COMPLETE "Build the complete implementation" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_BENCHMARKS "Build benchmark suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

# Optimization level options
set(OPTIMIZATION_LEVEL "DEFAULT" CACHE STRING "Optimization level: DEFAULT, O0, O1, O2, O3, Ofast")
set_property(CACHE OPTIMIZATION_LEVEL PROPERTY STRINGS DEFAULT O0 O1 O2 O3 Ofast)

# Configure optimization flags
if(NOT CMAKE_BUILD_TYPE AND NOT OPTIMIZATION_LEVEL STREQUAL "DEFAULT")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug or Release)" FORCE)
endif()

# Set optimization flags based on OPTIMIZATION_LEVEL
if(NOT OPTIMIZATION_LEVEL STREQUAL "DEFAULT")
    if(OPTIMIZATION_LEVEL STREQUAL "O0")
        set(OPT_FLAGS "-O0")
    elseif(OPTIMIZATION_LEVEL STREQUAL "O1")
        set(OPT_FLAGS "-O1")
    elseif(OPTIMIZATION_LEVEL STREQUAL "O2")
        set(OPT_FLAGS "-O2")
    elseif(OPTIMIZATION_LEVEL STREQUAL "O3")
        set(OPT_FLAGS "-O3")
    elseif(OPTIMIZATION_LEVEL STREQUAL "Ofast")
        set(OPT_FLAGS "-Ofast")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPT_FLAGS}")
endif()

# Debug and Release configurations
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

# Find required libraries
find_library(MATH_LIBRARY m)
if(NOT MATH_LIBRARY)
    message(STATUS "Math library not found as separate library (may be built into C library)")
    set(MATH_LIBRARY "")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(HEADER_FILES
    src/sssp_breakthrough.h
    src/utils.h
)

set(UTILITY_SOURCES
    src/utils.c
)

# Public API header
set(PUBLIC_HEADER_FILES
    include/sssp.h
)

# Demo executable
if(BUILD_DEMO)
    add_executable(sssp_demo
        src/sssp_demo.c
        ${HEADER_FILES}
    )

    target_link_libraries(sssp_demo ${MATH_LIBRARY})

    # Set properties for demo
    set_target_properties(sssp_demo PROPERTIES
        OUTPUT_NAME "sssp_demo"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    message(STATUS "Building demo executable: sssp_demo")
endif()

# Complete implementation executable
if(BUILD_COMPLETE)
    add_executable(sssp_complete
        src/complete_implementation.c
        ${HEADER_FILES}
    )

    target_link_libraries(sssp_complete ${MATH_LIBRARY})

    # Set properties for complete implementation
    set_target_properties(sssp_complete PROPERTIES
        OUTPUT_NAME "sssp_complete"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    message(STATUS "Building complete implementation: sssp_complete")
endif()

# Test suite
if(BUILD_TESTS)
    # Check if test files exist
    file(GLOB TEST_SOURCES "src/*test*.c" "tests/*.c")

    if(TEST_SOURCES)
        # Include utility sources for tests
        add_executable(sssp_tests
            ${TEST_SOURCES}
            ${HEADER_FILES}
            ${UTILITY_SOURCES}
        )

        target_link_libraries(sssp_tests ${MATH_LIBRARY})

        set_target_properties(sssp_tests PROPERTIES
            OUTPUT_NAME "sssp_tests"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

        # Enable CTest integration
        enable_testing()
        add_test(NAME unit_tests COMMAND sssp_tests)

        # Set test properties
        set_tests_properties(unit_tests PROPERTIES
            TIMEOUT 300
            LABELS "unit"
        )

        message(STATUS "Building test suite: sssp_tests")
    else()
        message(STATUS "No test files found. Skipping test suite build.")
    endif()
endif()

# Benchmark suite (performance tests)
if(BUILD_BENCHMARKS)
    # Check if benchmark files exist
    file(GLOB BENCH_SOURCES "src/*bench*.c" "benchmarks/*.c" "tests/performance_tests.c")

    if(BENCH_SOURCES)
        # Include utility sources for benchmarks
        add_executable(sssp_benchmarks
            ${BENCH_SOURCES}
            ${HEADER_FILES}
            ${UTILITY_SOURCES}
        )

        target_link_libraries(sssp_benchmarks ${MATH_LIBRARY})

        set_target_properties(sssp_benchmarks PROPERTIES
            OUTPUT_NAME "sssp_benchmarks"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

        # Enable CTest integration for performance tests
        enable_testing()
        add_test(NAME performance_tests COMMAND sssp_benchmarks)

        # Set test properties
        set_tests_properties(performance_tests PROPERTIES
            TIMEOUT 600
            LABELS "performance;benchmark"
        )

        message(STATUS "Building benchmark suite: sssp_benchmarks")
    else()
        message(STATUS "No benchmark files found. Skipping benchmark suite build.")
    endif()
endif()

# Example programs
if(BUILD_EXAMPLES)
    # Check if example files exist
    file(GLOB EXAMPLE_SOURCES "examples/*.c")

    if(EXAMPLE_SOURCES)
        foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
            # Get filename without extension
            get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)

            add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE} ${HEADER_FILES})
            target_link_libraries(${EXAMPLE_NAME} ${MATH_LIBRARY})

            set_target_properties(${EXAMPLE_NAME} PROPERTIES
                OUTPUT_NAME "${EXAMPLE_NAME}"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
            )

            message(STATUS "Building example: ${EXAMPLE_NAME}")
        endforeach()
    else()
        message(STATUS "No example files found. Skipping examples build.")
    endif()
endif()

# Installation rules
include(GNUInstallDirs)

if(BUILD_DEMO)
    install(TARGETS sssp_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(BUILD_COMPLETE)
    install(TARGETS sssp_complete
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install public API header files
install(FILES ${PUBLIC_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install documentation
install(FILES
    README.md
    TECHNICAL_ANALYSIS.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# Print build configuration summary
message(STATUS "")
message(STATUS "=== SSSP Breakthrough Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Standard: C${CMAKE_C_STANDARD}")
if(NOT OPTIMIZATION_LEVEL STREQUAL "DEFAULT")
    message(STATUS "Optimization level: ${OPTIMIZATION_LEVEL}")
endif()
message(STATUS "Build demo: ${BUILD_DEMO}")
message(STATUS "Build complete: ${BUILD_COMPLETE}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===========================================")
message(STATUS "")
