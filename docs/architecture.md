# VAPI architecture overview

This document describes the platform layers, typical usage scenarios, main API call order, and links to key headers and examples. API details are in [detail_docs/](detail_docs/).

---

## 1. Architecture layers

Bottom-up hierarchy:

| Layer | Purpose | Key elements |
|-------|---------|--------------|
| **core** | Types, errors, files, logger, math, interfaces (IWindow, IInput, IByteSource, etc.) | `core/types.hpp`, `core/error.hpp`, `core/logger.hpp`, `core/file_utils.hpp`, `core/interfaces/` |
| **platform** | Window, input, monitors; default implementation is GLFW | `platform/platform.hpp`, `createPlatform(PlatformKind)`, `IPlatform` |
| **font** | Fonts, layout, glyph atlas, text draw list (no GPU) | `font/font_types.hpp`, `font/text_painter.hpp`, `font/font_draw_list.hpp`, `font/glyph_atlas.hpp`, `font/i_font_source.hpp` |
| **gpu** | Vulkan: instance, device, surface, swapchain, sync, command | `gpu/gpu.hpp`, `gpu/gpu_context.hpp` |
| **resource** | Buffers, images, descriptors, samplers, staging (require `VkDeviceWrapper`) | `resource/resource.hpp`, `BufferManager`, `ImageManager`, `DescriptorManager`, `SamplerCache`, `StagingManager` |
| **render** | Shaders, pipelines (graphics/compute), RenderContext (frame, dynamic render) | `render/render.hpp`, `render/render_context.hpp`, `ShaderModule`, `GraphicsPipelineBuilder`, `RenderContext` |
| **font_render_app** | Ready-made app: window + Vulkan + font + text draw loop | `render/font_render_app.hpp`, class `FontRenderApp` |

- **core** and **platform** do not depend on Vulkan (except requesting instance extensions).
- **font** does not depend on render/GPU: only metrics, layout, vertices and atlas; the caller (or FontRenderApp) does the drawing.
- **gpu** provides device and swapchain; **resource** and **render** are built on top.
- **FontRenderApp** combines platform, gpu, resource, render and font into one “window with text” API.

---

## 2. Typical scenarios

### 2.1. Headless font (TextPainter / FontDrawList only)

No window and no Vulkan: load font, measure and layout text, get geometry (vertices + atlas) for later drawing or export.

- **TextPainter:** `measureText`, `layoutText`, `drawText` / `paint` → `TextGeometry` (vertices + atlas).
- **FontDrawList:** `setFont` → `addText` (and optionally `addImage`) → `getVertices()`, `fillAtlasRgba(out)` or `getAtlasRgba()` — one atlas, one vertex buffer, one draw call; `reserveForText(text)` reduces reallocations. When glyph atlas is full — `GlyphAtlas::isFull()`.

Headers: [font/text_painter.hpp](../include/font/text_painter.hpp), [font/font_draw_list.hpp](../include/font/font_draw_list.hpp). UI text docs: [font_ui_text.md](font_ui_text.md).

### 2.2. Window with text (FontRenderApp)

Minimal scenario: one window, Vulkan, font, list of text lines; event loop and drawing are encapsulated in `FontRenderApp`.

Call order (without repeating header details):

1. **init**(width, height, title, shaderDir) — platform, window, GPU, render context.
2. **setFont**(fontNameOrPath) — optional; if not called or font not found, fallback is used.
3. **setFontSize**(pixelHeight) — optional; default 48.
4. **addText**(x, y, text, color) — one or more lines; (x, y) is top-left of first line.
5. **run**() — blocking loop until window close; returns `kRunSuccess` (0) or `kRunError` (1), etc.

Header: [render/font_render_app.hpp](../include/render/font_render_app.hpp). Examples: [examples/calculator_example.cpp](../examples/calculator_example.cpp), [counter_example.cpp](../examples/counter_example.cpp), [stopwatch_example.cpp](../examples/stopwatch_example.cpp).

### 2.3. Custom render (RenderContext + resource + font)

Full control: your window (platform), GpuContext, RenderContext, BufferManager/ImageManager/DescriptorManager/Staging, your pipeline and geometry from FontDrawList or TextPainter.

Typical order:

1. **Platform and window:** `createPlatform(PlatformKind::Glfw)` (or default) → `init()` → `createWindow(config)` → `getNativeHandle(winId)`.
2. **GPU:** `GpuContext::init(config)` (config: window handle, swapchain extent, platform requiredExtensions).
3. **Render:** `RenderContext::init(device, swapchain, config)`.
4. **Resources:** init BufferManager, ImageManager, DescriptorManager, SamplerCache, StagingManager as needed; create buffers/textures, upload vertices and atlas from FontDrawList (`getVertices()`, `fillAtlasRgba(out)` or `getAtlasRgba()`).
5. **Frame loop:** `beginFrame()` → on success `beginRendering(frame, clearColor)` → set viewport → bind pipeline/descriptors → draw → `endRendering(frame)` → `endFrame(frame)`; on resize — `recreateSwapchain` and `handleResize`.

Headers: [render/render_context.hpp](../include/render/render_context.hpp), [gpu/gpu_context.hpp](../include/gpu/gpu_context.hpp), [resource/resource.hpp](../include/resource/resource.hpp). Details: [detail_docs/render.md](detail_docs/render.md), [detail_docs/resource.md](detail_docs/resource.md), [detail_docs/gpu.md](detail_docs/gpu.md). RenderContext loop example: “Full render loop example” in [detail_docs/render.md](detail_docs/render.md).

---

## 3. Call order (summary)

- **FontRenderApp:** init → setFont [→ setFontSize] → addText (0..n) → run.
- **FontDrawList (headless or before your render):** setFont → addText / addImage → getVertices(), getAtlasRgba(); when content changes — clear() and add* again.
- **TextPainter (layout/paint only):** setPixelHeight [and other options] → measureText / layoutText / drawText / paint → use TextGeometry or appendText to external atlas/vertices.
- **RenderContext in your loop:** init(device, swapchain, config) → in loop: beginFrame → beginRendering → draw → endRendering → endFrame; on resize — recreateSwapchain + handleResize.

Signatures and code examples are in [detail_docs/](detail_docs/) and the referenced headers.

---

## 4. Key headers and examples

| Purpose | Header | Example |
|---------|--------|---------|
| Root include | `vapi.hpp` | — |
| Render (shaders, pipelines, context) | `render/render.hpp`, `render/render_context.hpp` | [detail_docs/render.md](detail_docs/render.md) |
| Resources (buffers, images, descriptors, staging) | `resource/resource.hpp` | [detail_docs/resource.md](detail_docs/resource.md) |
| Window with text | `render/font_render_app.hpp` | [examples/calculator_example.cpp](../examples/calculator_example.cpp) |
| Text draw list | `font/font_draw_list.hpp` | [font_ui_text.md](font_ui_text.md) |
| Platform and window | `platform/platform.hpp` | [detail_docs/platform.md](detail_docs/platform.md) |
| GPU (context, device, swapchain) | `gpu/gpu.hpp`, `gpu/gpu_context.hpp` | [detail_docs/gpu.md](detail_docs/gpu.md) |
| Core (types, errors, files) | `core/types.hpp`, `core/error.hpp`, etc. | [detail_docs/core.md](detail_docs/core.md) |

Header paths are relative to the project `include/` directory. Example paths are relative to the repository root.
