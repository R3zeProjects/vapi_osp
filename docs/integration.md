# VAPI integration guide

This document describes how to integrate VAPI into your project: requirements, build, library selection, and minimal code for typical scenarios.

> **Platform:** VAPI OSP is distributed as binaries for **Windows**. Building from source is supported on Windows.

---

## 1. Requirements

| Component | Version / notes |
|-----------|------------------|
| **C++** | C++23 (project uses `std::expected` and is built with `CMAKE_CXX_STANDARD 23`) |
| **CMake** | 3.14+ |
| **Vulkan** | Vulkan SDK (headers and loader). Set `Vulkan_ROOT` or use standard `find_package(Vulkan)` |
| **GLFW** | 3.x (window and input). Optional if using only core/font without a window |

If needed, set paths at configure time:
- `-DGLFW3_ROOT=C:/path/to/glfw`
- `-DVulkan_ROOT=C:/VulkanSDK/1.4.321.1`

---

## 2. Integration methods

### 2.1. As a subproject (add_subdirectory)

Add the repository to your project (git submodule, copy, etc.) and in your root `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/vapi_osp)
```

All VAPI targets (`vapi_core`, `vapi_platform`, `vapi_gpu`, `vapi_render`, `vapi_resource`, `vapi_font`, `vapi_font_render`, `vapi_ui`) become available. Link only what you need (see section 3).

### 2.2. Install and find_package

After installing VAPI (`cmake --install build` or when packaging), in your project:

```cmake
find_package(vapi_osp 0.0.1 REQUIRED)  # current API version
# ...
target_link_libraries(your_app PRIVATE vapi_osp::vapi_font_render)
```

Ensure `vapi_osp_DIR` points to the directory containing `vapi_ospConfig.cmake` (usually `<prefix>/lib/cmake/vapi_osp`). Vulkan is pulled in automatically; GLFW must be available in your project (find_package(glfw3) or `GLFW3_ROOT`). Consumer targets use the namespace: `vapi_osp::vapi_core`, `vapi_osp::vapi_font_render`, etc.

Build options when building VAPI itself: `VAPI_BUILD_EXAMPLES` (default ON), `VAPI_BUILD_TESTS` (default ON), `VAPI_BUILD_SHARED_LIBS` (default OFF). For shared libraries (DLL on Windows) set `-DVAPI_BUILD_SHARED_LIBS=ON` or use the `shared` preset in CMakePresets. When using as subproject you can disable examples/tests: `-DVAPI_BUILD_EXAMPLES=OFF -DVAPI_BUILD_TESTS=OFF`.

### 2.3. Building and using as a full DLL package (Windows)

To produce a relocatable library package (DLL + import libs + headers):

1. **Configure and build** with shared libs and install to a prefix:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release -DVAPI_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:/vapi_osp_install -G "MinGW Makefiles" -S .
   cmake --build build
   cmake --install build
   ```

2. **Installed layout** (example for `C:/vapi_osp_install`):
   - `bin/` — DLLs (`libvapi_core.dll`, `libvapi_platform.dll`, …)
   - `lib/` — import libraries (`.lib` or `.dll.a`) and CMake config (`cmake/vapi_osp/`)
   - `include/` — public headers

3. **Use in your project:** set `CMAKE_PREFIX_PATH` to the install prefix, then:
   ```cmake
   find_package(vapi_osp 0.0.1 REQUIRED)
   target_link_libraries(your_app PRIVATE vapi_osp::vapi_font_render)
   ```
   At runtime, either copy all required VAPI DLLs from `bin/` (and GLFW/Vulkan if needed) next to your `.exe`, or add the install `bin/` directory to `PATH`.

4. **ABI compatibility (DLL):** Prebuilt DLLs are built with a specific toolchain (e.g. MinGW GCC x64). Your application must be built with the same compiler and C++ runtime (same MinGW version, or same MSVC version) to avoid runtime crashes. When distributing DLLs, document the toolchain (e.g. “MinGW-w64 GCC 14.x x64”).

---

## 3. Library selection by scenario

Link only the targets you actually use. Headers are relative to the VAPI repo `include/` directory (added via `target_include_directories` when linking).

| Scenario | Targets for `target_link_libraries` | Main headers |
|----------|-------------------------------------|-------------|
| Core only (types, errors, files) | `vapi_core` | `vapi.hpp` or `core/types.hpp`, `core/error.hpp`, `core/file_utils.hpp` |
| Window and input (GLFW) | `vapi_platform` (pulls `vapi_core`, glfw) | `platform/platform.hpp` |
| Vulkan: instance, device, swapchain | `vapi_gpu` (pulls `vapi_core`, Vulkan::Vulkan, glfw) | `gpu/gpu.hpp`, `gpu/gpu_context.hpp` |
| Render: shaders, pipelines, frame context | `vapi_render` (pulls `vapi_gpu`) | `render/render.hpp`, `render/render_context.hpp` |
| Buffers, images, descriptors, staging | `vapi_resource` (pulls `vapi_gpu`) | `resource/resource.hpp` |
| Fonts, layout, atlas, FontDrawList (no GPU) | `vapi_font` (pulls `vapi_core`) | `font/text_painter.hpp`, `font/font_draw_list.hpp`, `font/font_types.hpp` |
| Ready-made “window + Vulkan + text” app | `vapi_font_render` (pulls render, resource, font, platform) | `render/font_render_app.hpp` |
| UI widgets (no Vulkan) | `vapi_ui` (pulls `vapi_core`) | `ui/ui.hpp`, `ui/widget.hpp`, `ui/button.hpp`, etc. |
| UI rendering via Vulkan | `vapi_ui_vulkan` (pulls ui, render, resource, font) | `ui/vulkan_ui_painter.hpp` |

Minimal “one window with text”: one target **`vapi_font_render`** and one header **`render/font_render_app.hpp`**.

For UI rendering via Vulkan use **`vapi_ui_vulkan`** (target) and **`ui/vulkan_ui_painter.hpp`** or **`ui/run_ui_loop.hpp`** for the full loop — see section 11 below.

**Pipeline cache (optional):** For faster startup, use `PipelineCache` (see [pipeline_cache.md](pipeline_cache.md)): load from file in init, pass `cache.handle()` to `GraphicsPipelineBuilder::build(device, layout, pipelineCache)`, and save on shutdown.

---

## 4. Minimal example: window with text (FontRenderApp)

Uses a single class `FontRenderApp`: init, font, add lines, run loop.

**Application code:**

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    auto initRes = app.init(800, 600, "My application", "/path/to/shaders");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    app.setFont("Arial");   // or path to .ttf/.otf
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "Hello, VAPI!", vapi::color4(1.f, 1.f, 1.f, 1.f));
    return app.run();
}
```

