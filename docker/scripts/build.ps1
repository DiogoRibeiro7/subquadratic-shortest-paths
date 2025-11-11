# PowerShell build script for Windows Docker container

param(
    [string]$BuildType = "Release",
    [string]$Generator = "Ninja",
    [string]$InstallPrefix = "C:\install",
    [switch]$RunTests = $true
)

$ErrorActionPreference = "Stop"

Write-Host "==================================================" -ForegroundColor Blue
Write-Host "  libsssp Windows Build Script" -ForegroundColor Blue
Write-Host "==================================================" -ForegroundColor Blue
Write-Host "Build type:     $BuildType"
Write-Host "Generator:      $Generator"
Write-Host "Install prefix: $InstallPrefix"
Write-Host "Run tests:      $RunTests"
Write-Host "==================================================" -ForegroundColor Blue
Write-Host ""

# Set up Visual Studio environment
$vsPath = &"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" `
    -latest -property installationPath

if ($vsPath) {
    $vcvarsPath = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
    if (Test-Path $vcvarsPath) {
        Write-Host "Setting up Visual Studio environment..." -ForegroundColor Green
        cmd /c "`"$vcvarsPath`" && set" | ForEach-Object {
            if ($_ -match "^(.*?)=(.*)$") {
                Set-Content "env:\$($matches[1])" $matches[2]
            }
        }
    }
}

# Project directories
$projectDir = "C:\workspace\libsssp"
$buildDir = Join-Path $projectDir "build"
$sourceDir = $projectDir

# Clean previous build
if (Test-Path $buildDir) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Set-Location $buildDir

# Configure
Write-Host ""
Write-Host "Configuring CMake..." -ForegroundColor Green

$cmakeArgs = @(
    "-G", $Generator,
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_INSTALL_PREFIX=$InstallPrefix",
    "-DSSSP_BUILD_SHARED=ON",
    "-DSSSP_BUILD_STATIC=ON",
    "-DSSSP_BUILD_TESTS=$($RunTests.ToString())",
    "-DSSSP_BUILD_EXAMPLES=ON",
    $sourceDir
)

& cmake $cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build
Write-Host ""
Write-Host "Building project..." -ForegroundColor Green
& cmake --build . --config $BuildType --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Build succeeded!" -ForegroundColor Green

# Run tests
if ($RunTests) {
    Write-Host ""
    Write-Host "Running tests..." -ForegroundColor Green

    & ctest -C $BuildType --output-on-failure --parallel 4

    if ($LASTEXITCODE -ne 0) {
        Write-Host "Tests failed!" -ForegroundColor Red
        exit 1
    }

    Write-Host "All tests passed!" -ForegroundColor Green
}

# Install
Write-Host ""
Write-Host "Installing to $InstallPrefix..." -ForegroundColor Green
& cmake --install . --config $BuildType

if ($LASTEXITCODE -ne 0) {
    Write-Host "Installation failed!" -ForegroundColor Red
    exit 1
}

# Verify installation
Write-Host ""
Write-Host "Verifying installation..." -ForegroundColor Green

$headerPath = Join-Path $InstallPrefix "include\sssp.h"
$dllPath = Join-Path $InstallPrefix "bin\sssp.dll"
$libPath = Join-Path $InstallPrefix "lib\sssp.lib"

if (Test-Path $headerPath) {
    Write-Host "✓ Headers installed" -ForegroundColor Green
} else {
    Write-Host "✗ Headers missing" -ForegroundColor Red
    exit 1
}

if ((Test-Path $dllPath) -or (Test-Path $libPath)) {
    Write-Host "✓ Libraries installed" -ForegroundColor Green
} else {
    Write-Host "✗ Libraries missing" -ForegroundColor Red
    exit 1
}

# Summary
Write-Host ""
Write-Host "==================================================" -ForegroundColor Blue
Write-Host "Build Summary" -ForegroundColor Green
Write-Host "==================================================" -ForegroundColor Blue
Write-Host "Build type:     $BuildType"
Write-Host "Generator:      $Generator"
if ($RunTests) {
    Write-Host "Test results:   PASSED" -ForegroundColor Green
}
Write-Host "Install prefix: $InstallPrefix"
Write-Host "==================================================" -ForegroundColor Blue
Write-Host "Build completed successfully!" -ForegroundColor Green
