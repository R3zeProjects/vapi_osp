# Render module — Detailed API

Include: `#include "render/render.hpp"` (vertex, shader, pipeline_builder, render_context). Requires Vulkan 1.3 for dynamic rendering (pipeline builder color/depth formats, RenderContext).

---

## 1. Vertex types (vertex.hpp)

Predefined vertex layouts with static `binding(bind)` and `attributes(bind)` for Vulkan vertex input.

| Type | Fields | Use case |
|------|--------|----------|
| **VertexP** | `vec3 position` | Position only |
| **VertexPC** | `vec3 position`, `color4 color` | Colored mesh |
| **VertexPNU** | `vec3 position`, `vec3 normal`, `vec2 uv` | Lit + UV |
| **VertexFull** | position, normal, uv, tangent, color | Full PBR-style |

Each returns:
- **binding(bind)** → `VkVertexInputBindingDescription` (stride, input rate).
- **attributes(bind)** → array of `VkVertexInputAttributeDescription` (location, format, offset).

### VertexInputState<V>

Template helper to build vertex input state from a vertex type:

```cpp
vapi::VertexInputState<vapi::VertexPC> state(0);
VkPipelineVertexInputStateCreateInfo ci = state.createInfo();
```

### Custom vertex type

Provide static `binding(u32 bind)` and `attributes(u32 bind)`; use with `VertexInputState<MyVertex>` or pass to pipeline builder manually.

```cpp
struct MyVertex {
    vapi::vec3 pos;
    vapi::vec2 uv;
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
```

---

## 2. ShaderModule (shader.hpp)

RAII wrapper for `VkShaderModule`. Loads SPIR-V and provides `VkPipelineShaderStageCreateInfo`.

### Methods

- **loadFromFile(device, path, stage)** → `Result<void>`.
- **loadFromMemory(device, spirv_span, stage)** → `Result<void>`.
- **destroy()**.
- **handle()** → `VkShaderModule`.
- **stage()** → `ShaderStage` (Vertex, Fragment, Compute, Geometry, TessControl, TessEval).
- **vkStage()** → `VkShaderStageFlagBits`.
- **stageInfo(entryPoint)** → `VkPipelineShaderStageCreateInfo` (default entry `"main"`).
- **setName(name)** / **name()** for debugging.

### Example

```cpp
vapi::ShaderModule vert, frag;
vert.loadFromFile(device, "shaders/vert.spv", vapi::ShaderStage::Vertex);
frag.loadFromFile(device, "shaders/frag.spv", vapi::ShaderStage::Fragment);
auto vs = vert.stageInfo("main");
auto fs = frag.stageInfo("main");
// Pass vs, fs to GraphicsPipelineBuilder::addShaderStage
```

---

## 3. GraphicsPipelineBuilder (pipeline_builder.hpp)

Fluent builder for graphics pipelines (Vulkan 1.3 dynamic rendering). Creates `VkPipeline` and optionally `VkPipelineLayout`.

### Shaders

- **addShaderStage(VkPipelineShaderStageCreateInfo)** — append one stage.
- **setShaderStages(vector)** — set all stages (e.g. vertex + fragment).

### Vertex input

- **setVertexInput(bindings, bindingCount, attributes, attributeCount)** — raw Vulkan structs.
- **setVertexInput<V>()** — from vertex type (e.g. `setVertexInput<VertexPC>()`).

### Topology and rasterization

- **setTopology(VkPrimitiveTopology)** — default triangle list.
- **setViewportCount(count)**.
- **setPolygonMode(mode)**, **setCullMode(cull, frontFace)**, **setLineWidth(w)**, **setDepthClamp(enable)**.

### Depth / stencil

- **setDepthTest(enable, write, compareOp)**.
- **setDepthBounds(enable, min, max)**, **setStencilTest(enable)**.

### Blending

- **setBlendAttachment(state)** — raw attachment.
- **setAlphaBlending()** — standard alpha blend preset.
- **setNoBlending()** — opaque.

### Multisampling and dynamic state

- **setMultisample(VkSampleCountFlagBits)**.
- **addDynamicState(VkDynamicState)** — e.g. `VK_DYNAMIC_STATE_VIEWPORT`, `VK_DYNAMIC_STATE_SCISSOR`.

### Layout

- **setPipelineLayout(VkPipelineLayout)**.
- **setDescriptorSetLayouts(layouts, count)**.
- **setPushConstantRange(stages, offset, size)**.

### Render targets (dynamic rendering)

- **setColorFormats(formats, count)** / **setColorFormat(format)**.
- **setDepthFormat(format)** / **setStencilFormat(format)**.

### Build

- **build(device)** → `Result<VkPipeline>` (uses internal layout if not set).
- **build(device, outLayout)** → `Result<VkPipeline>` and fills `VkPipelineLayout&` (creates layout from descriptor set layouts and push constants if not set).

### Full pipeline example

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
    .setDepthTest(true, true, VK_COMPARE_OP_LESS)
    .setAlphaBlending()
    .addDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
    .addDynamicState(VK_DYNAMIC_STATE_SCISSOR)
    .setColorFormat(VK_FORMAT_B8G8R8A8_SRGB)
    .setDepthFormat(VK_FORMAT_D32_SFLOAT);

