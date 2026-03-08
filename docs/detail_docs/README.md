# VAPI API — Detailed Reference

This folder documents each part of the VAPI API with descriptions and code examples.

| Document | Contents |
|----------|----------|
| [core.md](core.md) | Types, `Error`, `Result`, `errorMessage`, `Handle`/`HandlePool`, file utils, logger, math, easing, core interfaces — **with code examples** |
| [platform.md](platform.md) | `IPlatform`, `IWindowControl`, `IWindow`, window/input config, `createPlatform`, monitors, GLFW — **with code examples** |
| [gpu.md](gpu.md) | `GpuContext`, instance/device/surface/swapchain wrappers, sync/command, base helpers, GPU errors — **with code examples** |
| [render.md](render.md) | Vertex types, `ShaderModule`, graphics/compute pipeline builders, `RenderContext`, full pipeline usage — **with code examples** |
| [resource.md](resource.md) | `BufferManager`, `ImageManager`, `DescriptorManager`, `SamplerCache`, `Staging`, full resource example — **with code examples** |
| [ui.md](ui.md) | Widget system, `Emitter<T>`, `IPainter`, `VulkanUiPainter`, all widget types — **with code examples** |
| [font_render.md](font_render.md) | `FontRenderApp`, `FontDrawList`, `VulkanUiPainter` setup and frame loop — **with code examples** |

Each document includes **copy-pasteable code examples** for the main APIs. Examples use `vapi::` prefix or `using namespace vapi;`. Include paths are relative to the project `include/` directory.
