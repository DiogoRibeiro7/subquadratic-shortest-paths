# CompilerWarnings.cmake - Comprehensive compiler warning configuration

# This module sets up comprehensive warning flags for different compilers
# to help catch potential bugs and enforce code quality.

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Common warnings for GCC and Clang
    set(COMMON_WARNINGS
        -Wall                    # Enable most warnings
        -Wextra                  # Enable extra warnings
        -Wpedantic               # Strict ISO C compliance
        -Wshadow                 # Warn on variable shadowing
        -Wcast-align             # Warn on alignment issues in casts
        -Wunused                 # Warn on unused variables/functions
        -Wformat=2               # Extra format string checking
        -Wformat-security        # Security issues in format strings
        -Wnull-dereference       # Potential null pointer dereferences
        -Wdouble-promotion       # Implicit float to double promotion
        -Wimplicit-fallthrough   # Missing fallthrough annotation in switch
    )

    # GCC-specific warnings
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        list(APPEND COMMON_WARNINGS
            -Wlogical-op             # Suspicious use of logical operators
            -Wduplicated-cond        # Duplicated if-else conditions
            -Wduplicated-branches    # Identical if-else branches
        )
    endif()

    # Clang-specific warnings
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        list(APPEND COMMON_WARNINGS
            -Wconversion             # Implicit type conversions
            -Wsign-conversion        # Sign conversion issues
        )
    endif()

    # Apply warnings
    add_compile_options(${COMMON_WARNINGS})

elseif(MSVC)
    # MSVC warnings
    add_compile_options(
        /W4              # Warning level 4 (high)
        /w14242          # 'identifier': conversion, possible loss of data
        /w14254          # 'operator': conversion, possible loss of data
        /w14263          # Member function does not override base class virtual
        /w14265          # Class has virtual functions but destructor is not virtual
        /w14287          # Unsigned/negative constant mismatch
        /we4289          # Loop control variable used outside loop
        /w14296          # Expression is always true/false
        /w14311          # Pointer truncation
        /w14545          # Expression before comma has no effect
        /w14546          # Function call before comma missing argument list
        /w14547          # Operator before comma has no effect
        /w14549          # Operator before comma has no effect
        /w14555          # Expression has no effect
        /w14619          # Pragma warning with non-existent warning number
        /w14640          # Thread-unsafe static member initialization
        /w14826          # Conversion is sign-extended
        /w14905          # Wide string literal cast to LPSTR
        /w14906          # String literal cast to LPWSTR
        /w14928          # Illegal copy-initialization
    )
endif()

message(STATUS "Compiler warnings configured for ${CMAKE_C_COMPILER_ID}")
