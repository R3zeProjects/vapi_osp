# VAPI — полная документация с примерами кода

Детальное руководство по платформе **VAPI**: архитектура, модули, API и готовые примеры кода для типичных сценариев.

---

## Оглавление

1. [Введение и быстрый старт](#1-введение-и-быстрый-старт)
2. [Подключение заголовков](#2-подключение-заголовков)
3. [Core: типы, ошибки, файлы](#3-core-типы-ошибки-файлы)
4. [Platform: окно и ввод](#4-platform-окно-и-ввод)
5. [GPU: Vulkan и GpuContext](#5-gpu-vulkan-и-gpucontext)
6. [Render: шейдеры, пайплайны, RenderContext](#6-render-шейдеры-пайплайны-rendercontext)
7. [Resource: буферы, изображения, дескрипторы, staging](#7-resource-буферы-изображения-дескрипторы-staging)
8. [Font: шрифты и отрисовка текста](#8-font-шрифты-и-отрисовка-текста)
9. [FontRenderApp: окно с текстом «из коробки»](#9-fontrenderapp-окно-с-текстом-из-коробки)
10. [Полные примеры приложений](#10-полные-примеры-приложений)
11. [Ссылки на детальную справку](#11-ссылки-на-детальную-справку)

---

## 1. Введение и быстрый старт

**VAPI** — платформа для построения собственных API с сохранением контроля над Vulkan. Она не подменяет Vulkan, а упорядочивает его использование: контекст, обёртки, инструменты, единые контракты. В комплекте: окно и ввод (GLFW), режимы resize и кеш кадров, рендер-контекст и пайплайны, модуль шрифтов и отрисовка UI-текста.

**Минимальный пример (окно с текстом):**

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    auto initRes = app.init(800, 600, "VAPI Render", "shaders");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    app.setFont("Arial");      // имя или путь к .ttf/.otf
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "Hello, VAPI!", vapi::color4(1.f, 1.f, 1.f, 1.f));
    return app.run();
}
```

Сборка: линкуйте с VAPI (core, platform, gpu, font, render, resource). Каталог `shaders` должен содержать `font_quad.vert.spv` и `font_quad.frag.spv` (см. [examples/shaders/](../examples/shaders/)).

---

## 2. Подключение заголовков

Пути относительно каталога `include/` проекта.

| Назначение | Подключение |
|------------|-------------|
| Базовое (core + platform + GPU) | `#include "vapi.hpp"` |
| Рендер (шейдеры, пайплайны, контекст) | `#include "render/render.hpp"` |
| Ресурсы (буферы, изображения, staging) | `#include "resource/resource.hpp"` |
| Окно с текстом | `#include "render/font_render_app.hpp"` |
| Шрифт (TextPainter, FontDrawList) | `#include "font/font.hpp"` или отдельно `font/text_painter.hpp`, `font/font_draw_list.hpp` |

```cpp
// Минимальный набор для кастомного рендера с текстом
#include "vapi.hpp"
#include "render/render.hpp"
#include "resource/resource.hpp"
#include "font/font_draw_list.hpp"
```

---

## 3. Core: типы, ошибки, файлы

Подключение: `#include "vapi.hpp"` (уже тянет types, error, logger, file_utils) или по отдельности из `core/`.

### 3.1. Типы (types.hpp)

Используйте алиасы вместо сырых типов:

```cpp
#include "core/types.hpp"

vapi::u32 width = 800;
vapi::f32 scale = 1.5f;
vapi::usize count = items.size();
vapi::vec2 pos(10.f, 20.f);
vapi::vec3 pos3(0.f, 0.f, -5.f);
vapi::color4 red = vapi::color4::red();
vapi::color4 custom = vapi::color4(0.2f, 0.5f, 0.8f, 1.f);
vapi::rect r{ {0, 0}, {100, 100} };
vapi::SizeElement size(1280, 720);
```

### 3.2. Ошибки и Result (error.hpp)

```cpp
#include "core/error.hpp"

vapi::Result<int> parse(const std::string& s) {
    if (s.empty())
        return vapi::errors::InvalidArgument;
    return 42;
}

void example() {
    auto res = parse("");
    if (!res.has_value()) {
        std::string_view msg = vapi::errorMessage(res.error());
        std::fprintf(stderr, "Error: %.*s\n", (int)msg.size(), msg.data());
        return;
    }
    int value = *res;
}

// Макросы для раннего выхода при ошибке
vapi::Result<void> initAll() {
    VAPI_TRY_VOID(platform->init());
    VAPI_TRY_ASSIGN(wid, platform->createWindow(cfg));
    return {};
}
```

### 3.3. Файлы (file_utils.hpp)

```cpp
#include "core/file_utils.hpp"

namespace vf = vapi::file;

// Чтение
auto text = vf::readText("config.json");
if (text) { /* std::string content = *text; */ }

auto bin = vf::readBinary("data.bin");
if (bin) { /* std::vector<vapi::u8> data = *bin; */ }

// Пути
std::string ext = vf::getExtension(path);
auto parent = vf::getParent(path);
auto files = vf::listFilesByExtension("assets", ".png");

// Существование и размер
if (vf::fileExists("shaders/vert.spv"))
    size_t sz = vf::fileSize("shaders/vert.spv");
```

### 3.4. Логгер (logger.hpp)

```cpp
#include "core/logger.hpp"

vapi::logInfo("App", "Started");
vapi::logWarn("GPU", "Fallback adapter used");
vapi::logError("Render", "Pipeline creation failed");

vapi::Logger::instance().addSink(std::make_unique<vapi::FileSink>("app.log"));
vapi::Logger::instance().setMinLevel("Noise", vapi::LogLevel::Warning);
```

---

## 4. Platform: окно и ввод

Подключение: `#include "platform/platform.hpp"` или `vapi.hpp`.

### 4.1. Создание платформы и окна

```cpp
#include "platform/platform.hpp"
#include "platform/platform_codes.hpp"

std::unique_ptr<vapi::IPlatform> platform = vapi::createPlatform();

if (!platform->init().has_value()) {
    std::fprintf(stderr, "Failed to init platform.\n");
    return 1;
}

vapi::WindowConfig cfg;
cfg.title = "VAPI Window";
cfg.width = 800;
cfg.height = 600;
cfg.resizable = true;

auto widResult = platform->createWindow(cfg);
if (!widResult.has_value()) {
    platform->terminate();
    return 1;
}
vapi::WindowId winId = *widResult;
```

### 4.2. Callbacks окна и ввода

```cpp
vapi::WindowCallbacks wcb{};
wcb.onResize = [](vapi::WindowId, u32 w, u32 h) { /* layout */ };
wcb.onFramebufferResize = [](vapi::WindowId, u32 w, u32 h) {
    // Здесь обновлять swapchain: w, h — размер фреймбуфера
};
wcb.onClose = [](vapi::WindowId) { /* save & exit */ };
platform->setCallbacks(winId, wcb);

vapi::InputCallbacks icb{};
icb.onKey = [&platform](vapi::WindowId id, s32 key, s32, s32 action, s32) {
    if (key == static_cast<s32>(vapi::Key::Escape) &&
        action == static_cast<s32>(vapi::KeyAction::Press))
        platform->requestClose(id);
};
platform->setInputCallbacks(winId, icb);
```

### 4.3. Ручка окна для Vulkan и цикл событий

```cpp
void* windowHandle = platform->getNativeHandle(winId);  // например GLFWwindow* для surface

std::vector<const char*> vulkanExts = platform->getRequiredVulkanExtensions();

while (platform->anyWindowOpen()) {
    platform->pollEvents();
    // ... рендер ...
}

platform->destroyWindow(winId);
platform->terminate();
```

---

## 5. GPU: Vulkan и GpuContext

Подключение: `#include "gpu/gpu.hpp"` или `vapi.hpp`. Для рендера нужны device и swapchain.

### 5.1. Инициализация GpuContext (окно + swapchain)

```cpp
#include "gpu/gpu_context.hpp"

vapi::GpuContext gpu;
vapi::GpuContextConfig cfg;

cfg.instance.appName = "MyApp";
cfg.instance.requiredExtensions = platform->getRequiredVulkanExtensions();
cfg.windowHandle = platform->getNativeHandle(winId);
cfg.swapchainWidth = 1280;
cfg.swapchainHeight = 720;
cfg.createSwapchain = true;
cfg.vsync = true;

if (!gpu.init(cfg).has_value()) {
    std::fprintf(stderr, "GPU init failed.\n");
    return 1;
}

VkDevice dev = gpu.device()->device();
VkSwapchainKHR sw = gpu.swapchain()->handle();
```

### 5.2. Resize swapchain

При изменении размера окна (например в `onFramebufferResize`):

```cpp
auto state = platform->getWindowState(winId);
gpu.recreateSwapchain(state.framebufferWidth, state.framebufferHeight);
// После этого вызвать renderCtx.handleResize(state.framebufferWidth, state.framebufferHeight);
```

### 5.3. Ручная работа с Instance и Device (без GpuContext)

```cpp
#include "gpu/vk_instance.hpp"
#include "gpu/vk_device.hpp"
#include "gpu/vk_surface.hpp"

vapi::VkInstanceWrapper instance;
vapi::InstanceConfig instCfg;
instCfg.appName = "MyApp";
instCfg.requiredExtensions = platform->getRequiredVulkanExtensions();
instCfg.enableValidation = true;
if (!instance.create(instCfg).has_value()) return 1;

vapi::VkSurfaceWrapper surface;
if (!surface.create(instance.handle(), (GLFWwindow*)windowHandle).has_value()) return 1;

vapi::VkDeviceWrapper device;
vapi::DeviceConfig devCfg;
devCfg.instance = instance.handle();
devCfg.surface = surface.handle();
if (!device.create(devCfg).has_value()) return 1;

VkQueue graphicsQueue = device.graphicsQueue();
VkQueue presentQueue = device.presentQueue();
```

---

## 6. Render: шейдеры, пайплайны, RenderContext

Подключение: `#include "render/render.hpp"`. Требуется Vulkan 1.3 для dynamic rendering.

### 6.1. Загрузка шейдеров

```cpp
#include "render/shader.hpp"

vapi::ShaderModule vert, frag;
vert.loadFromFile(gpu.device()->device(), "shaders/vert.spv", vapi::ShaderStage::Vertex);
frag.loadFromFile(gpu.device()->device(), "shaders/frag.spv", vapi::ShaderStage::Fragment);

VkPipelineShaderStageCreateInfo vs = vert.stageInfo("main");
VkPipelineShaderStageCreateInfo fs = frag.stageInfo("main");
```

### 6.2. Сборка графического пайплайна

```cpp
#include "render/vertex.hpp"
#include "render/pipeline_builder.hpp"

vapi::GraphicsPipelineBuilder builder;
builder
    .addShaderStage(vert.stageInfo("main"))
    .addShaderStage(frag.stageInfo("main"))
    .setVertexInput<vapi::VertexPC>()
    .setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    .setDepthTest(true, true, VK_COMPARE_OP_LESS)
    .setAlphaBlending()
    .addDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
    .addDynamicState(VK_DYNAMIC_STATE_SCISSOR)
    .setColorFormat(VK_FORMAT_B8G8R8A8_SRGB)
    .setDepthFormat(VK_FORMAT_D32_SFLOAT);

VkPipelineLayout layout = VK_NULL_HANDLE;
auto res = builder.build(gpu.device()->device(), layout);
if (!res) {
    std::fprintf(stderr, "Pipeline: %.*s\n",
                 (int)vapi::errorMessage(res.error()).size(),
                 vapi::errorMessage(res.error()).data());
    return;
}
VkPipeline pipeline = *res;
```

### 6.3. RenderContext: цикл кадра

```cpp
#include "render/render_context.hpp"
#include "gpu/vk_base.hpp"

vapi::RenderContext renderCtx;
renderCtx.init(gpu.device(), gpu.swapchain(), {});

// Один кадр
auto frameRes = renderCtx.beginFrame();
if (!frameRes) {
    if (renderCtx.needsResize()) {
        auto state = platform->getWindowState(winId);
        gpu.recreateSwapchain(state.framebufferWidth, state.framebufferHeight);
        renderCtx.handleResize(state.framebufferWidth, state.framebufferHeight);
    }
    continue;
}
vapi::FrameData frame = *frameRes;

vapi::color4 clearColor(0.1f, 0.1f, 0.15f, 1.f);
renderCtx.beginRendering(frame, vapi::toVkClearColor(clearColor));
renderCtx.setFullViewport(frame);

vkCmdBindPipeline(frame.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
vkCmdBindVertexBuffers(frame.cmd, 0, 1, &vertexBuffer, &offset);
vkCmdDraw(frame.cmd, vertexCount, 1, 0, 0);

renderCtx.endRendering(frame);
if (!renderCtx.endFrame(frame).has_value())
    break;  // например swapchain out of date

// В конце приложения
renderCtx.shutdown();
```

---

## 7. Resource: буферы, изображения, дескрипторы, staging

Подключение: `#include "resource/resource.hpp"`. Нужен `VkDeviceWrapper` (например из `GpuContext`).

### 7.1. BufferManager

```cpp
#include "resource/resource.hpp"

vapi::BufferManager bufMgr;
bufMgr.init(gpu.device());

vapi::BufferDesc desc;
desc.size = vertices.size() * sizeof(Vertex);
desc.usage = vapi::BufferUsage::Vertex | vapi::BufferUsage::TransferDst;
desc.memoryUsage = vapi::MemoryUsage::GpuOnly;

auto id = bufMgr.createBuffer(desc);
if (!id) { /* handle error */ return; }
VkBuffer vkBuf = bufMgr.handle(*id);

// Для CPU-доступного буфера (uniform, staging)
desc.memoryUsage = vapi::MemoryUsage::CpuToGpu;
auto ubId = bufMgr.createBuffer(desc);
void* ptr = bufMgr.map(*ubId, 0, VK_WHOLE_SIZE);
memcpy(ptr, &uniformData, sizeof(uniformData));
bufMgr.flush(*ubId, 0, sizeof(uniformData));

// Освобождение
bufMgr.destroyBuffer(*id);
bufMgr.shutdown();
```

### 7.2. ImageManager и SamplerCache

```cpp
vapi::ImageManager imgMgr;
imgMgr.init(gpu.device());

vapi::ImageDesc idesc;
idesc.width = 1024;
idesc.height = 1024;
idesc.format = VK_FORMAT_R8G8B8A8_SRGB;
idesc.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
auto imgId = imgMgr.createImage(idesc);
VkImageView view = imgMgr.view(*imgId);

auto depthId = imgMgr.createDepthImage(800, 600);

vapi::SamplerCache samplers;
samplers.init(gpu.device());
vapi::SamplerDesc sdesc;
sdesc.magFilter = sdesc.minFilter = vapi::FilterMode::Linear;
sdesc.wrapU = sdesc.wrapV = vapi::WrapMode::Repeat;
VkSampler samp = *samplers.getOrCreate(sdesc);
```

### 7.3. DescriptorManager

```cpp
vapi::DescriptorManager descMgr;
descMgr.init(gpu.device());

vapi::DescriptorLayoutDesc layoutDesc;
layoutDesc.bindings = {
    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT}
};
auto layoutId = descMgr.createLayout(layoutDesc);
descMgr.createPool({
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
}, 10);

auto set = descMgr.allocateSet(*layoutId);
descMgr.writeBuffer(*set, 0, uniformBuffer, sizeof(UniformData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
descMgr.writeImage(*set, 1, imgMgr.view(texId), samp, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
```

### 7.4. StagingManager (загрузка вершин и текстур на GPU)

```cpp
vapi::StagingManager staging;
staging.init(gpu.device(), &renderCtx.commandManager());

staging.uploadBuffer(bufMgr.handle(*vbId), std::as_bytes(std::span(vertices)));
staging.uploadImage(textureImage, 256, 256, std::as_bytes(std::span(pixels)));

// Обновление уже загруженной текстуры (например следующий кадр видео)
staging.updateImage(textureImage, 256, 256, std::as_bytes(std::span(newPixels)));

staging.shutdown();
```

---

## 8. Font: шрифты и отрисовка текста

Модуль font не зависит от GPU: даёт метрики, layout и геометрию (вершины + атлас). Отрисовку выполняет вызывающий код или FontRenderApp.

### 8.1. TextPainter (измерение и раскладка)

```cpp
#include "font/text_painter.hpp"
#include "font/font.hpp"

vapi::FontService fonts;
auto idRes = fonts.loadFont("/path/to/font.ttf");
if (!idRes) { /* fallback */ return; }
vapi::IFontSource* font = fonts.getFont(*idRes);

vapi::TextPainter painter(*font);
painter.setPixelHeight(24.f);

vapi::TextMeasure measure = painter.measureText("Hello, World!");
// measure.width, measure.height

vapi::TextGeometry geom;
painter.paint("Hello, World!", 0.f, 0.f, vapi::color4::white(), geom);
// geom.vertices, geom.atlasWidth, geom.atlasHeight, atlas RGBA
```

### 8.2. FontDrawList (батчинг, один draw call)

```cpp
#include "font/font_draw_list.hpp"

vapi::FontDrawList list;
list.setFont(font, 24.f);  // IFontSource* + pixel height (нет отдельного setPixelHeight)
list.addText(50.f, 80.f, "First line", vapi::color4(1.f, 1.f, 1.f, 1.f));
list.addText(50.f, 110.f, "Second line", vapi::color4(0.9f, 0.9f, 1.f, 1.f));

std::vector<vapi::FontVertex> vertices = list.getVertices();
std::vector<vapi::u8> atlasRgba = list.getAtlasRgba();
// Загрузить vertices в vertex buffer, atlasRgba в текстуру и отрисовать одним vkCmdDraw
```

При смене контента: `list.clear()` и снова `addText` / `addImage`, затем снова `getVertices()`, `getAtlasRgba()`.

---

## 9. FontRenderApp: окно с текстом «из коробки»

Минимальный API: одно окно, Vulkan, шрифт, список строк; цикл событий и отрисовка инкапсулированы.

### 9.1. Жизненный цикл

1. **init(width, height, title, shaderDir)** — платформа, окно, GPU, контекст рендера.
2. **setFont(fontNameOrPath)** — опционально; при ошибке используется fallback.
3. **setFontSize(pixelHeight)** — опционально; по умолчанию 48.
4. **addText(x, y, text, color)** — одна или несколько строк; (x, y) — верх левого угла.
5. **run()** — блокирующий цикл до закрытия окна; возврат: `kRunSuccess` (0) или `kRunError` (1) и др.

### 9.2. Пример

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    auto initRes = app.init(800, 600, "VAPI Render", "shaders");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    auto fontRes = app.setFont("Arial");
    if (!fontRes)
        std::fprintf(stderr, "setFont failed (using fallback): %s\n",
                     vapi::errorMessage(fontRes.error()).data());
    app.setFontSize(24.f);

    const vapi::f32 x = 50.f, topY = 80.f;
    vapi::f32 lineH = app.getLineHeight();

    app.addText(x, topY, "Hello, Arial!", vapi::color4(1.f, 1.f, 1.f, 1.f));
    app.addText(x, topY + lineH, "Second line", vapi::color4(0.85f, 0.9f, 1.f, 1.f));
    app.addText(x, topY + lineH * 2.f, u8"Привет, мир!", vapi::color4(0.9f, 0.95f, 0.9f, 1.f));

    return app.run();
}
```

---

## 10. Полные примеры приложений

### 10.1. Примеры приложений (FontRenderApp + IPlatform)

Все примеры используют только API **FontRenderApp** и **IPlatform** (без прямого Vulkan/GLFW в коде):

- [examples/calculator_example.cpp](../examples/calculator_example.cpp) — калькулятор (сетка кнопок, дисплей)
- [examples/counter_example.cpp](../examples/counter_example.cpp) — счётчик (кнопка «Click»)
- [examples/stopwatch_example.cpp](../examples/stopwatch_example.cpp) — секундомер (Start/Stop/Reset)

См. раздел 9 (FontRenderApp) и раздел 1 (требования).

### 10.2. Окно с текстом (FontRenderApp)

Тот же подход: [examples/calculator_example.cpp](../examples/calculator_example.cpp) — полный пример с циклом, вводом и отрисовкой.

### 10.3. Кастомный цикл: Platform + GpuContext + RenderContext

Порядок:

1. `createPlatform()` → `init()` → `createWindow(config)` → `getNativeHandle(winId)`.
2. `GpuContext::init(config)` с `windowHandle` и размерами swapchain.
3. `RenderContext::init(device, swapchain, config)`.
4. При необходимости: BufferManager, ImageManager, DescriptorManager, StagingManager; загрузка геометрии из FontDrawList.
5. Цикл: `beginFrame()` → `beginRendering(frame, clearColor)` → set viewport → bind pipeline/descriptors → draw → `endRendering(frame)` → `endFrame(frame)`; при resize — `recreateSwapchain` и `handleResize`.

Подробный пример цикла см. в [detail_docs/render.md](detail_docs/render.md), раздел «Full render loop example».

---

## 11. Ссылки на детальную справку

| Тема | Документ |
|------|----------|
| Архитектура слоёв | [architecture.md](architecture.md) |
| Краткая таблица классов и методов | [quick_reference.md](quick_reference.md) |
| Core (типы, ошибки, файлы, логгер, интерфейсы) | [detail_docs/core.md](detail_docs/core.md) |
| Platform (окно, ввод, мониторы) | [detail_docs/platform.md](detail_docs/platform.md) |
| GPU (GpuContext, instance, device, swapchain, sync) | [detail_docs/gpu.md](detail_docs/gpu.md) |
| Render (вершины, шейдеры, пайплайны, RenderContext) | [detail_docs/render.md](detail_docs/render.md) |
| Resource (буферы, изображения, дескрипторы, staging) | [detail_docs/resource.md](detail_docs/resource.md) |
| UI-текст (FontDrawList, TextPainter, пайплайн) | [font_ui_text.md](font_ui_text.md) |
| Ограничения и узкие места | [shortcomings.md](shortcomings.md) |
| Тестирование | [testing.md](testing.md) |
| Pipeline cache (Vulkan) | [pipeline_cache.md](pipeline_cache.md) |

Пути к заголовкам в коде — относительно каталога `include/` проекта. Примеры — в [examples/](../examples/).
