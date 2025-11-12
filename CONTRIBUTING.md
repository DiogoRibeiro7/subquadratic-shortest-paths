# Contributing to SSSP Breakthrough Algorithm

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style Guidelines](#code-style-guidelines)
- [Testing Requirements](#testing-requirements)
- [Documentation Standards](#documentation-standards)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors, regardless of experience level, gender, gender identity and expression, sexual orientation, disability, personal appearance, body size, race, ethnicity, age, religion, or nationality.

### Expected Behavior

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

### Unacceptable Behavior

- Harassment, discriminatory comments, or personal attacks
- Trolling, insulting/derogatory comments, and political attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct which could reasonably be considered inappropriate

---

## Getting Started

### Prerequisites

- C99-compliant compiler (GCC 7+, Clang 9+, or MSVC 2019+)
- CMake 3.15+
- Git
- Basic understanding of graph algorithms

### Setting Up Development Environment

1. **Fork the repository** on GitHub

2. **Clone your fork:**
   ```bash
   git clone https://github.com/YOUR_USERNAME/subquadratic-shortest-paths.git
   cd subquadratic-shortest-paths
   ```

3. **Add upstream remote:**
   ```bash
   git remote add upstream https://github.com/original-org/subquadratic-shortest-paths.git
   ```

4. **Create development build:**
   ```bash
   mkdir build-dev && cd build-dev
   cmake -DCMAKE_BUILD_TYPE=Debug \
         -DSSSP_BUILD_TESTS=ON \
         -DSSSP_BUILD_EXAMPLES=ON \
         -DSSSP_ENABLE_ASAN=ON \
         -DSSSP_ENABLE_UBSAN=ON \
         ..
   make -j$(nproc)
   ```

5. **Run tests:**
   ```bash
   ctest --output-on-failure
   ```

---

## Development Workflow

### 1. Create a Feature Branch

```bash
git checkout develop
git pull upstream develop
git checkout -b feature/your-feature-name
```

Branch naming conventions:
- `feature/feature-name` - New features
- `bugfix/issue-description` - Bug fixes
- `docs/what-you-changed` - Documentation changes
- `refactor/component-name` - Code refactoring
- `test/what-youre-testing` - Test additions

### 2. Make Your Changes

- Write clean, well-documented code
- Follow the code style guidelines (see below)
- Add tests for new functionality
- Update documentation as needed

### 3. Commit Your Changes

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```bash
git add .
git commit -m "feat: add new graph generation algorithm"
```

Commit message format:
```
<type>(<scope>): <subject>

<body>

<footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Build system, CI, dependencies

Example:
```
feat(api): add path reconstruction function

Implement sssp_result_get_path() to reconstruct shortest paths
from source to any reachable vertex using predecessor array.

Closes #42
```

### 4. Push and Create Pull Request

```bash
git push origin feature/your-feature-name
```

Then create a pull request on GitHub.

---

## Code Style Guidelines

### C Code Style

We follow a consistent C99 style based on Linux kernel style with modifications:

#### Indentation
- **4 spaces** (no tabs)
- Indent case labels within switch statements

#### Naming Conventions

```c
// Functions: snake_case
void calculate_distance(int vertex);

// Types: snake_case with _t suffix
typedef struct graph graph_t;

// Constants: UPPER_SNAKE_CASE
#define MAX_VERTICES 100000

// Global variables: g_ prefix
static int g_vertex_count = 0;

// Public API: sssp_ prefix
sssp_graph_t* sssp_graph_create(int n);
```

#### Braces and Spacing

```c
// Opening brace on same line for functions
void function_name(int param) {
    // code
}

// Space after keywords
if (condition) {
    // code
} else {
    // code
}

// No space between function name and parenthesis
function_call(arg1, arg2);

// Spaces around operators
int result = a + b * c;
```

#### Comments

```c
/**
 * @brief One-line brief description
 *
 * Detailed description of function behavior, parameters,
 * and return value. Include complexity when relevant.
 *
 * @param n Number of vertices
 * @param source Source vertex (0 <= source < n)
 * @return Pointer to result, or NULL on error
 *
 * @complexity O(m log^(2/3) n)
 * @note This function modifies the graph structure
 */
sssp_result_t* sssp_solve(sssp_graph_t* graph, int source);

// Inline comments for complex logic
// Use // style, not /* */
int k = (int)floor(pow(log(n), 1.0/3.0));  // Frontier reduction factor
```

#### Error Handling

```c
// Always check return values
sssp_graph_t* graph = sssp_graph_create(n);
if (!graph) {
    return NULL;  // Or appropriate error code
}

// Use consistent error codes
if (vertex < 0 || vertex >= n) {
    return SSSP_ERROR_INVALID_VERTEX;
}
```

#### Memory Management

```c
// Free in reverse order of allocation
Edge* edge = malloc(sizeof(Edge));
if (!edge) {
    return NULL;
}

// Use edge...

free(edge);

// Always nullify pointers after freeing
graph->adj[i] = NULL;
```

### File Organization

```c
/* 1. Copyright and license header */
/**
 * @file filename.c
 * @brief Brief description
 */

/* 2. Includes */
#include "public_headers.h"
#include <system_headers.h>

/* 3. Defines and macros */
#define MAX_SIZE 1000

/* 4. Type definitions */
typedef struct { ... } my_type_t;

/* 5. Static function declarations */
static void helper_function(void);

/* 6. Global variables (minimize) */
static int g_global_var = 0;

/* 7. Public function implementations */

/* 8. Static function implementations */
```

### Code Formatting

We use `clang-format` for consistent formatting:

```bash
# Format all C files
find src include -name "*.c" -o -name "*.h" | xargs clang-format -i
```

Configuration is in `.clang-format`:
```yaml
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
```

---

## Testing Requirements

### Test Coverage

All new features must include tests:

- **Unit tests**: Test individual functions
- **Integration tests**: Test component interactions
- **Correctness tests**: Verify algorithm correctness
- **Performance tests**: Ensure no regression

### Writing Tests

Tests are in `tests/` directory:

```c
// tests/test_feature.c
#include "../include/sssp.h"
#include <assert.h>

void test_feature_basic(void) {
    // Setup
    sssp_graph_t* g = sssp_graph_create(10);

    // Exercise
    sssp_result_t* result = sssp_solve(g, 0);

    // Verify
    assert(result != NULL);
    assert(sssp_result_get_status(result) == SSSP_SUCCESS);

    // Cleanup
    sssp_result_destroy(result);
    sssp_graph_destroy(g);
}

int main(void) {
    test_feature_basic();
    printf("All tests passed!\n");
    return 0;
}
```

### Running Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test
ctest -R APITests --verbose

# Run with sanitizers
cmake -DSSSP_ENABLE_ASAN=ON ..
make
ctest
```

### Performance Testing

```bash
# Run performance benchmarks
./bin/examples/performance_test

# Compare with baseline
./scripts/benchmark_comparison.sh
```

---

## Documentation Standards

### Code Documentation

All public functions must have Doxygen documentation:

```c
/**
 * @brief Brief one-line description
 *
 * Detailed description of what the function does,
 * including any important behavior, side effects,
 * or limitations.
 *
 * @param graph Pointer to graph structure (must not be NULL)
 * @param source Source vertex ID (0 <= source < num_vertices)
 * @return Pointer to result structure, or NULL on error
 *
 * @complexity Time: O(m log^(2/3) n), Space: O(n + m)
 * @see sssp_result_destroy(), sssp_result_get_distance()
 *
 * @example
 * @code
 * sssp_graph_t* g = sssp_graph_create(100);
 * sssp_result_t* r = sssp_solve(g, 0);
 * @endcode
 */
sssp_result_t* sssp_solve(sssp_graph_t* graph, int source);
```

### Markdown Documentation

- Use clear, concise language
- Include code examples that work
- Add diagrams for complex concepts
- Keep line length under 100 characters
- Use proper markdown formatting

### Updating Documentation

When making changes, update:

1. **Inline code comments** - Keep up-to-date with code
2. **API Reference** - Update function signatures and descriptions
3. **README** - Update if adding major features
4. **CHANGELOG** - Add entry under [Unreleased]
5. **Examples** - Add new examples for new features

---

## Pull Request Process

### Before Submitting

**Checklist:**
- [ ] Code follows style guidelines
- [ ] All tests pass locally
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Commit messages follow conventions
- [ ] No merge conflicts with develop branch

### Creating the Pull Request

1. **Push your branch:**
   ```bash
   git push origin feature/your-feature
   ```

2. **Create PR on GitHub:**
   - Target the `develop` branch
   - Use a clear, descriptive title
   - Fill out the PR template completely

3. **PR Template:**
   ```markdown
   ## Description
   Brief description of changes

   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation update

   ## Testing
   - [ ] All tests pass
   - [ ] New tests added
   - [ ] Manual testing completed

   ## Checklist
   - [ ] Code follows style guidelines
   - [ ] Documentation updated
   - [ ] CHANGELOG.md updated
   ```

### Review Process

1. **Automated checks** must pass (CI/CD)
2. **At least one maintainer approval** required
3. **Address review comments** promptly
4. **Update PR** as needed

### After Approval

- Maintainers will merge your PR
- Delete your feature branch
- Sync your fork with upstream

---

## Issue Reporting

### Before Creating an Issue

1. **Search existing issues** - avoid duplicates
2. **Check documentation** - may already be answered
3. **Test with latest version** - issue may be fixed

### Creating a Good Issue

**Bug Report Template:**
```markdown
## Description
Clear description of the bug

## To Reproduce
Steps to reproduce:
1. Create graph with...
2. Add edges...
3. Run sssp_solve...
4. See error

## Expected Behavior
What you expected to happen

## Actual Behavior
What actually happened

## Environment
- OS: Ubuntu 22.04
- Compiler: GCC 11.3
- Version: 1.0.0

## Additional Context
- Error messages
- Stack traces
- Minimal reproducible example
```

**Feature Request Template:**
```markdown
## Feature Description
Clear description of proposed feature

## Use Case
Why is this feature needed?

## Proposed Solution
How should it work?

## Alternatives Considered
Other approaches you considered

## Additional Context
Any other relevant information
```

---

## Development Tips

### Debugging

```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Use GDB
gdb ./bin/test_api
(gdb) run
(gdb) bt  # backtrace on crash

# Use Valgrind for memory issues
valgrind --leak-check=full ./bin/test_api
```

### Performance Profiling

```bash
# Using gprof
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-pg" ..
make
./bin/performance_test
gprof ./bin/performance_test gmon.out > analysis.txt

# Using perf (Linux)
perf record ./bin/performance_test
perf report
```

### Code Analysis

```bash
# Static analysis with clang-tidy
clang-tidy src/*.c -- -I./include

# Cppcheck
cppcheck --enable=all --inconclusive src/
```

---

## Getting Help

- **Documentation**: Read [DOCUMENTATION.md](DOCUMENTATION.md)
- **Examples**: Check `examples/` directory
- **Discussions**: GitHub Discussions tab
- **Chat**: Join our Discord/Slack (if available)
- **Email**: maintainers@example.com (for sensitive issues)

---

## Recognition

Contributors will be:
- Listed in AUTHORS file
- Credited in release notes
- Mentioned in CHANGELOG.md

Thank you for contributing to advancing graph algorithms research!

---

**Last Updated**: 2025-11-12
**Maintained by**: SSSP Breakthrough Project Contributors
