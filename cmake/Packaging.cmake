# Packaging.cmake - CPack configuration for creating distributable packages

# Only configure packaging if this is the main project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    return()
endif()

include(InstallRequiredSystemLibraries)

# Package metadata
set(CPACK_PACKAGE_NAME "libsssp")
set(CPACK_PACKAGE_VENDOR "SSSP Breakthrough Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "O(m log^{2/3} n) Breakthrough SSSP Algorithm")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Version information
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Contact information
set(CPACK_PACKAGE_CONTACT "sssp-dev@example.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/your-org/subquadratic-shortest-paths")

# Installation directory
set(CPACK_PACKAGE_INSTALL_DIRECTORY "libsssp-${PROJECT_VERSION}")

# ============================================================================
# Source package configuration
# ============================================================================
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "libsssp-${PROJECT_VERSION}-source")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/\\.github/"
    "/\\.vscode/"
    "/\\.idea/"
    "/build/"
    "/cmake-build-.*/"
    "\\.swp$"
    "\\.orig$"
    "\\.DS_Store$"
    "~$"
)

# ============================================================================
# Binary package configuration (platform-specific)
# ============================================================================

if(WIN32)
    # Windows-specific packaging

    # NSIS installer for Windows
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "libsssp ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "libsssp")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_HELP_LINK "${CPACK_PACKAGE_HOMEPAGE_URL}")

    # WiX installer as alternative
    list(APPEND CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "B6F8A8E6-5C4D-4B9E-8F3A-1D2C3E4F5A6B")
    set(CPACK_WIX_PRODUCT_GUID "*")  # Generate new GUID for each version
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

    # NuGet package
    list(APPEND CPACK_GENERATOR "NuGet")
    set(CPACK_NUGET_PACKAGE_NAME "libsssp")
    set(CPACK_NUGET_PACKAGE_TITLE "libsssp - Breakthrough SSSP Algorithm")
    set(CPACK_NUGET_PACKAGE_AUTHORS "SSSP Development Team")
    set(CPACK_NUGET_PACKAGE_OWNERS "SSSP Project")
    set(CPACK_NUGET_PACKAGE_DESCRIPTION "High-performance implementation of the O(m log^{2/3} n) breakthrough single-source shortest paths algorithm")
    set(CPACK_NUGET_PACKAGE_LICENSE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}/blob/main/LICENSE")
    set(CPACK_NUGET_PACKAGE_PROJECT_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NUGET_PACKAGE_TAGS "graph-algorithms;shortest-path;sssp;performance")

elseif(APPLE)
    # macOS-specific packaging

    set(CPACK_GENERATOR "TGZ;ZIP")

    # macOS Bundle/DMG (optional, requires additional configuration)
    list(APPEND CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "libsssp-${PROJECT_VERSION}")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DMGSetup.scpt")

    # macOS Package (.pkg)
    list(APPEND CPACK_GENERATOR "productbuild")
    set(CPACK_PRODUCTBUILD_IDENTITY_NAME "Developer ID Installer")

else()
    # Linux/Unix-specific packaging

    set(CPACK_GENERATOR "TGZ;TBZ2;DEB;RPM")

    # Debian package (.deb)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17)")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
    set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)

    # RPM package (.rpm)
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.17")
    set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
    set(CPACK_RPM_PACKAGE_AUTOREQ ON)
    set(CPACK_RPM_PACKAGE_AUTOPROV ON)

    # Separate debug symbols package
    set(CPACK_RPM_DEBUGINFO_PACKAGE ON)
    set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
endif()

# ============================================================================
# Component-based packaging
# ============================================================================

# Define components for finer-grained packaging
set(CPACK_COMPONENTS_ALL libraries headers development documentation)

# Runtime libraries component
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Runtime Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Shared libraries for running applications")
set(CPACK_COMPONENT_LIBRARIES_GROUP "Runtime")
set(CPACK_COMPONENT_LIBRARIES_REQUIRED ON)

# Header files component
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "C header files for development")
set(CPACK_COMPONENT_HEADERS_GROUP "Development")
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

# Development files component (static libs, CMake config, pkg-config)
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development Files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Static libraries and build system files")
set(CPACK_COMPONENT_DEVELOPMENT_GROUP "Development")
set(CPACK_COMPONENT_DEVELOPMENT_DEPENDS headers)

# Documentation component
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "API documentation and examples")
set(CPACK_COMPONENT_DOCUMENTATION_GROUP "Documentation")

# Component groups
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "Runtime files needed to use the library")
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "Files needed to develop with the library")
set(CPACK_COMPONENT_GROUP_DOCUMENTATION_DESCRIPTION "Documentation and examples")

# ============================================================================
# Archive naming
# ============================================================================

# Get system information for package name
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686|i386")
    set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
    set(ARCH "armv7")
else()
    set(ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Package file name
set(CPACK_PACKAGE_FILE_NAME "libsssp-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCH}")

# ============================================================================
# Additional package metadata
# ============================================================================

set(CPACK_STRIP_FILES ON)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

# Include CPack module (must be last)
include(CPack)

message(STATUS "Package generation configured:")
message(STATUS "  Generators: ${CPACK_GENERATOR}")
message(STATUS "  Package name: ${CPACK_PACKAGE_FILE_NAME}")
message(STATUS "  Components: ${CPACK_COMPONENTS_ALL}")
