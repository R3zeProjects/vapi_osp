# VAPI OSP

**VAPI OSP** is a platform for building custom APIs while keeping control over Vulkan: context, wrappers, tools, cross-API ABI. Window and input (GLFW), resize modes and frame cache, render context and pipelines, font module and UI text rendering.

**Current API version:** 0.0.1-alpha (see `CMakeLists.txt` and [docs/VERSIONING.md](docs/VERSIONING.md)). Before 1.0, incompatible changes may occur. The library is **usable now**; broader test coverage is being added for production confidence.

**Distribution:** The library is distributed as compiled binaries for **Windows**. Source code is published openly; official support and ready-made builds are for Windows only.

**License:** Free for non-commercial use. Commercial use requires a written agreement. See [LICENSE](LICENSE) and [licenses/LICENSE_VAPI.txt](licenses/LICENSE_VAPI.txt).

**Copyright (c) 2025–2026 R3zeProjects.**

---

## Quick start (Windows)

1. Install dependencies: **Vulkan SDK**, **GLFW 3.x**. If needed set paths: `-DGLFW3_ROOT=C:/GLFW` and `-DVulkan_ROOT=C:/VulkanSDK/...`.
2. From the repository root:
   ```
   cmake -B build -S .
   cmake --build build
   ```
3. Run example: `build\Debug\calculator_example.exe`

See [docs/quick_start.md](docs/quick_start.md) for more.

---

## Usage examples

### Minimal window with text

Init, font, one text line, blocking loop until window close:

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    auto r = app.init(800, 400, "Hello", "path/to/shaders");  // directory with font_quad.vert.spv, font_quad.frag.spv
    if (!r) {
        std::fprintf(stderr, "init: %s\n", vapi::errorMessage(r.error()).data());
        return 1;
    }
    app.setFont("Arial");
    app.setFontSize(24.f);
    app.addText(50.f, 80.f, "Hello, VAPI!", vapi::color4(1.f, 1.f, 1.f, 1.f));
    return app.run();
}
```

### Interactive app (loop and input)

As in calculator, counter, and stopwatch examples: loop `beginFrame` → `clear` → draw text and “buttons” by mouse coordinates → `endFrame`. Platform and window via `app.getPlatform()` and `app.getWindowId()`; click position via `getCursorPosInFramebuffer()`.

```cpp
vapi::IPlatform* platform = app.getPlatform();
vapi::WindowId winId = app.getWindowId();
vapi::InputCallbacks icb;
icb.onKey = [platform, winId](vapi::WindowId id, s32 key, s32, s32 action, s32) {
    if (key == (s32)vapi::Key::Escape && action == (s32)vapi::KeyAction::Press)
        platform->requestClose(id);
};
icb.onMouseButton = [&](vapi::WindowId id, s32 button, s32 action, s32) {
    if (button != (s32)vapi::MouseButton::Left || action != (s32)vapi::KeyAction::Press) return;
    auto [fx, fy] = platform->getCursorPosInFramebuffer(id);
    // check hit in button area using fx, fy and window size
};
platform->setInputCallbacks(winId, icb);

while (platform->anyWindowOpen()) {
    platform->pollEvents();
    auto frame = app.beginFrame();
    if (!frame) continue;
    app.clear();
    app.addText(x, y, "Text", vapi::color4(1,1,1,1));
    if (!app.endFrame()) break;
}
```

Full examples: [examples/calculator_example.cpp](examples/calculator_example.cpp), [examples/counter_example.cpp](examples/counter_example.cpp), [examples/stopwatch_example.cpp](examples/stopwatch_example.cpp). See [docs/quick_start.md](docs/quick_start.md) and [docs/integration.md](docs/integration.md).

---

## Documentation

- **Russian:** [docs/ru/](docs/ru/) — документация на русском (копия основной).
- **[docs/quick_start.md](docs/quick_start.md)** — Quick start: build, run, minimal code on one page.
- **[docs/](docs/)** — Full documentation: architecture, scenarios, API, examples.
- **[docs/README.md](docs/README.md)** — Documentation index.
- **[docs/architecture.md](docs/architecture.md)** — Layers (core → platform, font, gpu → resource, render; FontRenderApp), scenarios, call order.
- **[docs/integration.md](docs/integration.md)** — Project integration, “scenario → libraries and headers” table.
- **[docs/user_guide.md](docs/user_guide.md)** — User guide: install, build, examples.

Headers: `#include "vapi.hpp"` (core, platform, gpu); for render `#include "render/render.hpp"`; for window with text `#include "render/font_render_app.hpp"`. Module and scenario table: [docs/integration.md](docs/integration.md#3-library-selection-by-scenario), class quick reference: [docs/quick_reference.md](docs/quick_reference.md).

---

## Summary

**VAPI OSP** is a C++23 platform library for building custom rendering APIs with direct Vulkan control. It provides:

- **Window and input** via GLFW (`vapi_platform`)
- **Vulkan backend**: instance, device, swapchain, sync, command recording, GPU profiler, fences (`vapi_gpu`)
- **Render context**: dynamic rendering (no render passes), shader loading, pipeline builder and cache (`vapi_render`)
- **Resource management**: buffers, images, descriptor sets, samplers, staging (`vapi_resource`)
- **Font rendering**: STB TrueType, vector Bezier fonts, glyph atlas, text measurement, `FontDrawList` (`vapi_font`)
- **High-level facade**: `FontRenderApp` — one call for a window with text rendering (`vapi_font_render`)
- **UI widget system**: signals/slots (`Emitter<T>`), 13+ widget types, `VulkanUiPainter` for GPU-backed rendering (`vapi_ui`)

**Current API version:** 0.0.1-alpha. **Distributed as Windows binaries.** Source code is open; official builds target Windows only.

**Quick build (Windows):**
```
cmake -B build -S . -DGLFW3_ROOT=C:/GLFW -DVulkan_ROOT=C:/VulkanSDK/...
cmake --build build
build\Debug\calculator_example.exe
```

**License:** Dual — free for non-commercial use; commercial use requires a written agreement.
See [LICENSE](LICENSE) and [licenses/LICENSE_VAPI.txt](licenses/LICENSE_VAPI.txt) for full terms.
Copyright (c) 2025–2026 R3zeProjects. All rights reserved.

See [CONTRIBUTING.md](CONTRIBUTING.md) to contribute and [docs/quick_start.md](docs/quick_start.md) to get started.
