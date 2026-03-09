# Quick reference — main classes and methods

Short table: where to find what. Details in [detail_docs/](detail_docs/).

| Area | Class / function | Main methods / purpose | Document |
|------|------------------|------------------------|----------|
| **Include** | `vapi.hpp` | core + platform + gpu | — |
| | `render/render.hpp` | shaders, pipelines, RenderContext | [render.md](detail_docs/render.md) |
| | `resource/resource.hpp` | buffers, images, staging, descriptors | [resource.md](detail_docs/resource.md) |
| **Errors** | `Result<T>`, `errors::*` | `has_value()`, `error()`, `errorMessage(e)` | [core.md](detail_docs/core.md) |
| **Platform** | `createPlatform(PlatformKind)` | Returns `std::unique_ptr<IPlatform>`; default `PlatformKind::Glfw` | [platform.md](detail_docs/platform.md) |
| | `IPlatform` | `init()`, `terminate()`, `getRequiredVulkanExtensions()` | [platform.md](detail_docs/platform.md) |
| | | Windows: `createWindow(cfg)`, `destroyWindow(id)`, `setCallbacks`, `pollEvents()`, `anyWindowOpen()` | |
| **GPU** | `GpuContext` | `init(GpuContextConfig)`, `shutdown()`, `instance()`, `device()`, `surface()`, `swapchain()`, `recreateSwapchain(w,h)` | [gpu.md](detail_docs/gpu.md) |
| **Shaders** | `ShaderModule` | `loadFromFile(device, path, stage[, allowedRoot][, maxSpirvBytes])`, `loadFromFile(..., LoadShaderOptions)`, `loadFromMemory(...)`, `destroy()`, `stageInfo(entry)` | [render.md](detail_docs/render.md) |
| | `LoadShaderOptions` | `allowedRoot`, `maxSpirvBytes` | [render.md](detail_docs/render.md) |
| | `ShaderProgram` | `vertex`, `fragment`, `compute`, …; `stages(entry)` | [render.md](detail_docs/render.md) |
| **Buffers** | `BufferManager` | `init(device)`, `createBuffer(BufferDesc)`, `destroyBuffer(id)`, `map(id, offset, size)`, `write(id, data, size, offset)`, `handle(id)`, `get(id)` | [resource.md](detail_docs/resource.md) |
| **Images** | `ImageManager` | `init(device)`, `createImage(ImageDesc)`, `destroyImage(id)`, `handle(id)`, `view(id)` | [resource.md](detail_docs/resource.md) |
| **Staging** | `StagingManager` | `init(device, commands)`, `uploadBuffer(dst, data[, offset][, dstSize])`, `uploadBuffer(bufferManager, bufferId, data[, offset])`, `uploadImage(...)`, `updateImage(...)` | [resource.md](detail_docs/resource.md) |
| **Files** | `vapi::file::*` | `readText`, `readBinary`, `resolveSecure(path, root)`, `isPathKeySafe(key)`, `canonicalize`, `tempFilePath` | [core.md](detail_docs/core.md) |
| **CPU bridge** | `ICpuServices` | `getThreadPool()`, `getHostAllocator()`, `getProfiler()` | [core.md](detail_docs/core.md) |
| | `IAppLogic` | `update(dt)`, `onBeginFrame()`, `onEndFrame()` | [core.md](detail_docs/core.md) |
| | `ICpuFrameDriver` | `beginFrame()` → dt, `endFrame()`, `frameIndex()` | [core.md](detail_docs/core.md) |
| | `CpuServicesHolder`, `SimpleCpuFrameDriver` | Implementations in `core/tools/cpu_bridge_impl.hpp` | [core.md](detail_docs/core.md) |
| **Font (headless)** | `TextPainter` | `setPixelHeight`, `measureText`, `layoutText`, `drawText`, `paint` → TextGeometry | [architecture.md](architecture.md), [font_ui_text.md](font_ui_text.md) |
| | `FontDrawList` | `setFont`, `addText`, `addImage`, `getVertices()`, `fillAtlasRgba(out)` (reuse buffer), `getAtlasRgba()`, `reserveForText(text)` | [architecture.md](architecture.md), [font_ui_text.md](font_ui_text.md) |
| | `GlyphAtlas` | `ensureGlyph`, `getQuad`, `isFull()` (atlas full) | [font_ui_text.md](font_ui_text.md) |
| **Window with text** | `FontRenderApp` | `init`, `setFont`, `setFontSize`, `addText`, `run` | [architecture.md](architecture.md), example [calculator_example.cpp](../examples/calculator_example.cpp) |

**Naming:** In docs and examples **IPlatform** is used; alias **IPlatformBackend** is kept for compatibility. **PlatformKind** selects the platform backend (`Glfw` and others).

---

## Module dependencies (Vulkan SDK requirement)

| Module | Requires Vulkan SDK | Notes |
|--------|:-------------------:|-------|
| `core/` | No | Pure C++ types, errors, file utils, logging |
| `platform/` | No | Window/input abstraction (GLFW); exposes `getRequiredVulkanExtensions()` but does not include `vulkan.h` |
| `font/` | No | Font loading, glyph atlas, text measurement — CPU-only |
| `gpu/` | **Yes** | Vulkan wrappers (instance, device, swapchain, commands, sync). All `vk_*.hpp` headers include `vulkan/vulkan.h` |
| `resource/` | **Yes** | Buffer/image/descriptor/staging managers. Uses Vulkan types (`VkBuffer`, `VkImage`, `VkFormat`, etc.) |
| `render/` | **Yes** | Pipeline builder, shader modules, render context. Uses Vulkan types throughout |
| `ui/` | No | Widget system and layout (CPU-side). `VulkanUiPainter` in `vapi_ui_vulkan` requires Vulkan |

**Recommended includes:**
- `#include "vapi.hpp"` — pulls in `core/`, `platform/`, and `gpu/` (requires Vulkan SDK)
- `#include "render/render.hpp"` — render module (requires Vulkan SDK)
- `#include "resource/resource.hpp"` — resource module (requires Vulkan SDK)
- `#include "font/font.hpp"` — font module only (no Vulkan SDK needed)
- `#include "render/font_render_app.hpp"` — high-level window+text (requires Vulkan SDK)