VkPipelineLayout layout = VK_NULL_HANDLE;
auto res = builder.build(device, layout);
if (!res) {
    std::fprintf(stderr, "Pipeline: %.*s\n", (int)vapi::errorMessage(res.error()).size(),
                 vapi::errorMessage(res.error()).data());
    return;
}
VkPipeline pipeline = *res;
// Use pipeline and layout for drawing; destroy when done
```

---

## 4. ComputePipelineBuilder (pipeline_builder.hpp)

For compute-only pipelines.

- **setShader(VkPipelineShaderStageCreateInfo)** — single compute stage.
- **setPipelineLayout** / **setDescriptorSetLayouts** / **setPushConstantRange** — same idea as graphics.
- **build(device)** / **build(device, outLayout)** → `Result<VkPipeline>`.

```cpp
vapi::ShaderModule comp;
comp.loadFromFile(device, "shaders/comp.spv", vapi::ShaderStage::Compute);
vapi::ComputePipelineBuilder builder;
builder.setShader(comp.stageInfo("main"));
// set layouts / push constants
auto pipe = builder.build(device, layout);
```

---

## 5. RenderContext (render_context.hpp)

Manages per-frame sync (semaphores, fences), command buffers, and Vulkan 1.3 dynamic rendering. Works with `VkDeviceWrapper` and `VkSwapchainWrapper` (e.g. from **GpuContext**).

### Minimal init and one frame

```cpp
#include "render/render_context.hpp"
#include "gpu/gpu_context.hpp"
#include "gpu/vk_base.hpp"

vapi::RenderContext renderCtx;
renderCtx.init(gpu.device(), gpu.swapchain(), {});

// One frame
auto frameRes = renderCtx.beginFrame();
if (!frameRes) { /* skip or handle resize */ return; }
vapi::FrameData frame = *frameRes;
renderCtx.beginRendering(frame, vapi::toVkClearColor(vapi::color4(0.1f, 0.1f, 0.15f, 1.f)));
renderCtx.setFullViewport(frame);
// vkCmdBindPipeline(frame.cmd, ...); vkCmdDraw(...);
renderCtx.endRendering(frame);
renderCtx.endFrame(frame);
```

### FrameData

- **frameIndex**, **imageIndex** — current frame and swapchain image.
- **cmd** — `VkCommandBuffer` for this frame.
- **extent**, **colorFormat** — from swapchain.

### RenderContextConfig

- **framesInFlight** (default 2), **vsync** (stored for reference; swapchain already created with vsync).

### Init and shutdown

- **init(device, swapchain, config)** → `Result<void>`.
- **shutdown()**.

### Frame loop

- **beginFrame()** → `Result<FrameData>`. May fail (e.g. swapchain out of date); then check **needsResize()** and call **handleResize** / **recreateSwapchain** as needed.
- **endFrame(frame)** → `Result<void>` (submit and present). May return swapchain-out-of-date.

### Rendering

- **beginRendering(frame, clearColor)** — start dynamic rendering with clear color.
- **beginRendering(frame, clearColor, depthView, depthClear)** — with depth attachment.
- **endRendering(frame)** — end render pass.
- **setFullViewport(cmd)** / **setFullViewport(frame)** / **setFullViewport(cmd, extent)** — set viewport and scissor to full extent.

### Resize and accessors

- **handleResize(framebufferWidth, framebufferHeight)** → `Result<void>` — recreates swapchain-dependent state (call after **GpuContext::recreateSwapchain** with same dimensions).
- **currentFrame()**, **framesInFlight()**, **needsResize()**, **requestResize()**.
- **syncManager()**, **commandManager()** — for advanced use.

### Full render loop example

```cpp
vapi::GpuContext gpu;
// ... gpu.init(cfg) with window and swapchain

vapi::RenderContext renderCtx;
renderCtx.init(gpu.device(), gpu.swapchain(), {});

while (running) {
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

    vapi::color4 clearColor{0.1f, 0.1f, 0.15f, 1.f};
    renderCtx.beginRendering(frame, vapi::toVkClearColor(clearColor));
    renderCtx.setFullViewport(frame);

    vkCmdBindPipeline(frame.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindVertexBuffers(frame.cmd, 0, 1, &vertexBuffer, &offset);
    vkCmdDraw(frame.cmd, vertexCount, 1, 0, 0);

    renderCtx.endRendering(frame);
    if (!renderCtx.endFrame(frame).has_value())
        break;
}
renderCtx.shutdown();
```

---

## 6. Pipeline usage summary

1. **Create shaders** — `ShaderModule::loadFromFile` (or from memory).
2. **Build pipeline** — `GraphicsPipelineBuilder` (or `ComputePipelineBuilder`): set stages, vertex input, topology, depth, blend, dynamic state, color/depth formats, then `build(device, layout)`.
3. **Init RenderContext** — `init(device, swapchain, config)`.
4. **Each frame:** `beginFrame()` → on success, `beginRendering(frame, clearColor)` → set viewport → bind pipeline and resources → draw → `endRendering(frame)` → `endFrame(frame)`. On resize, recreate swapchain and call `handleResize`.
5. **Cleanup** — destroy pipeline and layout, then `renderCtx.shutdown()`.
