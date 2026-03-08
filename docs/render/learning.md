# Render Module — Learning Guide

This guide explains the VAPI render module: vertex types, shaders, pipeline building, and the render context. All examples use the C++ API.

---

## 1. Vertex types

The render module provides standard vertex layouts in `render/vertex.hpp`:

- **VertexP** — position only (`vec3`)
- **VertexPC** — position + color (`vec3` + `color4`)
- **VertexPNU** — position + normal + UV (`vec3` + `vec3` + `vec2`)
- **VertexFull** — position, normal, UV, tangent, color

Each type exposes static `binding(bind)` and `attributes(bind)` for pipeline vertex input.

### Example: using a vertex type

```cpp
#include "render/vertex.hpp"
#include "render/pipeline_builder.hpp"

using namespace vapi;

// Build pipeline with VertexPC (position + color)
GraphicsPipelineBuilder builder;
builder.setVertexInput<VertexPC>();
// ... set shaders, topology, etc.
```

### Example: custom vertex and VertexInputState

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

## 2. Shaders

`ShaderModule` loads SPIR-V from file or memory and provides `VkPipelineShaderStageCreateInfo`.

### Example: load vertex and fragment shaders

```cpp
#include "render/shader.hpp"
#include "core/error.hpp"

VkDevice device = /* from GpuContext or VkDeviceWrapper */;

vapi::ShaderModule vertShader, fragShader;
if (!vertShader.loadFromFile(device, "shaders/vert.spv", vapi::ShaderStage::Vertex).has_value())
    return;
if (!fragShader.loadFromFile(device, "shaders/frag.spv", vapi::ShaderStage::Fragment).has_value())
    return;

auto vertStage = vertShader.stageInfo("main");
auto fragStage = fragShader.stageInfo("main");

// Use in GraphicsPipelineBuilder
vapi::GraphicsPipelineBuilder builder;
builder.addShaderStage(vertStage).addShaderStage(fragStage);
```

### Example: load from memory (SPIR-V bytes)

```cpp
std::vector<uint32_t> spirv = loadSpirvFromFile("shaders/vert.spv");
vapi::ShaderModule shader;
shader.loadFromMemory(device, spirv, vapi::ShaderStage::Vertex);
auto stageInfo = shader.stageInfo("main");
```

---

## 3. Graphics pipeline builder

`GraphicsPipelineBuilder` configures and builds a `VkPipeline` (and optionally `VkPipelineLayout`).

### Example: minimal triangle pipeline

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
    // handle error: res.error(), vapi::errorMessage(res.error())
    return;
}
VkPipeline pipeline = *res;
```

### Example: depth and dynamic viewport

```cpp
builder
    .setDepthTest(true, true, VK_COMPARE_OP_LESS)
    .setDepthFormat(VK_FORMAT_D32_SFLOAT)
    .addDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
    .addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
```

---

## 4. Render context (frame loop)

`RenderContext` manages per-frame sync, command buffers, and dynamic rendering (Vulkan 1.3). It works with `VkDeviceWrapper` and `VkSwapchainWrapper`.

### Example: init and main loop

```cpp
#include "render/render_context.hpp"
#include "gpu/gpu_context.hpp"
#include "gpu/vk_base.hpp"

// Assume GpuContext is initialized with swapchain
vapi::GpuContext gpu;
// ... gpu.init(config) with windowHandle and createSwapchain = true

vapi::RenderContext renderCtx;
vapi::RenderContextConfig rcc;
rcc.framesInFlight = 2;
rcc.vsync = true;
if (!renderCtx.init(gpu.device(), gpu.swapchain(), rcc).has_value())
    return;

// Main loop
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
    // vkCmdBindPipeline(frame.cmd, ...), vkCmdDraw(...), etc.
    renderCtx.endRendering(frame);

    if (!renderCtx.endFrame(frame).has_value())
        break;
}
renderCtx.shutdown();
```

### Example: beginFrame / endFrame and resize

```cpp
auto frame = renderCtx.beginFrame();
if (!frame) {
    if (frame.error() == gpu_errors::SwapchainOutOfDate || renderCtx.needsResize())
        renderCtx.handleResize(width, height);
    continue;
}
// ... record commands into (*frame).cmd ...
renderCtx.endFrame(*frame);
```

---

## 5. Error handling

Use `Result<T>` and `errorMessage(Error)` for human-readable messages:

```cpp
#include "core/error.hpp"

auto r = builder.build(device);
if (!r) {
    std::fprintf(stderr, "Pipeline build failed: %.*s\n",
        (int)vapi::errorMessage(r.error()).size(),
        vapi::errorMessage(r.error()).data());
    return;
}
VkPipeline pipeline = *r;
```

---

## 6. Quick start with GpuContext

Use the GPU facade to init instance, surface, device, and swapchain in one call, then pass `device()` and `swapchain()` into `RenderContext`:

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
// ... main loop with renderCtx.beginFrame() / endFrame()
```

On window resize, call `gpu.recreateSwapchain(fbWidth, fbHeight)` and optionally `renderCtx.handleResize(fbWidth, fbHeight)`.

---

## 7. Module include

Single include for the render module:

```cpp
#include "render/render.hpp"
```

Root include (core + platform + GPU, no render/resource):

```cpp
#include "vapi.hpp"
```

This pulls in `core/types.hpp`, `core/error.hpp`, `core/logger.hpp`, `core/file_utils.hpp`, `platform/platform.hpp`, and `gpu/gpu.hpp`. Add `render/render.hpp` or `resource/resource.hpp` when needed.
