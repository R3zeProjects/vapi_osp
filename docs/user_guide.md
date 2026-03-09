# VAPI user guide

This guide describes how to install VAPI, build the project, run examples, and start using the platform in your applications.

---

## 1. About the platform

**VAPI** is a platform for building custom APIs on top of Vulkan while keeping control over context, wrappers, and tools. It does not replace Vulkan but organizes its use and provides consistent contracts.

**Main features:**

- **Window and input** — via GLFW (window creation, events, polling).
- **Vulkan** — instance, device, surface, swapchain, sync, commands.
- **Resources** — buffers, images, descriptors, samplers, staging upload.
- **Render** — shaders, pipelines (graphics/compute), frame context and dynamic rendering.
- **Fonts** — font loading (TTF/OTF), text measurement and layout, glyph atlas, text rendering in a window or headless (no GPU).

**Typical scenarios:**

1. **Window with text** — ready-made `FontRenderApp`: minimal code, window + Vulkan + font + draw loop.
2. **Fonts only (headless)** — measure and layout text, get geometry (vertices + atlas) without window and Vulkan.
3. **Custom render** — full control: your window, `GpuContext`, `RenderContext`, resources, your pipelines and geometry (including from `FontDrawList` or `TextPainter`).

See [architecture.md](architecture.md) for layers and architecture.

---

## 2. Requirements

- **C++23 compiler** (MSVC or MinGW/GCC on Windows).
- **CMake** 3.14 or newer.
- **Vulkan SDK** — Vulkan headers and libraries; in PATH or set `Vulkan_ROOT` at configure time.
- **GLFW 3.x** — for window and input. Either installed and found via `find_package(glfw3)`, or set `GLFW3_ROOT`, or CMake may fetch GLFW via FetchContent (see root `CMakeLists.txt`).
- **glslangValidator** (from Vulkan SDK) — for compiling GLSL shaders to SPIR-V; required for examples (calculator, counter, stopwatch). Usually in `VULKAN_SDK/Bin` or `bin`.

**Supported platforms:**

| Platform | Notes |
|----------|-------|
| Windows | MSVC, MinGW; Vulkan SDK and GLFW in PATH or via `GLFW3_ROOT`, `Vulkan_ROOT`. |

The library is distributed as compiled binaries for **Windows**. Building from source is supported on Windows.

**Public API:** headers in `include/`. Before version 1.0 contract stability is not guaranteed; incompatible changes may occur.

---

## 3. Building the project

### 3.1. Configure and build

From the repository root:

```
cmake -B build -S .
cmake --build build
```

Set dependency paths if needed:

```
cmake -B build -S . -DGLFW3_ROOT=C:/GLFW -DVulkan_ROOT=C:/VulkanSDK/1.4.321.1
```

### 3.2. What gets built

- **Libraries:** `vapi_core`, `vapi_platform`, `vapi_gpu`, `vapi_render`, `vapi_resource`, `vapi_font`, `vapi_font_render`, `vapi_ui` (and others per `CMakeLists.txt`).
- **Executables:**
  - `vapi_main` — main app (supports `--headless` for CI).
  - `calculator_example` — calculator (FontRenderApp + IPlatform).
  - `counter_example` — counter (single button).
  - `stopwatch_example` — stopwatch (Start/Stop/Reset).
- **Tests** — many unit and integration tests (see [testing.md](testing.md)).

Shaders for examples are compiled to SPIR-V into the build directory (`examples/shaders`) if `glslangValidator` is found.

---

## 4. Quick start

### 4.1. Run the “window with text” example

After building:

```
build\Debug\calculator_example.exe
```

The calculator window should open. Example code is in [examples/](../examples/) (calculator_example.cpp, counter_example.cpp, stopwatch_example.cpp).

### 4.2. Minimal code with FontRenderApp

To show text in a window with minimal code in your project:

1. Include the headers and link with `vapi_font_render` (and its dependencies per CMake).
2. Create the app, init, set font and size, add lines, run the loop:

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"

int main() {
    vapi::FontRenderApp app;
    auto initRes = app.init(800, 600, "My window", "/path/to/shaders");
    if (!initRes) {
        // handle error: vapi::errorMessage(initRes.error())
        return 1;
    }

    app.setFont("Arial");   // font name or path to .ttf/.otf
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "Hello, world!", vapi::color4(1.f, 1.f, 1.f, 1.f));

    return app.run();  // blocking loop until window close
}
```

Call order: **init** → optionally **setFont** / **setFontSize** → **addText** (0 or more times) → **run**. See [architecture.md](architecture.md), section “Window with text (FontRenderApp)”.

### 4.3. Using VAPI in your project

- **Root include:** `#include "vapi.hpp"` — core, platform, gpu.
- **Render:** `#include "render/render.hpp"` — shaders, pipelines, `RenderContext`.
- **Resources:** `#include "resource/resource.hpp"` — buffers, images, descriptors, staging.
- **Window with text:** `#include "render/font_render_app.hpp"` — class `FontRenderApp`.

Header paths are relative to the project `include/` directory. In CMake add `include` to `target_include_directories` and the needed targets to `target_link_libraries` (e.g. `vapi_font_render` for FontRenderApp).

