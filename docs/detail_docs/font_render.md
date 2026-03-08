# vapi_font_render / vapi_ui_vulkan — Detail Documentation

Covers two closely related targets:
- **`vapi_font_render`** — `FontRenderApp`: all-in-one window + Vulkan + text rendering facade
- **`vapi_ui_vulkan`** — `VulkanUiPainter`: concrete `IPainter` for widget rendering via Vulkan

---

## FontRenderApp

**Target:** `vapi_osp::vapi_font_render`  
**Header:** `#include "render/font_render_app.hpp"`

### Purpose

`FontRenderApp` combines GLFW window creation, Vulkan initialization, font loading, and a frame loop into a single high-level API. Use it when you want a window with text rendering in minimal lines of code.

### Lifecycle

```
init() → setFont() [optional] → setFontSize() [optional]
  → either: addText() + run()         (static scene)
  → or:     setFrameDrawCallback() + run()  (interactive)
  → or:     beginFrame() + endFrame() repeated  (manual loop)
```

### Init

```cpp
FontRenderApp app;
auto r = app.init(1280, 720, "My App", SHADER_DIR);
```

`SHADER_DIR` must contain `font_quad.vert.spv` and `font_quad.frag.spv` (compiled from `examples/shaders/`).

| Method | Description |
|--------|-------------|
| `Result<void> init(u32 w, u32 h, std::string_view title, std::string_view shaderDir)` | Create window + Vulkan + render context |
| `Result<FontId> setFont(std::string_view nameOrPath)` | Load system/file font; falls back to built-in Bezier font on failure |
| `void setFontSize(f32 pixels)` | Set font size (default 48 px) |

### Static Scene Mode

Add text before `run()`. The scene is fixed for the entire window lifetime.

```cpp
app.setFont("");
app.setFontSize(32.f);
app.addText(100, 100, "Hello, World!", {1,1,1,1});
return app.run();  // returns kRunSuccess (0) or kRunError (1)
```

| Method | Description |
|--------|-------------|
| `void addText(f32 x, f32 y, std::string_view text, color4 color)` | Add text to static draw list |
| `void clear()` | Clear static draw list |
| `f32 getLineHeight() const` | Current font line height in pixels |
| `int run()` | Enter event loop; returns `kRunSuccess` or `kRunError` |

### Interactive Mode (Frame Callback)

```cpp
app.setFrameDrawCallback([](FontDrawList& dl, u32 w, u32 h) {
    dl.addText(10, 10, "FPS: 60", {1,1,0,1});
});
return app.run();
```

| Type | Signature |
|------|-----------|
| `FrameDrawCallback` | `void(FontDrawList& drawList, u32 width, u32 height)` |
| `MouseButtonCallback` | `void(WindowId, s32 button, s32 action, s32 mods)` |

| Method | Description |
|--------|-------------|
| `void setFrameDrawCallback(FrameDrawCallback)` | Set per-frame draw lambda |
| `void setMouseButtonCallback(MouseButtonCallback)` | Set mouse button handler |

### Manual Loop Mode

```cpp
while (app.getPlatform()->anyWindowOpen()) {
    app.getPlatform()->pollEvents();
    auto info = app.beginFrame().value();
    // ... update game state ...
    app.clear();
    app.addText(10, 10, "Tick", {1,1,1,1});
    app.endFrame();
}
```

| Method | Description |
|--------|-------------|
| `Result<FrameInfo> beginFrame()` | Handle resize, return frame dimensions and time |
| `Result<void> endFrame()` | Upload atlas/vertices, render, present |
| `IPlatform* getPlatform()` | Access the platform layer (events, input) |
| `WindowId getWindowId() const` | GLFW window ID |

### FrameInfo

```cpp
struct FrameInfo {
    u32    width{0};
    u32    height{0};
    double timeSeconds{0.0};
};
```

---

## FontDrawList

**Header:** `#include "font/font_draw_list.hpp"`  
**Used by:** FontRenderApp internally; exposed via frame callbacks and `setFrameDrawCallback()`.

Single-atlas, single-draw-call accumulator for text and UI primitives.

### Font Setup

```cpp
FontDrawList dl(1024u, 1024u);   // atlas size
dl.setFont(myFontSource, 24.f, 2u);  // font, pixel height, oversample
```

| Method | Description |
|--------|-------------|
| `void setFont(IFontSource*, f32 pixelHeight, u32 oversample = 2)` | Set current font |
| `f32 getLineHeight() const` | Line height in pixels (0 if no font set) |

### Draw Commands

| Method | Description |
|--------|-------------|
| `void addText(f32 x, f32 y, std::string_view text, color4 color)` | Add UTF-8 text at position |
| `void addRect(const rect& r, color4 color)` | Filled rectangle |
| `void addImage(const DrawRect& screen, const DrawRect& uv, color4 color)` | Textured quad |
| `void reserveForText(std::string_view)` | Pre-allocate vertex storage |
| `void clear()` | Reset vertices and atlas (keeps font) |

### Spacing Control

