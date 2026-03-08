# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html) for public API from 1.0.0 onward. Before 1.0.0, incompatible changes may occur without a major version bump.

**Versioning scheme:** `0.` = released, `0.0` = beta, `0.0.1` = alpha, `b` = patch (e.g. 0.0.1b1). See [docs/VERSIONING.md](docs/VERSIONING.md). **Current API version:** 0.0.1 (beta).

## [Unreleased]

### Added
- CMake install and package config: `find_package(vapi_osp)`, namespaced targets `vapi_osp::vapi_*`.
- Build options: `VAPI_BUILD_EXAMPLES`, `VAPI_BUILD_TESTS` (default ON).
- Install of font shader sources to `<prefix>/share/vapi_osp/shaders/`.
- Persistent GPU resources in `FontRenderApp` interactive loop: atlas image and vertex buffer are reused and only reallocated when size/capacity grows; in-place updates via `updateImage`/`uploadBuffer` when content changes.
- `formatError(Error, string_view context)` in `core/error.hpp` for logging with call-site context.
- Typed Vulkan accessors in `gpu/vulkan_backend_access.hpp`: `getVulkanInstance`, `getVulkanDevice`, `getVulkanSurface`, `getVulkanSwapchain` when backend type is Vulkan.
- `FontRenderApp::init()` and window creation now propagate concrete errors instead of generic `errors::Init`/`CreateWindow` where available.
- `VulkanUiPainter` in `ui/vulkan_ui_painter.hpp` / `src/ui/vulkan_ui_painter.cpp`: concrete `IPainter` implementation backed by `vapi_font` + Vulkan dynamic rendering. Draws colored rectangles via `drawRect`, renders UTF-8 text via `drawText`/`drawTextInRect`, integrates with `FontDrawList` and uploads atlas+vertices each frame.
- `CONTRIBUTING.md`: contribution guidelines, coding conventions, PR process.
- `SECURITY.md`: security policy, reporting instructions, known-safe configurations.
- `docs/detail_docs/ui.md`: full API reference for the UI module (Widget, Layout, signals, all widget types).
- `docs/detail_docs/font_render.md`: full API reference for `FontRenderApp` and `FontDrawList`.
- Google Test (GTest) as test framework via `FetchContent`; all 59+ test executables migrated from raw `assert()` to `EXPECT_*`/`ASSERT_*`.
- Test for `VulkanUiPainter` (headless-safe; skips if no GPU).

### Changed
- CHANGELOG and docs: replaced placeholder `your-org` URLs with real `R3zeProjects` URLs.
- `include/core/interfaces/`: `IEncoder`, `ICompressor`, `ISerializable`, `IClonable` moved to `include/core/interfaces/future/` and excluded from `vapi.hpp` and `core/README.md` to avoid unimplemented API surface in the public interface.
- CI: Windows job now installs GLFW via `choco install glfw`; `test_glfw_cmake` subdirectory gated behind `VAPI_BUILD_TESTS`.
- `README.md`: added English summary section below the Russian content.

### Fixed
- `test_glfw_cmake` subdirectory was unconditionally included; now only added when `VAPI_BUILD_TESTS=ON`.
- Distribution model clarified: Windows-only binary distribution; source code is open.

## [0.1.0] - Initial state

- Core, platform (GLFW), GPU (Vulkan), render, resource, font, font_render_app, UI modules.
- `Result<T>`, `errorMessage()`, VAPI_TRY macros.
- `FontRenderApp`, `FontDrawList`, `fillAtlasRgba`, `reserveForText`, `GlyphAtlas::isFull()`.
- `createPlatform(PlatformKind)`.
- Quick start and integration documentation.

[Unreleased]: https://github.com/R3zeProjects/vapi_osp/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/R3zeProjects/vapi_osp/releases/tag/v0.1.0
