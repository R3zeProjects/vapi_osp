# Модуль Render — руководство по обучению

В этом руководстве описаны типы вершин, шейдеры, построение пайплайнов и контекст рендера. Все примеры используют C++ API.

---

## 1. Типы вершин

В `render/vertex.hpp` определены стандартные форматы вершин:

- **VertexP** — только позиция (`vec3`)
- **VertexPC** — позиция и цвет (`vec3` + `color4`)
- **VertexPNU** — позиция, нормаль, UV (`vec3` + `vec3` + `vec2`)
- **VertexFull** — позиция, нормаль, UV, тангент, цвет

У каждого типа есть статические методы `binding(bind)` и `attributes(bind)` для настройки вершинного ввода пайплайна.

### Пример: использование типа вершины

```cpp
#include "render/vertex.hpp"
#include "render/pipeline_builder.hpp"

using namespace vapi;

// Сборка пайплайна с VertexPC (позиция + цвет)
GraphicsPipelineBuilder builder;
builder.setVertexInput<VertexPC>();
// ... задать шейдеры, топологию и т.д.
```

### Пример: свой тип вершины и VertexInputState

```cpp
#include "render/vertex.hpp"

struct MyVertex {
    vec3 pos;
    vec2 uv;
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return { bind, sizeof(MyVertex), VK_VERTEX_INPUT_RATE_VERTEX };
    }
    static std::array<VkVertexInputAttributeDescription, 2> attributes(u32 bind = 0) {
        return {{
            { 0, bind, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MyVertex, pos) },
            { 1, bind, VK_FORMAT_R32G32_SFLOAT,    offsetof(MyVertex, uv)  },
        }};
    }
};

vapi::VertexInputState<MyVertex> vertexState(0);
auto ci = vertexState.createInfo();
```

---

## 2. Шейдеры

`ShaderModule` загружает SPIR-V из файла или памяти и даёт `VkPipelineShaderStageCreateInfo`.

### Пример: загрузка вершинного и фрагментного шейдеров

```cpp
#include "render/shader.hpp"
#include "core/error.hpp"

VkDevice device = /* из GpuContext или VkDeviceWrapper */;

vapi::ShaderModule vertShader, fragShader;
if (!vertShader.loadFromFile(device, "shaders/vert.spv", vapi::ShaderStage::Vertex).has_value())
    return;
if (!fragShader.loadFromFile(device, "shaders/frag.spv", vapi::ShaderStage::Fragment).has_value())
    return;

auto vertStage = vertShader.stageInfo("main");
auto fragStage = fragShader.stageInfo("main");

// Использование в GraphicsPipelineBuilder
vapi::GraphicsPipelineBuilder builder;
builder.addShaderStage(vertStage).addShaderStage(fragStage);
```

### Пример: загрузка из памяти (байты SPIR-V)

```cpp
std::vector<uint32_t> spirv = loadSpirvFromFile("shaders/vert.spv");
vapi::ShaderModule shader;
shader.loadFromMemory(device, spirv, vapi::ShaderStage::Vertex);
auto stageInfo = shader.stageInfo("main");
```

---

## 3. Построитель графического пайплайна

`GraphicsPipelineBuilder` настраивает и создаёт `VkPipeline` (и при необходимости `VkPipelineLayout`).

### Пример: минимальный пайплайн для треугольника

```cpp
#include "render/vertex.hpp"
#include "render/shader.hpp"
#include "render/pipeline_builder.hpp"
#include "gpu/vk_base.hpp"

vapi::ShaderModule vert, frag;
vert.loadFromFile(device, "shaders/vert.spv", vapi::ShaderStage::Vertex);
frag.loadFromFile(device, "shaders/frag.spv", vapi::ShaderStage::Fragment);

vapi::GraphicsPipelineBuilder builder;
builder
    .addShaderStage(vert.stageInfo("main"))
    .addShaderStage(frag.stageInfo("main"))
    .setVertexInput<vapi::VertexPC>()
    .setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    .setAlphaBlending()
    .setColorFormat(VK_FORMAT_B8G8R8A8_SRGB);

VkPipelineLayout layout = VK_NULL_HANDLE;
auto res = builder.build(device, layout);
if (!res) {
    // обработка ошибки: res.error(), vapi::errorMessage(res.error())
    return;
}
VkPipeline pipeline = *res;
```

### Пример: глубина и динамический вьюпорт

