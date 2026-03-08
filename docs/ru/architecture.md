# Обзор архитектуры VAPI

Документ описывает слои платформы, типичные сценарии использования, порядок вызовов основных API и ссылки на ключевые заголовки и примеры. Детали API — в [detail_docs/](../detail_docs/).

---

## 1. Слои архитектуры

Иерархия снизу вверх:

| Слой | Назначение | Ключевые элементы |
|------|-------------|--------------------|
| **core** | Типы, ошибки, файлы, логгер, математика, интерфейсы (IWindow, IInput, IByteSource и др.) | `core/types.hpp`, `core/error.hpp`, `core/logger.hpp`, `core/file_utils.hpp`, `core/interfaces/` |
| **platform** | Окно, ввод, мониторы; реализация по умолчанию — GLFW | `platform/platform.hpp`, `createPlatform(PlatformKind)`, `IPlatform` |
| **font** | Шрифты, layout, атлас глифов, список отрисовки текста (без GPU) | `font/font_types.hpp`, `font/text_painter.hpp`, `font/font_draw_list.hpp`, `font/glyph_atlas.hpp`, `font/i_font_source.hpp` |
| **gpu** | Vulkan: instance, device, surface, swapchain, sync, command | `gpu/gpu.hpp`, `gpu/gpu_context.hpp` |
| **resource** | Буферы, изображения, дескрипторы, сэмплеры, staging (требуют `VkDeviceWrapper`) | `resource/resource.hpp`, `BufferManager`, `ImageManager`, `DescriptorManager`, `SamplerCache`, `StagingManager` |
| **render** | Шейдеры, пайплайны (graphics/compute), RenderContext (кадр, динамический рендер) | `render/render.hpp`, `render/render_context.hpp`, `ShaderModule`, `GraphicsPipelineBuilder`, `RenderContext` |
| **font_render_app** | Готовое приложение: окно + Vulkan + шрифт + цикл отрисовки текста | `render/font_render_app.hpp`, класс `FontRenderApp` |

- **core** и **platform** не зависят от Vulkan (кроме запроса расширений для instance).
- **font** не зависит от render/GPU: только метрики, layout, вершины и атлас; отрисовку выполняет вызывающий (или FontRenderApp).
- **gpu** даёт устройство и swapchain; **resource** и **render** строятся поверх них.
- **FontRenderApp** объединяет platform, gpu, resource, render и font в один API «окно с текстом».

---

## 2. Типичные сценарии

### 2.1. Headless font (только TextPainter / FontDrawList)

Без окна и Vulkan: загрузка шрифта, измерение и раскладка текста, получение геометрии (вершины + атлас) для последующей отрисовки или экспорта.

- **TextPainter:** `measureText`, `layoutText`, `drawText` / `paint` → `TextGeometry` (вершины + атлас).
- **FontDrawList:** `setFont` → `addText` (и при необходимости `addImage`) → `getVertices()`, `fillAtlasRgba(out)` или `getAtlasRgba()` — один атлас, один буфер вершин, один draw call; `reserveForText(text)` снижает реаллокации. При переполнении атласа глифов — `GlyphAtlas::isFull()`.

### 2.2. Окно с текстом (FontRenderApp)

Минимальный сценарий: одно окно, Vulkan, шрифт, список строк текста; цикл событий и отрисовка инкапсулированы в `FontRenderApp`.

Порядок вызовов:

1. **init**(width, height, title, shaderDir) — платформа, окно, GPU, контекст рендера.
2. **setFont**(fontNameOrPath) — опционально; если не вызван или шрифт не найден — используется fallback.
3. **setFontSize**(pixelHeight) — опционально; по умолчанию 48.
4. **addText**(x, y, text, color) — одна или несколько строк; (x, y) — верх левого угла первой строки.
5. **run**() — блокирующий цикл до закрытия окна; возврат: `kRunSuccess` (0) или `kRunError` (1) и др.

Заголовок: `render/font_render_app.hpp`. Примеры: calculator_example.cpp, counter_example.cpp, stopwatch_example.cpp.

### 2.3. Кастомный рендер (RenderContext + resource + font)

Полный контроль: своё окно (platform), GpuContext, RenderContext, BufferManager/ImageManager/DescriptorManager/Staging, свой пайплайн и привязка геометрии из FontDrawList или TextPainter.

Типичный порядок:

1. **Платформа и окно:** `createPlatform(PlatformKind::Glfw)` → `init()` → `createWindow(config)` → `getNativeHandle(winId)`.
2. **GPU:** `GpuContext::init(config)` (в config — handle окна, extent swapchain, расширения от платформы).
3. **Рендер:** `RenderContext::init(device, swapchain, config)`.
4. **Ресурсы:** инициализация BufferManager, ImageManager, DescriptorManager, StagingManager по необходимости; создание буферов/текстур, загрузка вершин и атласа из FontDrawList.
5. **Цикл кадра:** `beginFrame()` → при успехе `beginRendering(frame, clearColor)` → viewport, привязка пайплайна/дескрипторов → draw → `endRendering(frame)` → `endFrame(frame)`; при resize — `recreateSwapchain` и `handleResize`.

Детали и пример цикла — в [detail_docs/render.md](../detail_docs/render.md), [detail_docs/resource.md](../detail_docs/resource.md), [detail_docs/gpu.md](../detail_docs/gpu.md).

---

## 3. Порядок вызовов (кратко)

- **FontRenderApp:** init → setFont [→ setFontSize] → addText (0..n) → run.
- **FontDrawList (headless или перед своим рендером):** setFont → addText / addImage → getVertices(), getAtlasRgba(); при смене контента — clear() и снова add*.
- **TextPainter (только layout/paint):** setPixelHeight [и прочие опции] → measureText / layoutText / drawText / paint → использование TextGeometry или appendText во внешний атлас/вершины.
- **RenderContext в своём цикле:** init(device, swapchain, config) → в цикле: beginFrame → beginRendering → draw → endRendering → endFrame; при resize — recreateSwapchain + handleResize.

Подробные сигнатуры и примеры кода — в [detail_docs/](../detail_docs/) и в указанных заголовках.
