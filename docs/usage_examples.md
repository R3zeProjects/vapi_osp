# VAPI OSP usage examples

Short examples for typical scenarios: window with text and interactive app. All examples use only **FontRenderApp** and **IPlatform** API (no direct Vulkan or GLFW in code).

---

## 1. Minimal window with text

Single `app.run()` call — blocking loop until window close. Text is set before `run()` and does not change.

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    // shaderDir — directory with font_quad.vert.spv and font_quad.frag.spv
    auto r = app.init(800, 400, "Title", "path/to/shaders");
    if (!r) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(r.error()).data());
        return vapi::kRunError;
    }
    app.setFont("Arial");
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "First line", vapi::color4(1.f, 1.f, 1.f, 1.f));
    app.addText(50.f, 110.f, "Second line", vapi::color4(0.9f, 0.9f, 1.f, 1.f));

    return app.run();
}
```

**CMake:** link with `vapi_font_render`, add `target_compile_definitions(my_app PRIVATE SHADER_DIR="${VAPI_FONT_SHADER_DIR}")` if shaders come from the VAPI build tree.

---

## 2. Interactive app (custom loop)

Each frame: handle events, begin frame, clear, draw text and “buttons”, end frame. Input via `InputCallbacks` and `getCursorPosInFramebuffer()`.

```cpp
#include "render/font_render_app.hpp"
#include "platform/platform_codes.hpp"
#include "core/types.hpp"
#include "core/error.hpp"
#include <cstdio>
#include <string>

#ifdef SHADER_DIR
#define VAPI_SHADER_DIR SHADER_DIR
#else
#define VAPI_SHADER_DIR "."
#endif

int main() {
    using namespace vapi;

    FontRenderApp app;
    auto initRes = app.init(320, 200, "Example", VAPI_SHADER_DIR);
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", errorMessage(initRes.error()).data());
        return kRunError;
    }
    app.setFont("Arial");
    app.setFontSize(22.f);

    IPlatform* platform = app.getPlatform();
    WindowId winId = app.getWindowId();
    if (!platform || winId == 0) return kRunError;

    int counter = 0;
    InputCallbacks icb;
    icb.onKey = [platform, winId](WindowId id, s32 key, s32, s32 action, s32) {
        if (key == static_cast<s32>(Key::Escape) && action == static_cast<s32>(KeyAction::Press))
            platform->requestClose(id);
    };
    icb.onMouseButton = [&counter, platform, winId](WindowId id, s32 button, s32 action, s32) {
        if (button != static_cast<s32>(MouseButton::Left) || action != static_cast<s32>(KeyAction::Press))
            return;
        auto [fx, fy] = platform->getCursorPosInFramebuffer(id);
        WindowState ws = platform->getWindowState(id);
        float w = static_cast<float>(ws.framebufferWidth > 0 ? ws.framebufferWidth : 1);
        float h = static_cast<float>(ws.framebufferHeight > 0 ? ws.framebufferHeight : 1);
        // Button in center (approx 100x40)
        float bx = w * 0.5f - 50.f, by = h * 0.5f - 20.f;
        if (fx >= bx && fx <= bx + 100.f && fy >= by && fy <= by + 40.f)
            counter++;
    };
    platform->setInputCallbacks(winId, icb);

    while (platform->anyWindowOpen()) {
        platform->pollEvents();
        auto frameRes = app.beginFrame();
        if (!frameRes) {
            if (frameRes.error() == errors::NotInitialized) break;
            continue;
        }
        FrameInfo fi = *frameRes;
        app.clear();

        float w = static_cast<float>(fi.width), h = static_cast<float>(fi.height);
        float lineH = app.getLineHeight();
        app.addText(20.f, 20.f, "Clicks: " + std::to_string(counter), color4(0.95f, 0.95f, 1.f, 1.f));
        app.addText(w * 0.5f - 30.f, h * 0.5f - lineH * 0.5f, "Click", color4(0.4f, 0.7f, 1.f, 1.f));

        if (!app.endFrame()) break;
    }
    return kRunSuccess;
}
```

---

## 3. Structure as in calculator

In [examples/calculator_example.cpp](../examples/calculator_example.cpp):

- Button grid uses relative coordinates (fractions of window width/height).
- Click is handled in `onMouseButton`: normalized `(fx/fbW, fy/fbH)` compared to cell bounds.
- Press animation: last pressed button and time stored; in the draw loop `pressFactor` is computed via `applyEasing(Easing::EaseOutCubic, t)` and text/color are offset.

Same approach in [counter_example.cpp](../examples/counter_example.cpp) (single button) and [stopwatch_example.cpp](../examples/stopwatch_example.cpp) (three buttons, timer).

---

## 4. Dependencies and build

| Scenario | CMake target | Headers |
|----------|--------------|---------|
| Window with text | `vapi_font_render` | `render/font_render_app.hpp`, `core/error.hpp` |
| Platform/input | (included in font_render) | `platform/platform_codes.hpp` for Key, MouseButton, KeyAction |

Shader path is passed to `app.init(width, height, title, shaderDir)`. When building with VAPI’s CMake you can use `SHADER_DIR="${VAPI_FONT_SHADER_DIR}"` (directory with built `font_quad.vert.spv`, `font_quad.frag.spv`).

See also: [integration.md](integration.md), [quick_start.md](quick_start.md).
