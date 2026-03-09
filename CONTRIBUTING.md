# Contributing to VAPI OSP

Thank you for your interest in contributing! This document explains the process and conventions.

## Getting Started

1. Fork the repository at https://github.com/R3zeProjects/vapi_osp
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes following the conventions below
4. Run tests: `cmake -B build -DVAPI_BUILD_TESTS=ON && cmake --build build && ctest --test-dir build`
5. Open a pull request against `main`

## Code Style

- **C++ standard:** C++23 (`std::expected`, ranges, deduced this where applicable)
- **Error handling:** Use `Result<T>` (`std::expected<T, Error>`) and `VAPI_TRY`/`VAPI_TRY_ASSIGN` macros; no exceptions in hot paths
- **Naming:** `snake_case` for files, variables, and functions; `PascalCase` for classes and structs; `kConstantName` for compile-time constants
- **Headers:** `[[nodiscard]]` on all functions returning `Result<T>` or owning resources
- **Comments:** Explain *why*, not *what*; no obvious narration comments
- **No raw `new`/`delete`:** Use `std::unique_ptr`, `std::shared_ptr`, or handle pools

## Project Structure

```
include/   — public headers (API surface)
src/       — implementation files
tests/     — unit and integration tests (Google Test)
examples/  — standalone example programs
docs/      — documentation
```

## Adding a New Module

See [docs/scaling_and_changes.md](docs/scaling_and_changes.md) for the step-by-step guide: create library target in `CMakeLists.txt`, add `include/<module>/` and `src/<module>/`, link to existing modules, add tests.

## Tests

- All tests use **Google Test** (`EXPECT_*`/`ASSERT_*`)
- Tests that require a display or GPU must check for headless environment and call `GTEST_SKIP()` gracefully
- Add new tests to `tests/` and register them in `CMakeLists.txt` under `VAPI_BUILD_TESTS`

## Pull Request Checklist

- [ ] Code compiles with no warnings on at least one of: MSVC, GCC, Clang
- [ ] New public API is documented in the relevant `docs/detail_docs/` file
- [ ] Tests added or updated for changed behavior

## Reporting Issues

Open an issue at https://github.com/R3zeProjects/vapi_osp/issues. For security vulnerabilities, see [SECURITY.md](SECURITY.md).
