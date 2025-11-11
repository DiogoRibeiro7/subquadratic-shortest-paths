"""
Conan recipe for libsssp - O(m log^{2/3} n) Breakthrough SSSP Algorithm

This recipe allows libsssp to be easily integrated into projects using Conan.

Usage:
    # Install from source
    conan create . --build=missing

    # Use in conanfile.txt
    [requires]
    libsssp/1.0.0

    [generators]
    CMakeDeps
    CMakeToolchain

    # Use in conanfile.py
    def requirements(self):
        self.requires("libsssp/1.0.0")
"""

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy, load
from conan.tools.scm import Version
import os


class LibSSSPConan(ConanFile):
    name = "libsssp"
    license = "MIT"
    author = "SSSP Development Team"
    url = "https://github.com/your-org/subquadratic-shortest-paths"
    description = "High-performance O(m log^{2/3} n) Single-Source Shortest Paths algorithm"
    topics = ("graph-algorithms", "shortest-path", "sssp", "performance")

    # Package settings
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "enable_lto": [True, False],
        "enable_tests": [True, False],
        "enable_benchmarks": [True, False],
        "enable_examples": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "enable_lto": False,
        "enable_tests": False,
        "enable_benchmarks": False,
        "enable_examples": False,
    }

    # Sources are in the same repository
    exports_sources = (
        "CMakeLists.txt",
        "VERSION",
        "cmake/*",
        "src/*",
        "include/*",
        "README.md",
        "LICENSE",
        "CITATION.md",
    )

    def set_version(self):
        """Read version from VERSION file"""
        version_file = os.path.join(self.recipe_folder, "VERSION")
        self.version = load(self, version_file).strip()

    def config_options(self):
        """Remove invalid options based on settings"""
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        """Configure package options"""
        if self.options.shared:
            # Shared library doesn't need fPIC
            self.options.rm_safe("fPIC")

    def layout(self):
        """Define package layout"""
        cmake_layout(self, src_folder=".")

    def generate(self):
        """Generate build system files"""
        # Generate CMake toolchain
        tc = CMakeToolchain(self)

        # Map Conan options to CMake options
        tc.variables["SSSP_BUILD_SHARED"] = self.options.shared
        tc.variables["SSSP_BUILD_STATIC"] = not self.options.shared
        tc.variables["SSSP_BUILD_TESTS"] = self.options.enable_tests
        tc.variables["SSSP_BUILD_BENCHMARKS"] = self.options.enable_benchmarks
        tc.variables["SSSP_BUILD_EXAMPLES"] = self.options.enable_examples
        tc.variables["SSSP_ENABLE_LTO"] = self.options.enable_lto
        tc.variables["SSSP_BUILD_DOCS"] = False  # Don't build docs in Conan

        # Position independent code
        if self.settings.os != "Windows":
            tc.variables["SSSP_POSITION_INDEPENDENT_CODE"] = self.options.get_safe("fPIC", True)

        tc.generate()

        # Generate CMake dependencies
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        """Build the package"""
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        # Run tests if enabled
        if self.options.enable_tests:
            cmake.test()

    def package(self):
        """Package the built files"""
        cmake = CMake(self)
        cmake.install()

        # Copy license
        copy(self, "LICENSE",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))

        # Copy documentation
        for doc in ["README.md", "CITATION.md"]:
            copy(self, doc,
                 src=self.source_folder,
                 dst=os.path.join(self.package_folder, "docs"))

    def package_info(self):
        """Define package information for consumers"""
        # Library name
        self.cpp_info.libs = ["sssp"]

        # Include directories
        self.cpp_info.includedirs = ["include"]

        # System libraries
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.system_libs.append("m")  # Math library

        # Set component for shared/static library
        if self.options.shared:
            self.cpp_info.components["shared"].libs = ["sssp"]
            self.cpp_info.components["shared"].includedirs = ["include"]
        else:
            self.cpp_info.components["static"].libs = ["sssp"]
            self.cpp_info.components["static"].includedirs = ["include"]

        # pkg-config name
        self.cpp_info.set_property("pkg_config_name", "libsssp")

        # CMake target name
        self.cpp_info.set_property("cmake_target_name", "libsssp::libsssp")
        self.cpp_info.set_property("cmake_file_name", "libsssp")

        # Build directories (for editable mode)
        self.cpp_info.builddirs = ["lib/cmake/libsssp"]

    def package_id(self):
        """Define package ID for binary compatibility"""
        # Package is header-only compatible across different compilers
        # if using static linking
        if not self.options.shared:
            del self.info.settings.compiler.version