**CMake (your executable):**

```cmake
add_executable(my_app main.cpp)
target_include_directories(my_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/path/to/vapi_osp/include)
target_link_libraries(my_app PRIVATE vapi_font_render)
# Shader path: either pass to init() from build dir, or copy font_quad.vert.spv / font_quad.frag.spv
```

Important: `init(..., shaderDir)` expects a directory containing compiled `font_quad.vert.spv` and `font_quad.frag.spv`. You can get these from the repo (see section 5).

---

## 5. Shaders for FontRenderApp

The app expects two files in `shaderDir`:

- `font_quad.vert.spv`
- `font_quad.frag.spv`

GLSL sources are in the repo under `examples/shaders/` (e.g. `font_quad.vert`, `font_quad.frag`). When installing VAPI (`cmake --install build`) they are copied to `<prefix>/share/vapi_osp/shaders/`; consumers can compile them to SPIR-V in their own directory. To compile to SPIR-V:

```bash
glslangValidator -V font_quad.vert -o font_quad.vert.spv
glslangValidator -V font_quad.frag -o font_quad.frag.spv
```

In the VAPI build tree, if `glslangValidator` is available, target `font_shaders` and a directory with built `.spv` are created (e.g. `CMAKE_CURRENT_BINARY_DIR/examples/shaders`). Your example can depend on this target and pass that directory to `init(..., shaderDir)` via a macro, as in `calculator_example`:

```cmake
target_compile_definitions(my_app PRIVATE SHADER_DIR="${VAPI_FONT_SHADER_DIR}")
```

and in code:

```cpp
#ifdef SHADER_DIR
#define VAPI_SHADER_DIR SHADER_DIR
#else
#define VAPI_SHADER_DIR "."
#endif
app.init(800, 600, "Title", VAPI_SHADER_DIR);
```

---

## 6. Example: fonts only without window (headless)

If you only need to measure and layout text, get vertices and atlas (you handle drawing or export):

```cmake
add_executable(my_font_tool main.cpp)
target_include_directories(my_font_tool PRIVATE ${VAPI_INCLUDE_DIR})
target_link_libraries(my_font_tool PRIVATE vapi_font)
```

```cpp
#include "font/font_draw_list.hpp"
#include "font/font_types.hpp"
#include "font/font_service.hpp"

// Load font via FontService, get IFontSource*
vapi::FontService fonts;
auto idRes = fonts.loadFont("/path/to/font.ttf");  // or system font name
if (!idRes) { /* fallback or error */ }
vapi::IFontSource* fontSrc = fonts.getFont(*idRes);

// setFont(IFontSource*, pixelHeight) → addText → getVertices(), fillAtlasRgba()
vapi::FontDrawList list;
list.setFont(fontSrc, 24.f);  // font + pixel size (no separate setPixelHeight)
list.addText(0.f, 0.f, "Hello", vapi::color4(1, 1, 1, 1));
const auto& vertices = list.getVertices();
std::vector<vapi::u8> atlasRgba;
list.fillAtlasRgba(atlasRgba);  // reusable buffer — more efficient
// Then upload to your buffers/textures and draw
```

See [architecture.md](architecture.md) (“Headless font”), [font_ui_text.md](font_ui_text.md).

---

## 7. Example: custom render (own window + Vulkan + pipelines)

Full control: platform, `GpuContext`, `RenderContext`, resource managers, your pipeline. Link:

