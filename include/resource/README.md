# Resource module

GPU buffers, images, samplers, descriptors, and staging uploads.

- **resource_types.hpp** — `GpuBuffer`, `GpuImage`, `ImageDesc`, `GpuSampler`, `SamplerDesc`, descriptor types, `BufferId`/`ImageId`/etc.
- **buffer_manager.hpp/cpp** — `BufferManager`: create/destroy buffers, map, write, flush.
- **image_manager.hpp/cpp** — `ImageManager`: create image/view, depth image.
- **descriptor_manager.hpp/cpp** — `DescriptorManager`: layouts, pool, allocate sets, write buffer/image.
- **sampler_cache.hpp/cpp** — `SamplerCache`: get or create `VkSampler` by `SamplerDesc`.
- **staging.hpp/cpp** — `StagingManager`: upload buffer/image CPU→GPU.

Uses `core/interfaces/i_resource.hpp` (`BufferDesc`, `MemoryUsage`, `BufferUsage`). Include: `#include "resource/resource.hpp"`. Link: `vapi_resource` (and `vapi_gpu`).
