# Release process

- **Version:** Set in `CMakeLists.txt` (`project(vapi_osp VERSION x.y.z)`). Update on release. **Current API version:** 0.0.1-alpha (first alpha). Version scheme: [VERSIONING.md](VERSIONING.md).
- **Before 1.0.0:** Incompatible changes may occur without a major version bump.
- **From 1.0.0:** Follow [Semantic Versioning](https://semver.org/): major = incompatible API changes, minor = new backward-compatible functionality, patch = fixes.

## Release steps

1. Update version in `CMakeLists.txt`.
2. Build and run tests: `cmake -B build && cmake --build build && ctest --test-dir build`.
3. Create tag (e.g. `v0.0.1-alpha`) and push, or create a Release in the repository.
4. If distributing via package managers, update their config (vcpkg, Conan, etc.) when applicable.

## Supported configurations

Official distribution platform is **Windows** (MSVC, MinGW). Binaries are published for Windows. See [user_guide.md](user_guide.md) for details.
