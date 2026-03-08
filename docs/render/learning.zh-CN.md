# 渲染模块 — 学习指南

本文介绍 VAPI 渲染模块：顶点类型、着色器、管线构建与渲染上下文。示例均使用 C++ API。

---

## 1. 顶点类型

渲染模块在 `render/vertex.hpp` 中提供标准顶点布局：

- **VertexP** — 仅位置（`vec3`）
- **VertexPC** — 位置 + 颜色（`vec3` + `color4`）
- **VertexPNU** — 位置 + 法线 + UV（`vec3` + `vec3` + `vec2`）
- **VertexFull** — 位置、法线、UV、切线、颜色

每种类型都提供静态方法 `binding(bind)` 和 `attributes(bind)`，用于管线顶点输入。

### 示例：使用顶点类型

```cpp
#include "render/vertex.hpp"
#include "render/pipeline_builder.hpp"

using namespace vapi;

// 使用 VertexPC（位置 + 颜色）构建管线
GraphicsPipelineBuilder builder;
builder.setVertexInput<VertexPC>();
// ... 设置着色器、拓扑等
```

### 示例：自定义顶点与 VertexInputState

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

## 2. 着色器

`ShaderModule` 从文件或内存加载 SPIR-V，并提供 `VkPipelineShaderStageCreateInfo`。

### 示例：加载顶点与片段着色器

```cpp
#include "render/shader.hpp"
#include "core/error.hpp"

VkDevice device = /* 来自 GpuContext 或 VkDeviceWrapper */;

vapi::ShaderModule vertShader, fragShader;
if (!vertShader.loadFromFile(device, "shaders/vert.spv", vapi::ShaderStage::Vertex).has_value())
    return;
if (!fragShader.loadFromFile(device, "shaders/frag.spv", vapi::ShaderStage::Fragment).has_value())
    return;

auto vertStage = vertShader.stageInfo("main");
auto fragStage = fragShader.stageInfo("main");

// 在 GraphicsPipelineBuilder 中使用
vapi::GraphicsPipelineBuilder builder;
builder.addShaderStage(vertStage).addShaderStage(fragStage);
```

### 示例：从内存加载（SPIR-V 字节）

```cpp
std::vector<uint32_t> spirv = loadSpirvFromFile("shaders/vert.spv");
vapi::ShaderModule shader;
shader.loadFromMemory(device, spirv, vapi::ShaderStage::Vertex);
auto stageInfo = shader.stageInfo("main");
```

---

## 3. 图形管线构建器

`GraphicsPipelineBuilder` 配置并构建 `VkPipeline`（以及可选的 `VkPipelineLayout`）。

### 示例：最简三角形管线

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
    // 处理错误：res.error()、vapi::errorMessage(res.error())
    return;
}
VkPipeline pipeline = *res;
```

### 示例：深度与动态视口

```cpp
builder
    .setDepthTest(true, true, VK_COMPARE_OP_LESS)
    .setDepthFormat(VK_FORMAT_D32_SFLOAT)
    .addDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
    .addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
```

---

## 4. 渲染上下文（帧循环）

`RenderContext` 管理每帧同步、命令缓冲与动态渲染（Vulkan 1.3），与 `VkDeviceWrapper` 和 `VkSwapchainWrapper` 配合使用。

### 示例：初始化与主循环

```cpp
#include "render/render_context.hpp"
#include "gpu/gpu_context.hpp"
#include "gpu/vk_base.hpp"

// 假设 GpuContext 已带 swapchain 初始化
vapi::GpuContext gpu;
// ... gpu.init(config)，带 windowHandle 且 createSwapchain = true

vapi::RenderContext renderCtx;
vapi::RenderContextConfig rcc;
rcc.framesInFlight = 2;
rcc.vsync = true;
if (!renderCtx.init(gpu.device(), gpu.swapchain(), rcc).has_value())
    return;

// 主循环
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
    // vkCmdBindPipeline(frame.cmd, ...)、vkCmdDraw(...) 等
    renderCtx.endRendering(frame);

    if (!renderCtx.endFrame(frame).has_value())
        break;
}
renderCtx.shutdown();
```

### 示例：beginFrame / endFrame 与调整大小

```cpp
auto frame = renderCtx.beginFrame();
if (!frame) {
    if (frame.error() == gpu_errors::SwapchainOutOfDate || renderCtx.needsResize())
        renderCtx.handleResize(width, height);
    continue;
}
// ... 向 (*frame).cmd 录制命令 ...
renderCtx.endFrame(*frame);
```

---

## 5. 错误处理

使用 `Result<T>` 和 `errorMessage(Error)` 获取可读错误信息：

```cpp
#include "core/error.hpp"

auto r = builder.build(device);
if (!r) {
    std::fprintf(stderr, "管线构建失败: %.*s\n",
        (int)vapi::errorMessage(r.error()).size(),
        vapi::errorMessage(r.error()).data());
    return;
}
VkPipeline pipeline = *r;
```

---

## 6. 使用 GpuContext 快速上手

用 GPU 门面一次完成 instance、surface、device、swapchain 的初始化，再把 `device()` 和 `swapchain()` 传给 `RenderContext`：

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
// ... 主循环中 renderCtx.beginFrame() / endFrame()
```

窗口缩放时调用 `gpu.recreateSwapchain(fbWidth, fbHeight)`，必要时再调用 `renderCtx.handleResize(fbWidth, fbHeight)`。

---

## 7. 模块包含

渲染模块单头文件包含：

```cpp
#include "render/render.hpp"
```

根头文件（core + platform + GPU，不含 render/resource）：

```cpp
#include "vapi.hpp"
```

会引入 `core/types.hpp`、`core/error.hpp`、`core/logger.hpp`、`core/file_utils.hpp`、`platform/platform.hpp` 和 `gpu/gpu.hpp`。需要时再添加 `render/render.hpp` 或 `resource/resource.hpp`。各 API 元素的详细说明见 **docs/detail_docs/**。
