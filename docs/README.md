# VAPI documentation

**Russian documentation:** [docs/ru/](ru/) — копия основной документации на русском языке.

**VAPI** is a high-quality platform for building custom APIs while keeping full control over Vulkan (context, wrappers, tools, cross-API ABI, tests). It does not replace Vulkan but organizes its use and provides consistent contracts for extensions. Window and input are included; resize modes and frame cache support comfortable resizing; render context and pipelines handle drawing.

**Getting started:** [Quick start (quick_start.md)](quick_start.md) — build, first run, and minimal code on one page. [Usage examples (usage_examples.md)](usage_examples.md) — ready-made code for a window with text and an interactive app.

- **System install:** After building, run `cmake --install build`; then use `find_package(vapi_osp)` and targets `vapi_osp::vapi_*` in your project. See [integration.md](integration.md).
- **Version scheme:** [VERSIONING.md](VERSIONING.md) (0. = release, 0.0 = beta, 0.0.1 = alpha, b = patch). **Current API version:** 0.0.1-alpha (first alpha).
- **Running tests:** [testing.md](testing.md) (test levels, CI, headless). Tests are built when `VAPI_BUILD_TESTS=ON`.
- **Supported configurations:** [user_guide.md](user_guide.md) (platforms, requirements). Release: [RELEASING.md](RELEASING.md).

---

## Modules and purpose

| Module | Purpose |
|--------|---------|
| **vapi_core** | Types, errors (Result), files, logger, math, interfaces |
| **vapi_platform** | Window and input (IPlatform, createPlatform(PlatformKind)); implementation: GLFW |
| **vapi_gpu** | Vulkan: instance, device, surface, swapchain (GpuContext) |
| **vapi_render** | Shaders, pipelines, RenderContext |
| **vapi_resource** | Buffers, images, descriptors, staging |
| **vapi_font** | Fonts, layout, glyph atlas, FontDrawList (no GPU) |
| **vapi_font_render** | Ready-made “window + Vulkan + text” app (FontRenderApp) |
| **vapi_ui** | Widgets, layout, signals (no Vulkan) |

Scenario → targets and headers: [integration.md](integration.md#3-library-selection-by-scenario).

---

- **[user_guide.md](user_guide.md)** — **User guide**: install, build, run examples, quick start, main scenarios, common issues.
- **[full_documentation.md](full_documentation.md)** — **Full documentation with code examples**: quick start, including headers, Core, Platform, GPU, Render, Resource, Font, FontRenderApp, full app examples and links to detailed reference.
- **[architecture.md](architecture.md)** — Architecture overview: layers (core → platform, font, gpu → resource, render; FontRenderApp), typical scenarios (headless font, window with text, custom render), API call order, links to key headers and examples.
- **[integration.md](integration.md)** — Integration guide: requirements, add_subdirectory, library selection by scenario, minimal examples (FontRenderApp, headless font, custom render), shaders, build.
- **[scaling_and_changes.md](scaling_and_changes.md)** — Scaling and making changes: adding modules, examples and tests, new platform/GPU backends, extending font and render, API changes, build and checklist.
- **[shortcomings.md](shortcomings.md)** — Known limitations and narrow spots of the project.
- **[quick_reference.md](quick_reference.md)** — Quick table: main classes and methods, where to look in detail_docs.
- **[security_layer.ru.md](security_layer.ru.md)** — Security layer (Russian): critical areas, protection and cybersecurity (no sandbox like Chromium; recommendations for paths, shaders, buffers).
- **[testing.md](testing.md)** — How to run unit tests and full check (ctest).
- **[DOCUMENTATION_LANGUAGE.md](DOCUMENTATION_LANGUAGE.md)** — All docs and code comments are in English; exceptions and migration note.
- **[PROGRESS_AND_CHANGES.md](PROGRESS_AND_CHANGES.md)** — **Progress and changes**: detailed log of recent documentation and repository updates (`.gitignore`, version comparison).
- **[pipeline_cache.md](pipeline_cache.md)** — What is pipeline cache (Vulkan) and why it matters.
- **[render/](render/)** — Render module: learning guides (EN, RU, ZH) and README.
- **[detail_docs/](detail_docs/)** — **Detailed API reference**: each module and element with code examples (core, platform, gpu, render including pipeline, resource).

For a quick start, use `#include "vapi.hpp"` (root include). For render, add `#include "render/render.hpp"` and see **render/learning.md**. For per-element reference and pipeline usage, see **detail_docs/**.

Design: some interfaces are forward-looking (stub or no implementation yet) and may be refined later; render and resource modules are Vulkan-first — cross-API abstraction is not in current scope (see **shortcomings.md**, section "Abstraction level and design choices").
