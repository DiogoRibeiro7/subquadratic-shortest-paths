# CMake toolchain file for cross-compiling to macOS from Linux
# Requires osxcross: https://github.com/tpoechtrager/osxcross
# Usage: cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-macos-cross.cmake

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set the target macOS version (adjust as needed)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

# Specify osxcross prefix (adjust to your installation)
set(OSXCROSS_ROOT $ENV{OSXCROSS_ROOT})
if(NOT OSXCROSS_ROOT)
    set(OSXCROSS_ROOT "/opt/osxcross")
endif()

set(OSXCROSS_TARGET "darwin21")  # macOS 12 (Monterey), adjust as needed
set(OSXCROSS_SDK "${OSXCROSS_ROOT}/SDK/MacOSX12.0.sdk")

# Specify the cross compiler
set(CMAKE_C_COMPILER "${OSXCROSS_ROOT}/bin/o64-clang")
set(CMAKE_CXX_COMPILER "${OSXCROSS_ROOT}/bin/o64-clang++")
set(CMAKE_AR "${OSXCROSS_ROOT}/bin/x86_64-apple-${OSXCROSS_TARGET}-ar" CACHE FILEPATH "ar")
set(CMAKE_RANLIB "${OSXCROSS_ROOT}/bin/x86_64-apple-${OSXCROSS_TARGET}-ranlib" CACHE FILEPATH "ranlib")

# Where to look for the target environment
set(CMAKE_FIND_ROOT_PATH "${OSXCROSS_SDK}")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# macOS specific settings
set(CMAKE_OSX_SYSROOT "${OSXCROSS_SDK}")
set(APPLE TRUE)
set(UNIX TRUE)
