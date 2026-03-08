# Краткая справка — основные классы и методы

Краткая таблица: где что искать. Подробности — в [detail_docs/](../detail_docs/).

| Область | Класс / функция | Основные методы / назначение | Документ |
|--------|----------------------------------|-----------------------------|----------|
| **Include** | `vapi.hpp` | core + platform + gpu | — |
| | `render/render.hpp` | шейдеры, пайплайны, RenderContext | [render.md](../detail_docs/render.md) |
| | `resource/resource.hpp` | буферы, изображения, staging, дескрипторы | [resource.md](../detail_docs/resource.md) |
| **Ошибки** | `Result<T>`, `errors::*` | `has_value()`, `error()`, `errorMessage(e)` | [core.md](../detail_docs/core.md) |
| **Платформа** | `createPlatform(PlatformKind)` | Возвращает `std::unique_ptr<IPlatform>`; по умолчанию `PlatformKind::Glfw` | [platform.md](../detail_docs/platform.md) |
| | `IPlatform` | `init()`, `terminate()`, `getRequiredVulkanExtensions()`; окна: `createWindow(cfg)`, `destroyWindow(id)`, `setCallbacks`, `pollEvents()`, `anyWindowOpen()` | |
| **GPU** | `GpuContext` | `init(GpuContextConfig)`, `shutdown()`, `instance()`, `device()`, `surface()`, `swapchain()`, `recreateSwapchain(w,h)` | [gpu.md](../detail_docs/gpu.md) |
| **Шейдеры** | `ShaderModule` | `loadFromFile(device, path, stage[, allowedRoot][, maxSpirvBytes])`, `loadFromMemory(...)`, `destroy()`, `stageInfo(entry)` | [render.md](../detail_docs/render.md) |
| | `LoadShaderOptions` | `allowedRoot`, `maxSpirvBytes` | |
| **Буферы** | `BufferManager` | `init(device)`, `createBuffer(BufferDesc)`, `destroyBuffer(id)`, `map(id, offset, size)`, `write(...)`, `handle(id)`, `get(id)` | [resource.md](../detail_docs/resource.md) |
| **Изображения** | `ImageManager` | `init(device)`, `createImage(ImageDesc)`, `destroyImage(id)`, `handle(id)`, `view(id)` | [resource.md](../detail_docs/resource.md) |
| **Staging** | `StagingManager` | `init(device, commands)`, `uploadBuffer(...)`, `uploadImage(...)`, `updateImage(...)` | [resource.md](../detail_docs/resource.md) |
| **Файлы** | `vapi::file::*` | `readText`, `readBinary`, `resolveSecure(path, root)`, `isPathKeySafe(key)`, `canonicalize`, `tempFilePath` | [core.md](../detail_docs/core.md) |
| **CPU связка** | `ICpuServices` | `getThreadPool()`, `getHostAllocator()`, `getProfiler()` | [core.md](../detail_docs/core.md) |
| | `IAppLogic` | `update(dt)`, `onBeginFrame()`, `onEndFrame()` | |
| | `ICpuFrameDriver` | `beginFrame()` → dt, `endFrame()`, `frameIndex()` | |
| **Шрифт (headless)** | `TextPainter` | `setPixelHeight`, `measureText`, `layoutText`, `drawText`, `paint` → TextGeometry | [architecture.md](architecture.md) |
| | `FontDrawList` | `setFont`, `addText`, `addImage`, `getVertices()`, `fillAtlasRgba(out)`, `getAtlasRgba()`, `reserveForText(text)` | [architecture.md](architecture.md) |
| | `GlyphAtlas` | `ensureGlyph`, `getQuad`, `isFull()` (переполнение атласа) | |
| **Окно с текстом** | `FontRenderApp` | `init`, `setFont`, `setFontSize`, `addText`, `run` | [architecture.md](architecture.md), пример [calculator_example.cpp](../../examples/calculator_example.cpp) |

**Именование:** в документации и примерах используется **IPlatform**; алиас **IPlatformBackend** сохранён для совместимости. **PlatformKind** — выбор бэкенда платформы (`Glfw` и др.).
