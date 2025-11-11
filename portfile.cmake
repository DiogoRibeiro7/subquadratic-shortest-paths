# vcpkg portfile for libsssp
# This file defines how vcpkg should build and install libsssp

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO your-org/subquadratic-shortest-paths
    REF v1.0.0
    SHA512 0  # Replace with actual SHA512 of the release archive
    HEAD_REF main
)

# Check for available features
set(FEATURE_OPTIONS "")
if("tests" IN_LIST FEATURES)
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_TESTS=ON)
else()
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_TESTS=OFF)
endif()

if("benchmarks" IN_LIST FEATURES)
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_BENCHMARKS=ON)
else()
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_BENCHMARKS=OFF)
endif()

if("examples" IN_LIST FEATURES)
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_EXAMPLES=ON)
else()
    list(APPEND FEATURE_OPTIONS -DSSSP_BUILD_EXAMPLES=OFF)
endif()

# Configure CMake
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DSSSP_BUILD_SHARED=ON
        -DSSSP_BUILD_STATIC=ON
        -DSSSP_BUILD_DOCS=OFF
        -DSSSP_INSTALL=ON
        ${FEATURE_OPTIONS}
    MAYBE_UNUSED_VARIABLES
        SSSP_BUILD_BENCHMARKS
)

# Build the project
vcpkg_cmake_install()

# Fix up CMake config files
vcpkg_cmake_config_fixup(
    PACKAGE_NAME libsssp
    CONFIG_PATH lib/cmake/libsssp
)

# Fix up pkg-config files
vcpkg_fixup_pkgconfig()

# Remove duplicate files from debug directory
file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

# Handle copyright
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

# Copy usage file
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