---

## 5. Main usage scenarios

### 5.1. Window with text (FontRenderApp)

| Step | Action |
|------|--------|
| 1 | `app.init(width, height, title, shaderDir)` — platform, window, GPU, render. |
| 2 | Optionally `app.setFont(nameOrPath)`, `app.setFontSize(pixelHeight)`. |
| 3 | `app.addText(x, y, text, color)` — one or more lines. |
| 4 | `app.run()` — loop until window close; returns exit code (e.g. `kRunSuccess` / `kRunError`). |

Header: [render/font_render_app.hpp](../include/render/font_render_app.hpp). Examples: [calculator_example.cpp](../examples/calculator_example.cpp), [counter_example.cpp](../examples/counter_example.cpp), [stopwatch_example.cpp](../examples/stopwatch_example.cpp).

### 5.2. Fonts only (no window and Vulkan)

- **TextPainter** — measure and layout text, get geometry (vertices + atlas): `measureText`, `layoutText`, `drawText` / `paint` → `TextGeometry`.
- **FontDrawList** — draw list: `setFont` → `addText` (and optionally `addImage`) → `getVertices()`, `fillAtlasRgba(out)` or `getAtlasRgba()`; one atlas and one draw call; `reserveForText(text)` reserves vertices.

Headers: [font/text_painter.hpp](../include/font/text_painter.hpp), [font/font_draw_list.hpp](../include/font/font_draw_list.hpp). UI text docs: [font_ui_text.md](font_ui_text.md).

### 5.3. Custom render (full control)

Typical order:

1. Platform and window: `createPlatform(PlatformKind::Glfw)` (or `createPlatform()` default) → `init()` → `createWindow(config)` → if needed `getNativeHandle(winId)`.
2. GPU: `GpuContext::init(config)` (config: window handle, swapchain extent, platform extensions).
3. Render: `RenderContext::init(device, swapchain, config)`.
4. Resources: init BufferManager, ImageManager, DescriptorManager, StagingManager as needed; create buffers/textures, upload vertices and atlas from `FontDrawList` (`getVertices()`, `getAtlasRgba()`).
5. Frame loop: `beginFrame()` → on success `beginRendering(frame, clearColor)` → viewport, bind pipeline/descriptors → draw → `endRendering(frame)` → `endFrame(frame)`; on resize — `recreateSwapchain` and `handleResize`.

Details and loop example: [detail_docs/render.md](detail_docs/render.md), [detail_docs/resource.md](detail_docs/resource.md), [detail_docs/gpu.md](detail_docs/gpu.md).

---

## 6. Errors and checking results

Many VAPI functions return `Result<T>`. Checking:

- `if (!result)` — error.
- `result.error()` — error code.
- `vapi::errorMessage(result.error())` — text description (for logs and user output).

See [calculator_example.cpp](../examples/calculator_example.cpp) after `init` and `setFont`.

---

## 7. Testing

Full test run from project root:

```
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

Run a single test:

```
ctest --test-dir build -R test_gpu_tools --output-on-failure
```

Some tests require a display (window); in a headless environment they may be skipped. See [testing.md](testing.md).

---

## 8. Where to find details

| Topic | Document / location |
|-------|---------------------|
| Architecture, layers, scenarios | [architecture.md](architecture.md) |
| Quick table of classes and methods | [quick_reference.md](quick_reference.md) |
| Detailed API (core, platform, gpu, render, resource) | [detail_docs/](detail_docs/) |
| Limitations and narrow spots | [shortcomings.md](shortcomings.md) |
| Security (paths, shaders, buffers) | [security_layer.ru.md](security_layer.ru.md) |
| Pipeline cache (Vulkan) | [pipeline_cache.md](pipeline_cache.md) |
| Documentation index | [README.md](README.md) |

---

## 9. Common issues

- **“init failed” / window not created** — ensure GLFW and Vulkan SDK are installed and available; set `-DGLFW3_ROOT` and `-DVulkan_ROOT` at configure if libraries are not in PATH.
- **Shaders not found** — `FontRenderApp` needs a directory with compiled SPIR-V (e.g. `font_quad.vert.spv`, `font_quad.frag.spv`). Examples pass the build directory (`SHADER_DIR`); in your app set the correct path to that directory or to your shaders.
- **setFont does not find font** — pass a full path to `.ttf`/`.otf` or a font name (system path search).
- **Tests fail in CI without display** — some tests require a window; they are described in [testing.md](testing.md). Use `--headless` for `vapi_main` and exclude GUI tests in headless if needed.

---

## 10. Limitations (brief)

- One platform — GLFW; one graphics API — Vulkan.
- Render and resources are Vulkan-oriented (no D3D/Metal abstraction in current version).
- Some interfaces are stubs (e.g. some CPU/GPU tools) and may be refined later.

Full list: [shortcomings.md](shortcomings.md).

---

*Guide is current for VAPI API version 0.0.2b3 (version in `CMakeLists.txt`, docs in `docs/`). One-page quick start: [quick_start.md](quick_start.md).*
