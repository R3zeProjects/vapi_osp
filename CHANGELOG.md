# Changelog

All notable changes to VAPI OSP are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).  
Project versioning follows [docs/VERSIONING.md](docs/VERSIONING.md) (0.0.1 = alpha, 0.0.1bN = patch, 0.1.0 = first stable).

---

## [Unreleased]

_(No changes yet.)_

---

## [0.0.2b3] — 2026-03-09

### Changed (API design)

- **Strong-typed resource IDs** — `BufferId`, `ImageId`, `SamplerId`, `DescLayoutId`, `DescSetId` changed from `using = u32` to `enum class : u32`. Compiler now prevents mixing ID types. Added `kNullXxxId`, `kInvalidXxxId` sentinels, `toIndex()` and `advanceId()` helpers, `std::hash` specializations for use in `unordered_map`.
- **IGpuBackend void\* getters** — `getDevice()`, `getInstance()`, `getSurface()`, `getSwapchain()` marked `@deprecated` in Doxygen; use typed helpers from `vulkan_backend_access.hpp` instead.

### Added (Build / CI)

- **Linux CI job** — Ubuntu 22.04 build+test job in `.github/workflows/ci.yml` (Ninja, GLFW, Vulkan SDK, ctest).
- **SOVERSION** — Shared libraries on Linux now set `VERSION`/`SOVERSION` properties for proper `.so` symlinks.
- **Symbol visibility** — `vapi_export.hpp`: `VAPI_API` uses `__attribute__((visibility("default")))` on GCC/Clang when building shared; `-fvisibility=hidden` added for shared targets.
- **RPATH** — `CMAKE_BUILD_RPATH_USE_ORIGIN` and `CMAKE_INSTALL_RPATH_USE_LINK_PATH` enabled for Linux executables.

### Documentation

- **Module dependency matrix** — Added to `docs/quick_reference.md`: which modules require Vulkan SDK (`gpu/`, `resource/`, `render/`) and which are Vulkan-free (`core/`, `platform/`, `font/`).
- **Linux quick start** — Added build instructions for Linux in `README.md`.
- **api_evaluation.md** — Updated recommendations table and summary with v0.0.2b3 improvements.

---

## [0.0.2b2] — 2026-03-09

### Fixed (Safety / Resource leaks)

- **RAII destructors** — `BufferManager`, `ImageManager`, `StagingManager` destructors now call `shutdown()`; copy constructors/assignment deleted to prevent double-free.
- **Double-init protection** — `BufferManager::init()`, `ImageManager::init()`, `StagingManager::init()` return `AlreadyInitialized` on repeated calls instead of silently leaking resources.
- **Null device guards** — `BufferManager::destroyBuffer/map/unmap/flush` and `ImageManager::destroyImage` check for null `m_device` after `shutdown()`.
- **Vulkan handle leaks** — `ensureGpuPrepared`: full cleanup of `descPool`, `setLayout`, shader modules on partial pipeline failure. Static `run()` path: `vertModule.destroy()` / `fragModule.destroy()` on partial shader load.
- **Abandoned batch cleanup** — `StagingManager::shutdown()` now submits/frees abandoned batch command buffers.
- **FontRenderApp::init error paths** — Added missing `bufferManager.shutdown()` and `imageManager.shutdown()` when staging or sampler cache init fails.

### Fixed (Correctness)

- **GlyphAtlas(0, 0)** — Constructor clamps atlas dimensions to minimum 1×1 (prevents division by zero in `getWhitePixelUVRect()`).
- **ensureGlyph atlas-full** — Returns `advanceX` from font instead of 0 when atlas is full (fixes broken text cursor positioning).
- **bmp.pixels check** — Changed from `!bmp.pixels` to `bmp.pixels.empty()` (was a compilation error on `std::vector`).
- **Empty data guard** — `uploadBufferBatch`, `uploadImageBatch`, `updateImageBatch` reject empty `data` (Vulkan spec: buffer size must be > 0).
- **Integer overflow** — Image size calculations cast to `size_t` before multiplying (`static_cast<size_t>(width) * height * 4u`).
- **Unpaired callbacks** — `FontRenderApp::run` now calls `onEndFrameCb` when `beginFrame()` fails (before `continue`/`break`).
- **Font state RAII** — `TextPainter` uses `FontHeightGuard` to guarantee pixel height restore in `paintFromLayout`, `drawText`, `appendText` even on exception.
- **BufferManager::write** — Added null data check (`!data && size > 0` → `InvalidArgument`).

### Changed (API design)

- **VkCommandManager** — Stores `VkDevice` internally (passed once in `create()`); removed `device` parameter from `destroy()`, `resetPool()`, `beginSingleTime()`, `endSingleTime()`, `endSingleTimeWithFence()`. Added `assert` bounds checking on `frameIndex` in all accessors.
- **IFontSource** — `getAdvance()` and `getKernAdvance()` are now `const` (in interface and both implementations: `StbFontSource`, `AsciiBezierFontSource`).
- **GpuBackendConfig::requiredExtensions** — Changed from `std::vector<const char*>` to `std::vector<std::string>` (ownership safety, no dangling pointers).
- **TextPainter default oversample** — Changed from 1 to 2 (aligned with `FontDrawList` default).
- **setExtraGlyphSpacingRatio** — Now symmetrically clears `m_extraGlyphSpacing` (matching `setExtraGlyphSpacing` clearing ratio).
- **ImageManager** — Added ID recycling via `m_freeIds` (matching `BufferManager` pattern).
- **BufferManager::write** — Added `std::span<const u8>` overload for consistency with `StagingManager`.