- `vapi_platform` — window and input
- `vapi_gpu` — Vulkan
- `vapi_render` — frame context, shaders, pipelines
- `vapi_resource` — buffers, images, staging, descriptors
- optionally `vapi_font` — for text geometry

Order: `createPlatform()` → window → `GpuContext::init(...)` → `RenderContext::init(...)` → init resources → loop `beginFrame` → `beginRendering` → draw → `endRendering` → `endFrame`; on resize — `recreateSwapchain` and `handleResize`. Details and loop example in [architecture.md](architecture.md) and [detail_docs/render.md](detail_docs/render.md).

---

## 8. Include paths

All public headers are under `include/`. `#include` paths are relative to `include/`:

- `#include "vapi.hpp"` — core + platform + gpu
- `#include "render/render.hpp"` — render
- `#include "render/font_render_app.hpp"` — FontRenderApp
- `#include "resource/resource.hpp"` — resources
- `#include "font/font_draw_list.hpp"` — text draw list

When using `add_subdirectory` and `target_link_libraries(your_target PRIVATE vapi_*)`, the `include/` path is added to your target automatically (via `target_include_directories(vapi_* PUBLIC ...)`). If you integrate VAPI manually, add the VAPI repo `include` directory to your target.

---

## 9. Build and verify (Windows)

From the repo root:

```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the text example (after building shaders):

```
build\Release\calculator_example.exe
```

Unit tests: see [testing.md](testing.md).

---

## 10. Quick reference: targets and headers

| What you need | Target | Include |
|---------------|--------|---------|
| All-in-one (window + Vulkan + text) | `vapi_font_render` | `render/font_render_app.hpp` |
| Fonts only (layout, vertices, atlas) | `vapi_font` | `font/font_draw_list.hpp`, `font/text_painter.hpp` |
| Window + input | `vapi_platform` | `platform/platform.hpp` |
| Vulkan (device, swapchain) | `vapi_gpu` | `gpu/gpu_context.hpp` |
| Render (frame, pipelines) | `vapi_render` | `render/render_context.hpp` |
| Buffers, textures, staging | `vapi_resource` | `resource/resource.hpp` |
| UI widgets (no GPU) | `vapi_ui` | `ui/ui.hpp`, `ui/widget.hpp` |
| UI rendering (Vulkan) | `vapi_ui_vulkan` | `ui/vulkan_ui_painter.hpp` |
| Types, errors, files | `vapi_core` | `vapi.hpp` or `core/error.hpp`, `core/types.hpp` |

See also: [architecture.md](architecture.md), [quick_reference.md](quick_reference.md), [detail_docs/](detail_docs/).

---

## 11. Example: UI widgets with Vulkan rendering (VulkanUiPainter)

Uses `vapi_ui` for the widget tree and `vapi_ui_vulkan` for drawing via Vulkan.

**Code:**

```cpp
#include "ui/ui.hpp"
#include "ui/vulkan_ui_painter.hpp"
#include "render/render_context.hpp"
#include "gpu/gpu_context.hpp"
#include "platform/platform.hpp"
#include "font/font_service.hpp"

int main() {
    // Init platform + Vulkan (see section 7 or architecture.md)
    auto platform = vapi::createPlatform();
    platform->init();
    // ... create window, GpuContext, RenderContext ...

    // Load font
    vapi::FontService fontSvc;
    auto fontId = fontSvc.loadFont("").value();
    auto* font = fontSvc.getFont(fontId);

    // Create VulkanUiPainter
    vapi::ui::VulkanUiPainter painter;
    painter.init(&device, &renderCtx, font, 16.f, SHADER_DIR);

    // Widget tree
    vapi::ui::Widget root(nullptr);
    root.setGeometry(0, 0, 1280, 720);
    vapi::ui::Button* btn = new vapi::ui::Button(&root);
    btn->setGeometry(100, 100, 200, 50);
    btn->setText("Click me");
    btn->clicked().bind([]() { std::printf("Clicked!\n"); });

    // Frame loop
    while (platform->anyWindowOpen()) {
        platform->pollEvents();
        auto frame = renderCtx.beginFrame().value();
        renderCtx.beginRendering(frame, clearColor);
        renderCtx.setFullViewport(frame);

        painter.beginFrame(frame);
        vapi::DrawContext ctx;
        ctx.userData = &painter;
        root.draw(ctx);
        painter.endFrame(frame);

        renderCtx.endRendering(frame);
        renderCtx.endFrame(frame);
    }
    painter.shutdown();
    return 0;
}
```

**CMake:**

```cmake
add_executable(ui_app main.cpp)
target_link_libraries(ui_app PRIVATE vapi_ui_vulkan vapi_platform vapi_render vapi_resource vapi_font)
target_compile_definitions(ui_app PRIVATE SHADER_DIR="${VAPI_FONT_SHADER_DIR}")
```

Shaders are the same `font_quad.vert.spv` / `font_quad.frag.spv` (see section 5). API details: [detail_docs/ui.md](detail_docs/ui.md), [detail_docs/font_render.md](detail_docs/font_render.md).
