# CMake toolchain file for cross-compiling to RISC-V 64-bit Linux
# Usage: cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-riscv64-linux.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Specify the cross compiler
set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)

# Where to look for the target environment
set(CMAKE_FIND_ROOT_PATH /usr/riscv64-linux-gnu)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compiler flags for RISC-V 64
# rv64gc = rv64imafdc (general-purpose ISA with compressed instructions)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64gc -mabi=lp64d" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=rv64gc -mabi=lp64d" CACHE STRING "" FORCE)