### Removed

- **Redundant overloads** — Removed `const std::string&` overloads of `TextPainter::drawText`, `paint`, `appendText` (`std::string_view` suffices via implicit conversion).

### Fixed (Build)

- **vulkan_ui_painter.cpp** — Replaced undefined `kNullImage`/`kNullBuffer` constants with `0`.

---

## [0.0.2b1] — 2026-03-09

### Added

- **Emitter snapshot-safe emit** — `Emitter::emit()` now snapshots the full reaction list before invoking callbacks, so disconnecting a reaction during emit no longer skips remaining reactions in the current dispatch.
- **FontRenderApp PIMPL fix** — Moved defaulted constructor to the `.cpp` file to resolve incomplete-type errors with `unique_ptr<Impl>` on compilers that check `sizeof` in the header.
- **ICpuFrameDriver::beginFrame() contract** — `runUILoop` now calls `beginFrame()` without arguments, matching the `ICpuFrameDriver` interface (time source is set separately via `setTimeSource()`).

### Removed

- **Debug instrumentation** — Removed leftover agent debug logging from `font_render_app.cpp` and `run_ui_loop.cpp` (hardcoded paths, `std::error_code` rvalue issues).

### Documentation

- **CREATOR_SIGNING.md** — Translated from Russian to English.
- **Internal files excluded** — Internal planning, evaluation, and process documents excluded from version control.

---

## [0.0.1b2] — 2025-03-09

### Added

- **runUILoop** — High-level UI run loop in `ui/run_ui_loop.hpp`. Single call runs the full cycle: platform events, Vulkan backend, `RenderContext`, `VulkanUiPainter`, widget tree drawing, and present until the window is closed. Applications no longer need to duplicate the platform + GPU + font + render loop. Options: `RunUILoopOptions` (shader dir, app name, font name/size, clear color). Target `vapi_ui_vulkan` now includes this and links `vapi_platform` and `vapi_gpu`.
- **Headless platform** — New backend `PlatformKind::Headless` and `HeadlessPlatform`. No real window; `createWindow()` returns `errors::NotSupported`, `anyWindowOpen()` is always false. Intended for CI and headless tests. Use `createPlatform(PlatformKind::Headless)`.
- **ListView widget** — New widget in `ui/list_view.hpp`: list of strings, single selection, scrollable. API: `setItems()`, `addItem()`, `clear()`, `selectedIndex()` / `setSelectedIndex()`, `selectedItem()`, and `selected()` emitter (emits index on click). Implemented with `ScrollArea` and row buttons.
- **IAssetLoader** — Async asset loading in `core/asset_loader.hpp`. Interface `IAssetLoader::loadAsync(path, callback)`; callback receives `Result<std::vector<u8>>` and may be invoked from a worker thread. `createSimpleAssetLoader()` provides a implementation that reads the file on a `std::thread` and invokes the callback.
- **Assistant callback backend** — `createCallbackBackend(AssistantSubmitFn)` in `ui/assistant_backend.hpp`. Allows plugging a custom submit function (e.g. HTTP request to an LLM API) without implementing a full `IAssistantBackend` subclass. Signature: `(prompt, systemContext, replyFn)`.

### Documentation

- **Pipeline cache** — Short note in `docs/integration.md` on optional use of `PipelineCache` for faster startup (load/save and pass to pipeline builder).
- **DLL / ABI** — New section in `docs/building_dll.md`: “ABI and toolchain (DLL)” — same compiler/CRT for app and all DLLs; recommend documenting the toolchain when distributing prebuilt DLLs.

---

## [0.0.1-alpha] — First alpha

First alpha release. Includes:

- **Core** — Types, errors, files, logger, math, interfaces (IWindow, IInput, IByteSource, etc.).
- **Platform** — GLFW backend; window, input, monitors.
- **GPU** — Vulkan: instance, device, surface, swapchain, sync, command recording, profiler, fences.
- **Render** — Shaders, pipeline cache, pipeline builder, `RenderContext` (dynamic rendering).
- **Resource** — Buffers, images, descriptors, samplers, staging.
- **Font** — STB TrueType, vector Bezier fonts, glyph atlas, text measurement, `FontDrawList`.
- **FontRenderApp** — High-level “window + Vulkan + text” app.
- **UI** — Signals (`Emitter`), 13+ widgets (Button, Label, LineEdit, ScrollArea, GroupBox, TabWidget, MathArea, Assistant, etc.), layouts (Box, Grid, Stack).
- **VulkanUiPainter** — `IPainter` implementation for widget tree over Vulkan + font.
- **CMake** — Install and `find_package(vapi_osp)`; optional shared libs (`VAPI_BUILD_SHARED_LIBS`); 59+ tests (GTest).

---

[Unreleased]: https://github.com/R3zeProjects/vapi_osp/compare/v0.0.2b3...HEAD
[0.0.2b3]: https://github.com/R3zeProjects/vapi_osp/compare/v0.0.2b2...v0.0.2b3
[0.0.2b2]: https://github.com/R3zeProjects/vapi_osp/compare/v0.0.2b1...v0.0.2b2
[0.0.2b1]: https://github.com/R3zeProjects/vapi_osp/compare/v0.0.1b2...v0.0.2b1
[0.0.1b2]: https://github.com/R3zeProjects/vapi_osp/compare/v0.0.1-alpha...v0.0.1b2
[0.0.1-alpha]: https://github.com/R3zeProjects/vapi_osp/releases/tag/v0.0.1-alpha
