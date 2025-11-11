cmake_minimum_required(VERSION 3.15)

# Read version from file
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" SSSP_VERSION)
string(STRIP "${SSSP_VERSION}" SSSP_VERSION)
string(REPLACE "." ";" VERSION_LIST ${SSSP_VERSION})
list(GET VERSION_LIST 0 SSSP_VERSION_MAJOR)
list(GET VERSION_LIST 1 SSSP_VERSION_MINOR)
list(GET VERSION_LIST 2 SSSP_VERSION_PATCH)

project(libsssp
    VERSION ${SSSP_VERSION}
    DESCRIPTION "O(m log^{2/3} n) Breakthrough Single-Source Shortest Paths Algorithm"
    HOMEPAGE_URL "https://github.com/your-org/subquadratic-shortest-paths"
    LANGUAGES C
)

# ============================================================================
# Build options
# ============================================================================
include(CMakeDependentOption)

option(SSSP_BUILD_SHARED "Build shared library" ON)
option(SSSP_BUILD_STATIC "Build static library" ON)
option(SSSP_BUILD_TESTS "Build test suite" ON)
option(SSSP_BUILD_BENCHMARKS "Build benchmark suite" OFF)
option(SSSP_BUILD_EXAMPLES "Build example programs" ON)
option(SSSP_BUILD_DOCS "Build documentation (requires Doxygen)" OFF)
option(SSSP_INSTALL "Generate installation targets" ON)
option(SSSP_ENABLE_COVERAGE "Enable code coverage" OFF)
option(SSSP_ENABLE_LTO "Enable Link-Time Optimization" OFF)
option(SSSP_POSITION_INDEPENDENT_CODE "Enable position independent code (-fPIC)" ON)