```cpp
builder
    .setDepthTest(true, true, VK_COMPARE_OP_LESS)
    .setDepthFormat(VK_FORMAT_D32_SFLOAT)
    .addDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
    .addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
```

---

## 4. Контекст рендера (цикл кадров)

`RenderContext` управляет синхронизацией по кадрам, командными буферами и динамическим рендерингом (Vulkan 1.3). Работает с `VkDeviceWrapper` и `VkSwapchainWrapper`.

### Пример: инициализация и главный цикл

```cpp
#include "render/render_context.hpp"
#include "gpu/gpu_context.hpp"
#include "gpu/vk_base.hpp"

// GpuContext инициализирован с swapchain
vapi::GpuContext gpu;
// ... gpu.init(config) с windowHandle и createSwapchain = true

vapi::RenderContext renderCtx;
vapi::RenderContextConfig rcc;
rcc.framesInFlight = 2;
rcc.vsync = true;
if (!renderCtx.init(gpu.device(), gpu.swapchain(), rcc).has_value())
    return;

// Главный цикл
while (running) {
    auto frameRes = renderCtx.beginFrame();
    if (!frameRes) {
        if (renderCtx.needsResize()) {
            renderCtx.handleResize(fbWidth, fbHeight);
            gpu.recreateSwapchain(fbWidth, fbHeight);
        }
        continue;
    }
    vapi::FrameData frame = *frameRes;

    renderCtx.beginRendering(frame, vapi::toVkClearColor(vapi::color4{0.1f, 0.1f, 0.15f, 1.f}));
    renderCtx.setFullViewport(frame);
    // vkCmdBindPipeline(frame.cmd, ...), vkCmdDraw(...) и т.д.
    renderCtx.endRendering(frame);

    if (!renderCtx.endFrame(frame).has_value())
        break;
}
renderCtx.shutdown();
```

### Пример: beginFrame / endFrame и изменение размера

```cpp
auto frame = renderCtx.beginFrame();
if (!frame) {
    if (frame.error() == gpu_errors::SwapchainOutOfDate || renderCtx.needsResize())
        renderCtx.handleResize(width, height);
    continue;
}
// ... запись команд в (*frame).cmd ...
renderCtx.endFrame(*frame);
```

---

## 5. Обработка ошибок

Используйте `Result<T>` и `errorMessage(Error)` для человекочитаемых сообщений:

```cpp
#include "core/error.hpp"

auto r = builder.build(device);
if (!r) {
    std::fprintf(stderr, "Ошибка сборки пайплайна: %.*s\n",
        (int)vapi::errorMessage(r.error()).size(),
        vapi::errorMessage(r.error()).data());
    return;
}
VkPipeline pipeline = *r;
```

---

## 6. Быстрый старт с GpuContext

Используйте фасад GPU для инициализации instance, surface, device и swapchain одним вызовом, затем передайте `device()` и `swapchain()` в `RenderContext`:

```cpp
#include "vapi.hpp"
#include "render/render.hpp"

vapi::GpuContext gpu;
vapi::GpuContextConfig cfg;
cfg.instance.appName = "MyApp";
cfg.instance.requiredExtensions = platform->getRequiredVulkanExtensions();
cfg.windowHandle = platform->getNativeHandle(winId);
cfg.swapchainWidth = 1280;
cfg.swapchainHeight = 720;
cfg.createSwapchain = true;
cfg.vsync = true;

if (!gpu.init(cfg).has_value())
    return;
vapi::RenderContext renderCtx;
renderCtx.init(gpu.device(), gpu.swapchain(), {});
// ... главный цикл с renderCtx.beginFrame() / endFrame()
```

При изменении размера окна вызовите `gpu.recreateSwapchain(fbWidth, fbHeight)` и при необходимости `renderCtx.handleResize(fbWidth, fbHeight)`.

---

## 7. Подключение модуля

Один инклюд для всего модуля рендера:

```cpp
#include "render/render.hpp"
```

Корневой инклюд (core + platform + GPU, без render/resource):

```cpp
#include "vapi.hpp"
```

Он подключает `core/types.hpp`, `core/error.hpp`, `core/logger.hpp`, `core/file_utils.hpp`, `platform/platform.hpp` и `gpu/gpu.hpp`. При необходимости добавляйте `render/render.hpp` или `resource/resource.hpp`. Подробное описание каждого элемента API см. в **docs/detail_docs/**.
