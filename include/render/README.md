# Render module

Vertex layouts, shader modules, pipeline builders, and render context.

- **vertex.hpp** — `VertexP`, `VertexPC`, `VertexPNU`, `VertexFull`, `VertexInputState<V>`.
- **shader.hpp/cpp** — `ShaderModule` (load SPIR-V), `ShaderProgram`, `ShaderStage`.
- **pipeline_builder.hpp/cpp** — `GraphicsPipelineBuilder`, `ComputePipelineBuilder` (fluent API). *Requires Vulkan 1.3 (dynamic rendering); currently excluded from default build.*
- **render_context.hpp/cpp** — `RenderContext`: `beginFrame`/`endFrame`, dynamic rendering. *Requires Vulkan 1.3; excluded from default build.*

Include: `#include "render/render.hpp"` or individual headers. Link: `vapi_render` (and `vapi_gpu`).