# Sanitizer options
option(SSSP_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(SSSP_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(SSSP_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(SSSP_ENABLE_MSAN "Enable MemorySanitizer" OFF)

# Advanced options
cmake_dependent_option(SSSP_BUILD_DEMOS "Build demo executables" ON
    "SSSP_BUILD_SHARED OR SSSP_BUILD_STATIC" OFF)

# ============================================================================
# CMake policies and settings
# ============================================================================
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
endif()

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW)  # MSVC warning flags are not in CMAKE_<LANG>_FLAGS by default
endif()

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# ============================================================================
# Project-wide settings
# ============================================================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ${SSSP_POSITION_INDEPENDENT_CODE})

# Enable Link-Time Optimization if requested
if(SSSP_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "Link-Time Optimization enabled")
    else()
        message(WARNING "LTO requested but not supported: ${lto_error}")
    endif()
endif()

# ============================================================================
# Compiler flags
# ============================================================================
include(cmake/CompilerWarnings.cmake)

# Platform-specific flags
if(MSVC)
    # MSVC-specific flags
    add_compile_options(
        /W4                # Warning level 4
        /WX-               # Don't treat warnings as errors (configurable)
        /permissive-      # Standards conformance
        /Zc:__cplusplus   # Enable __cplusplus macro
        /utf-8            # Set source and execution character sets to UTF-8
    )
    # Disable specific MSVC warnings
    add_compile_options(
        /wd4996           # Disable deprecation warnings
    )
    # Release optimizations
    add_compile_options("$<$<CONFIG:Release>:/O2>")
    add_compile_options("$<$<CONFIG:Release>:/GL>")  # Whole program optimization
else()
    # GCC/Clang flags
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    # Architecture-specific optimizations for Release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        include(CheckCCompilerFlag)
        check_c_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
        if(COMPILER_SUPPORTS_MARCH_NATIVE AND NOT CMAKE_CROSSCOMPILING)
            add_compile_options(-march=native)
        endif()
    endif()
endif()

# ============================================================================
# Sanitizers
# ============================================================================
include(cmake/Sanitizers.cmake)

# ============================================================================
# Dependencies
# ============================================================================
find_library(MATH_LIBRARY m)
if(NOT MATH_LIBRARY)
    set(MATH_LIBRARY "")
    message(STATUS "Math library not found as separate library (likely built into C library)")
endif()

# ============================================================================
# Source files
# ============================================================================
set(SSSP_PUBLIC_HEADERS
    include/sssp.h
)

set(SSSP_PRIVATE_HEADERS
    src/sssp_breakthrough.h
    src/utils.h
)

set(SSSP_SOURCES
    src/complete_implementation.c
    src/sssp_api.c
    src/utils.c
)

# ============================================================================
# Library targets
# ============================================================================
# Shared library
if(SSSP_BUILD_SHARED)
    add_library(sssp_shared SHARED ${SSSP_SOURCES} ${SSSP_PRIVATE_HEADERS})
    add_library(libsssp::shared ALIAS sssp_shared)

    set_target_properties(sssp_shared PROPERTIES
        OUTPUT_NAME "sssp"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        PUBLIC_HEADER "${SSSP_PUBLIC_HEADERS}"
    )

    target_include_directories(sssp_shared
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(sssp_shared PRIVATE ${MATH_LIBRARY})

    # Define export macro for DLL on Windows
    target_compile_definitions(sssp_shared
        PRIVATE SSSP_BUILDING_DLL
        PUBLIC $<$<PLATFORM_ID:Windows>:SSSP_DLL>
    )

    if(SSSP_INSTALL)
        install(TARGETS sssp_shared
            EXPORT libssspTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
    endif()
endif()

# Static library
if(SSSP_BUILD_STATIC)
    add_library(sssp_static STATIC ${SSSP_SOURCES} ${SSSP_PRIVATE_HEADERS})
    add_library(libsssp::static ALIAS sssp_static)

    set_target_properties(sssp_static PROPERTIES
        OUTPUT_NAME "sssp"
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER "${SSSP_PUBLIC_HEADERS}"
    )

    target_include_directories(sssp_static
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(sssp_static PRIVATE ${MATH_LIBRARY})

    if(SSSP_INSTALL)
        install(TARGETS sssp_static
            EXPORT libssspTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
    endif()
endif()

# Create an alias for the default library type
if(SSSP_BUILD_SHARED)
    add_library(libsssp::libsssp ALIAS sssp_shared)
    set(SSSP_DEFAULT_LIBRARY sssp_shared)
elseif(SSSP_BUILD_STATIC)
    add_library(libsssp::libsssp ALIAS sssp_static)
    set(SSSP_DEFAULT_LIBRARY sssp_static)
endif()

# ============================================================================
# Demo programs
# ============================================================================
if(SSSP_BUILD_DEMOS AND TARGET ${SSSP_DEFAULT_LIBRARY})
    # API demo
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/demo_api.c")
        add_executable(demo_api demo_api.c)
        target_link_libraries(demo_api PRIVATE ${SSSP_DEFAULT_LIBRARY})
        set_target_properties(demo_api PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()

    # SSSP demo
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/sssp_demo.c")
        add_executable(sssp_demo src/sssp_demo.c)
        target_link_libraries(sssp_demo PRIVATE ${SSSP_DEFAULT_LIBRARY})
        target_include_directories(sssp_demo PRIVATE src)
        set_target_properties(sssp_demo PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
endif()

# ============================================================================
# Examples
# ============================================================================
if(SSSP_BUILD_EXAMPLES AND TARGET ${SSSP_DEFAULT_LIBRARY})
    file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.c")
    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
        target_link_libraries(${EXAMPLE_NAME} PRIVATE ${SSSP_DEFAULT_LIBRARY})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
        )
    endforeach()
endif()

# ============================================================================
# Tests
# ============================================================================
if(SSSP_BUILD_TESTS AND TARGET ${SSSP_DEFAULT_LIBRARY})
    enable_testing()

    # API test suite
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/test_api.c")
        add_executable(test_api src/test_api.c)
        target_link_libraries(test_api PRIVATE ${SSSP_DEFAULT_LIBRARY})
        target_include_directories(test_api PRIVATE src)
        add_test(NAME APITests COMMAND test_api)
        set_tests_properties(APITests PROPERTIES
            TIMEOUT 300
            LABELS "unit;api"
        )
    endif()

    # Debug test
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/test_debug.c")
        add_executable(test_debug src/test_debug.c)
        target_link_libraries(test_debug PRIVATE ${SSSP_DEFAULT_LIBRARY})
        target_include_directories(test_debug PRIVATE src)
        add_test(NAME DebugTest COMMAND test_debug)
        set_tests_properties(DebugTest PROPERTIES LABELS "debug")
    endif()

    # Medium graph test
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/test_medium_graph.c")
        add_executable(test_medium_graph src/test_medium_graph.c)
        target_link_libraries(test_medium_graph PRIVATE ${SSSP_DEFAULT_LIBRARY})
        target_include_directories(test_medium_graph PRIVATE src)
        add_test(NAME MediumGraphTest COMMAND test_medium_graph)
        set_tests_properties(MediumGraphTest PROPERTIES LABELS "integration")
    endif()

    # Large graph test
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/test_large_graph.c")
        add_executable(test_large_graph src/test_large_graph.c)
        target_link_libraries(test_large_graph PRIVATE ${SSSP_DEFAULT_LIBRARY})
        target_include_directories(test_large_graph PRIVATE src)
        add_test(NAME LargeGraphTest COMMAND test_large_graph)
        set_tests_properties(LargeGraphTest PROPERTIES
            TIMEOUT 600
            LABELS "stress"
        )
    endif()

    # Additional test files in tests/ directory
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE ${SSSP_DEFAULT_LIBRARY})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()

# ============================================================================
# Benchmarks
# ============================================================================
if(SSSP_BUILD_BENCHMARKS AND TARGET ${SSSP_DEFAULT_LIBRARY})
    file(GLOB BENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.c")
    foreach(BENCH_SOURCE ${BENCHMARK_SOURCES})
        get_filename_component(BENCH_NAME ${BENCH_SOURCE} NAME_WE)
        add_executable(${BENCH_NAME} ${BENCH_SOURCE})
        target_link_libraries(${BENCH_NAME} PRIVATE ${SSSP_DEFAULT_LIBRARY})
        set_target_properties(${BENCH_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
        )
        # Optionally add as CTest benchmark
        add_test(NAME ${BENCH_NAME} COMMAND ${BENCH_NAME})
        set_tests_properties(${BENCH_NAME} PROPERTIES
            LABELS "benchmark;performance"
            TIMEOUT 1200
        )
    endforeach()
endif()

# ============================================================================
# Documentation
# ============================================================================
if(SSSP_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    else()
        message(WARNING "Doxygen not found, cannot build documentation")
    endif()
endif()

# ============================================================================
# Installation
# ============================================================================
if(SSSP_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(FILES ${SSSP_PUBLIC_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install documentation
    install(FILES
        README.md
        CITATION.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )

    # Generate and install package config files
    set(SSSP_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libsssp")

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libssspConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libssspConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/libssspConfig.cmake"
        INSTALL_DESTINATION ${SSSP_CMAKE_CONFIG_DESTINATION}
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libssspConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libssspConfigVersion.cmake"
        DESTINATION ${SSSP_CMAKE_CONFIG_DESTINATION}
    )

    install(EXPORT libssspTargets
        FILE libssspTargets.cmake
        NAMESPACE libsssp::
        DESTINATION ${SSSP_CMAKE_CONFIG_DESTINATION}
    )

    # Generate and install pkg-config file
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libsssp.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/libsssp.pc"
        @ONLY
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libsssp.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    )
endif()

# ============================================================================
# CPack - Package generation
# ============================================================================
include(cmake/Packaging.cmake)

# ============================================================================
# Build summary
# ============================================================================
message(STATUS "")
message(STATUS "============================================")
message(STATUS "  libsssp ${PROJECT_VERSION} - Build Configuration")
message(STATUS "============================================")
message(STATUS "Build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler:          ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Install prefix:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Library options:")
message(STATUS "  Shared library:    ${SSSP_BUILD_SHARED}")
message(STATUS "  Static library:    ${SSSP_BUILD_STATIC}")
message(STATUS "  Position-independent code: ${SSSP_POSITION_INDEPENDENT_CODE}")
if(SSSP_ENABLE_LTO AND lto_supported)
message(STATUS "  Link-time optimization:    ON")
endif()
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  Tests:             ${SSSP_BUILD_TESTS}")
message(STATUS "  Benchmarks:        ${SSSP_BUILD_BENCHMARKS}")
message(STATUS "  Examples:          ${SSSP_BUILD_EXAMPLES}")
message(STATUS "  Demos:             ${SSSP_BUILD_DEMOS}")
message(STATUS "  Documentation:     ${SSSP_BUILD_DOCS}")
message(STATUS "")
if(SSSP_ENABLE_ASAN OR SSSP_ENABLE_UBSAN OR SSSP_ENABLE_TSAN OR SSSP_ENABLE_MSAN)
message(STATUS "Sanitizers enabled:")
if(SSSP_ENABLE_ASAN)
message(STATUS "  AddressSanitizer:  ON")
endif()
if(SSSP_ENABLE_UBSAN)
message(STATUS "  UBSanitizer:       ON")
endif()
if(SSSP_ENABLE_TSAN)
message(STATUS "  ThreadSanitizer:   ON")
endif()
if(SSSP_ENABLE_MSAN)
message(STATUS "  MemorySanitizer:   ON")
endif()
message(STATUS "")
endif()
message(STATUS "Installation:        ${SSSP_INSTALL}")
message(STATUS "============================================")
message(STATUS "")
