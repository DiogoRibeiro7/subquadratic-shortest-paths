# Packaging Guide for libsssp

This document describes how to create distributable packages of libsssp for various platforms and package managers.

## Table of Contents

- [CPack Packages](#cpack-packages)
- [Conan Packages](#conan-packages)
- [vcpkg Packages](#vcpkg-packages)
- [Distribution-Specific Packages](#distribution-specific-packages)
- [Publishing](#publishing)

---

## CPack Packages

CPack is integrated into the CMake build system and supports multiple package formats.

### Supported Package Formats

- **Linux**: DEB, RPM, TGZ, TBZ2
- **macOS**: DMG, PKG, TGZ
- **Windows**: NSIS, WIX, ZIP, NuGet

### Creating Packages

```bash
# Build the project first
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Create all default packages for your platform
cd build
cpack

# Create specific package type
cpack -G DEB      # Debian package
cpack -G RPM      # RPM package
cpack -G TGZ      # Tarball
cpack -G NSIS     # Windows installer
cpack -G NuGet    # NuGet package
```

### Source Package

```bash
cd build
cpack -G TGZ --config CPackSourceConfig.cmake
```

### Customizing Packages

Edit `cmake/Packaging.cmake` to customize:
- Package metadata
- Component grouping
- Installation rules
- Generator-specific options

---

## Conan Packages

### Local Package Creation

```bash
# Create package from source
conan create . --build=missing

# Test package
conan test test_package libsssp/1.0.0@

# Export to local cache
conan export . libsssp/1.0.0@
```

### Package Options

Available Conan options:

```python
shared=False          # Build shared library
fPIC=True             # Position-independent code
enable_lto=False      # Link-time optimization
enable_tests=False    # Build tests
enable_benchmarks=False
enable_examples=False
```

Example usage:

```bash
conan create . --build=missing \
  -o shared=True \
  -o enable_lto=True \
  -o enable_tests=True
```

### Publishing to Conan Center

1. Fork [conan-center-index](https://github.com/conan-io/conan-center-index)
2. Add recipe under `recipes/libsssp/`
3. Test the recipe:
   ```bash
   conan create recipes/libsssp/all/ --build=missing
   ```
4. Submit pull request

### Publishing to Artifactory

```bash
# Upload to private Artifactory
conan upload libsssp/1.0.0@ -r=my-artifactory --all
```

---

## vcpkg Packages

### Local Package Testing

```bash
# From vcpkg directory
./vcpkg install libsssp --overlay-ports=/path/to/subquadratic-shortest-paths

# Test the package
./vcpkg remove libsssp
./vcpkg install libsssp
```

### Package Files

Required files for vcpkg:
- `vcpkg.json` - Package manifest
- `portfile.cmake` - Build instructions
- `usage` - Integration instructions

### Publishing to vcpkg Registry

1. Fork [vcpkg registry](https://github.com/microsoft/vcpkg)
2. Create port directory: `ports/libsssp/`
3. Add required files:
   - `vcpkg.json`
   - `portfile.cmake`
   - `usage`
4. Test locally:
   ```bash
   ./vcpkg install libsssp
   ./vcpkg x-ci-verify-versions --verbose
   ```
5. Submit pull request

### Version Management

Update `versions/baseline.json` and `versions/l-/libsssp.json`:

```json
{
  "versions": [
    {
      "version": "1.0.0",
      "git-tree": "abcdef123456..."
    }
  ]
}
```

---

## Distribution-Specific Packages

### Debian/Ubuntu (.deb)

#### Manual Creation

```bash
mkdir -p debian/DEBIAN
cat > debian/DEBIAN/control << EOF
Package: libsssp
Version: 1.0.0
Architecture: amd64
Maintainer: SSSP Team <dev@example.com>
Description: Breakthrough O(m log^{2/3} n) SSSP Algorithm
 High-performance implementation of single-source shortest paths
EOF

# Install files
cmake --install build --prefix debian/usr

# Create package
dpkg-deb --build debian libsssp_1.0.0_amd64.deb
```

#### Using CPack

```bash
cd build
cpack -G DEB
sudo dpkg -i libsssp-1.0.0-Linux-x64.deb
```

#### Installing

```bash
sudo dpkg -i libsssp_1.0.0_amd64.deb
sudo apt-get install -f  # Fix dependencies if needed
```

### Fedora/RHEL/CentOS (.rpm)

#### Using CPack

```bash
cd build
cpack -G RPM
sudo rpm -i libsssp-1.0.0-Linux-x64.rpm
```

#### Using rpmbuild

Create `libsssp.spec`:

```spec
Name:           libsssp
Version:        1.0.0
Release:        1%{?dist}
Summary:        Breakthrough SSSP algorithm

License:        MIT
URL:            https://github.com/your-org/subquadratic-shortest-paths
Source0:        libsssp-1.0.0.tar.gz

BuildRequires:  cmake >= 3.15
BuildRequires:  gcc

%description
High-performance O(m log^{2/3} n) single-source shortest paths algorithm.

%prep
%autosetup

%build
%cmake
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%doc README.md
%{_libdir}/libsssp.so.*
%{_includedir}/sssp.h

%changelog
* Mon Jan 01 2024 Developer <dev@example.com> - 1.0.0-1
- Initial package
```

Build:

```bash
rpmbuild -ba libsssp.spec
```

### Arch Linux (PKGBUILD)

Create `PKGBUILD`:

```bash
pkgname=libsssp
pkgver=1.0.0
pkgrel=1
pkgdesc="Breakthrough O(m log^{2/3} n) SSSP algorithm"
arch=('x86_64' 'aarch64')
url="https://github.com/your-org/subquadratic-shortest-paths"
license=('MIT')
depends=('glibc')
makedepends=('cmake' 'ninja')
source=("${pkgname}-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cmake -B build -S "${pkgname}-${pkgver}" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

check() {
    cd build
    ctest --output-on-failure
}

package() {
    DESTDIR="$pkgdir" cmake --install build
    install -Dm644 "${pkgname}-${pkgver}/LICENSE" \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
```

Build:

```bash
makepkg -si
```

### Homebrew Formula (macOS)

Create `libsssp.rb`:

```ruby
class Libsssp < Formula
  desc "Breakthrough O(m log^{2/3} n) SSSP algorithm"
  homepage "https://github.com/your-org/subquadratic-shortest-paths"
  url "https://github.com/your-org/subquadratic-shortest-paths/archive/v1.0.0.tar.gz"
  sha256 "abcdef..."
  license "MIT"

  depends_on "cmake" => :build
  depends_on "ninja" => :build

  def install
    system "cmake", "-S", ".", "-B", "build",
                    "-G", "Ninja",
                    *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    (testpath/"test.c").write <<~EOS
      #include <sssp.h>
      int main() {
        sssp_graph_t* g = sssp_graph_create(5);
        sssp_graph_destroy(g);
        return 0;
      }
    EOS
    system ENV.cc, "test.c", "-L#{lib}", "-lsssp", "-o", "test"
    system "./test"
  end
end
```

### Windows Chocolatey

Create `libsssp.nuspec`:

```xml
<?xml version="1.0"?>
<package>
  <metadata>
    <id>libsssp</id>
    <version>1.0.0</version>
    <title>libsssp</title>
    <authors>SSSP Development Team</authors>
    <description>
      Breakthrough O(m log^{2/3} n) single-source shortest paths algorithm
    </description>
    <projectUrl>https://github.com/your-org/subquadratic-shortest-paths</projectUrl>
    <tags>graph algorithms sssp shortest-path</tags>
    <licenseUrl>https://github.com/your-org/subquadratic-shortest-paths/blob/main/LICENSE</licenseUrl>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
  </metadata>
  <files>
    <file src="tools\**" target="tools" />
  </files>
</package>
```

---

## Publishing

### GitHub Releases

```bash
# Create release artifacts
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cd build
cpack -G TGZ
cpack -G ZIP

# Upload to GitHub Releases via web UI or gh CLI
gh release create v1.0.0 \
  libsssp-1.0.0-*.tar.gz \
  libsssp-1.0.0-*.zip \
  --title "libsssp v1.0.0" \
  --notes "See CHANGELOG.md"
```

### Docker Hub

```bash
# Build multi-platform image
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --tag yourorg/libsssp:1.0.0 \
  --tag yourorg/libsssp:latest \
  --push \
  -f docker/Dockerfile.ubuntu .
```

### npm (for Emscripten builds)

```bash
# Build WebAssembly version
emcmake cmake -B build-wasm -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm

# Create npm package
npm publish
```

---

## Versioning

### Semantic Versioning

Follow [SemVer](https://semver.org/):
- MAJOR.MINOR.PATCH (e.g., 1.0.0)
- Increment MAJOR for incompatible API changes
- Increment MINOR for backward-compatible features
- Increment PATCH for backward-compatible bug fixes

### Updating Version

1. Edit `VERSION` file:
   ```
   1.1.0
   ```

2. Update `vcpkg.json`:
   ```json
   {
     "version-string": "1.1.0"
   }
   ```

3. Update `conanfile.py` (version auto-read from VERSION file)

4. Tag release:
   ```bash
   git tag -a v1.1.0 -m "Release version 1.1.0"
   git push origin v1.1.0
   ```

---

## Continuous Integration

All packaging is automated via GitHub Actions (`.github/workflows/build-and-test.yml`):

- Builds on multiple platforms
- Runs comprehensive tests
- Creates packages for each platform
- Uploads to GitHub Releases on tags

---

## Troubleshooting

### CPack Issues

**Problem:** Package contains wrong files

**Solution:** Check install rules in CMakeLists.txt:
```cmake
install(TARGETS sssp_shared DESTINATION lib)
install(FILES include/sssp.h DESTINATION include)
```

### Conan Upload Fails

**Problem:** Authentication error

**Solution:** Login to remote:
```bash
conan remote login my-remote -u username -p password
```

### vcpkg Portfile Fails

**Problem:** SHA512 mismatch

**Solution:** Calculate correct hash:
```bash
vcpkg hash v1.0.0.tar.gz
```

---

For more information, see [BUILD.md](BUILD.md) or contact the maintainers.