| Method | Description |
|--------|-------------|
| `void setEqualSpacing(bool)` | Fixed glyph step (monospace-like) |
| `void setExtraGlyphSpacing(f32 px)` | Fixed extra gap in pixels |
| `void setExtraGlyphSpacingRatio(f32)` | Extra gap = ratio × pixelHeight |
| `void setExtraGlyphSpacingRatioToWidth(f32)` | Extra gap += ratio × advance |
| `void setLineSpacing(f32 px)` | Override line height |

### GPU Upload Data

| Method | Description |
|--------|-------------|
| `const std::vector<FontVertex>& getVertices() const` | Vertex buffer data |
| `void fillAtlasRgba(std::vector<u8>& out) const` | Fill RGBA atlas buffer (reuse buffer, hot-path) |
| `std::vector<u8> getAtlasRgba() const` | Allocate and return RGBA atlas |
| `u32 getAtlasWidth() / getAtlasHeight() const` | Atlas dimensions |
| `bool empty() const` | True if no vertices accumulated |

---

## VulkanUiPainter

**Target:** `vapi_osp::vapi_ui_vulkan`  
**Header:** `#include "ui/vulkan_ui_painter.hpp"`  
**Dependencies:** `vapi_ui`, `vapi_render`, `vapi_resource`, `vapi_font`

### Purpose

Concrete `IPainter` that renders widget draw commands (`drawRect`, `drawText`, `drawTextInRect`) using `FontDrawList` and a Vulkan dynamic rendering pipeline. Integrates into an existing `RenderContext` frame.

### Initialization

```cpp
VulkanUiPainter painter;
auto r = painter.init(
    &device,      // VkDeviceWrapper*
    &renderCtx,   // RenderContext*
    fontSource,   // IFontSource* (from FontService or AsciiBezierFontSource)
    16.f,         // default font size in pixels
    shaderDir     // path to font_quad.vert.spv / font_quad.frag.spv
);
```

`shaderDir` is the same directory used by `FontRenderApp` — compiled SPIR-V from `examples/shaders/`.

### Per-Frame Usage

```cpp
// Inside your frame loop, after RenderContext::beginRendering():

painter.beginFrame(frame);          // clear draw list

DrawContext ctx;
ctx.userData = &painter;
rootWidget.draw(ctx);               // traverse widget tree → calls paint() → IPainter methods

painter.endFrame(frame);            // upload atlas+vertices → vkCmdDraw
```

### Font Management

```cpp
painter.setFont(newFontSource, 20.f);  // change font between frames (not mid-frame)
```

### Resource Ownership

`VulkanUiPainter` owns:
- `ImageManager`, `BufferManager`, `StagingManager`, `SamplerCache` (internal)
- Per-frame image and vertex buffer slots (2 frames in flight)
- Vulkan pipeline, pipeline layout, descriptor pool, sampler

Call `shutdown()` before destroying the Vulkan device.

### API Summary

| Method | Description |
|--------|-------------|
| `Result<void> init(device, renderCtx, font, fontSize, shaderDir)` | Initialize GPU resources |
| `void shutdown()` | Release all GPU resources |
| `void setFont(IFontSource*, f32)` | Update font source and size |
| `void beginFrame(const FrameData&)` | Clear draw list, store frame reference |
| `void endFrame(const FrameData&)` | Upload and draw |
| `bool isInitialized() const` | True after successful init |
| `void drawRect(rect, color4 fill, color4 border)` | Filled rect with optional border |
| `void drawText(x, y, text, color, pixelHeight)` | Text at position |
| `void drawTextInRect(rect, text, color, pixelHeight, align)` | Text with alignment |

### Integration Example

```cpp
// Setup (once):
VulkanUiPainter uiPainter;
uiPainter.init(&device, &renderCtx, fontSrc, 14.f, SHADER_DIR);

Widget root(nullptr);
root.setGeometry(0, 0, 1280, 720);
// ... add children ...

// Frame loop:
while (running) {
    platform.pollEvents();
    auto frame = renderCtx.beginFrame().value();
    renderCtx.beginRendering(frame, clearColor);
    renderCtx.setFullViewport(frame);

    uiPainter.beginFrame(frame);
    DrawContext ctx;
    ctx.userData = &uiPainter;
    root.draw(ctx);
    uiPainter.endFrame(frame);

    renderCtx.endRendering(frame);
    renderCtx.endFrame(frame);
}
uiPainter.shutdown();
```

---

## Shader Requirements

Both `FontRenderApp` and `VulkanUiPainter` use the same shaders:

| File | Stage |
|------|-------|
| `font_quad.vert.spv` | Vertex: transforms 2D position with viewport push constant |
| `font_quad.frag.spv` | Fragment: samples atlas texture, multiplies by vertex color |

**Vertex push constant:** 8 bytes = `vec2(viewportWidth, viewportHeight)`  
**Descriptor set binding 0:** `combined_image_sampler` — the glyph atlas texture

Source GLSL: `examples/shaders/font_quad.vert` / `font_quad.frag`  
Compile: `glslangValidator -V font_quad.vert -o font_quad.vert.spv`  
CMake compiles these automatically when `glslangValidator` is found.

---

## See Also

- [ui.md](ui.md) — Widget system and IPainter interface
- [architecture.md](../architecture.md) — Layer dependency diagram
- [integration.md](../integration.md) — CMake linking for vapi_font_render and vapi_ui_vulkan
