# Sanitizers.cmake - Configures sanitizer builds
# Supports: AddressSanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer, MemorySanitizer

if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(SSSP_ENABLE_ASAN OR SSSP_ENABLE_UBSAN OR SSSP_ENABLE_TSAN OR SSSP_ENABLE_MSAN)
        message(WARNING "Sanitizers are only supported with GCC and Clang compilers")
    endif()
    return()
endif()

set(SANITIZER_FLAGS "")
set(SANITIZER_LINK_FLAGS "")

# AddressSanitizer (detects memory errors)
if(SSSP_ENABLE_ASAN)
    list(APPEND SANITIZER_FLAGS "-fsanitize=address")
    list(APPEND SANITIZER_FLAGS "-fno-omit-frame-pointer")
    list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=address")
    message(STATUS "AddressSanitizer enabled")
endif()

# UndefinedBehaviorSanitizer (detects undefined behavior)
if(SSSP_ENABLE_UBSAN)
    list(APPEND SANITIZER_FLAGS "-fsanitize=undefined")
    list(APPEND SANITIZER_FLAGS "-fno-omit-frame-pointer")
    list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=undefined")
    message(STATUS "UndefinedBehaviorSanitizer enabled")
endif()

# ThreadSanitizer (detects data races) - mutually exclusive with ASan and MSan
if(SSSP_ENABLE_TSAN)
    if(SSSP_ENABLE_ASAN OR SSSP_ENABLE_MSAN)
        message(FATAL_ERROR "ThreadSanitizer is incompatible with AddressSanitizer and MemorySanitizer")
    endif()
    list(APPEND SANITIZER_FLAGS "-fsanitize=thread")
    list(APPEND SANITIZER_FLAGS "-fno-omit-frame-pointer")
    list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=thread")
    message(STATUS "ThreadSanitizer enabled")
endif()

# MemorySanitizer (detects uninitialized memory reads) - Clang only
if(SSSP_ENABLE_MSAN)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "MemorySanitizer is only supported with Clang")
    endif()
    if(SSSP_ENABLE_ASAN OR SSSP_ENABLE_TSAN)
        message(FATAL_ERROR "MemorySanitizer is incompatible with AddressSanitizer and ThreadSanitizer")
    endif()
    list(APPEND SANITIZER_FLAGS "-fsanitize=memory")
    list(APPEND SANITIZER_FLAGS "-fno-omit-frame-pointer")
    list(APPEND SANITIZER_FLAGS "-fsanitize-memory-track-origins")
    list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=memory")
    message(STATUS "MemorySanitizer enabled")
endif()

# Apply sanitizer flags
if(SANITIZER_FLAGS)
    add_compile_options(${SANITIZER_FLAGS})
    string(REPLACE ";" " " SANITIZER_LINK_FLAGS_STR "${SANITIZER_LINK_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_LINK_FLAGS_STR}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SANITIZER_LINK_FLAGS_STR}")

    # Disable optimizations for better stack traces
    add_compile_options(-O1 -g)

    message(STATUS "Sanitizer compile flags: ${SANITIZER_FLAGS}")
    message(STATUS "Sanitizer link flags: ${SANITIZER_LINK_FLAGS_STR}")
endif()
