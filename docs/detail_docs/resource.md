# Resource module — Detailed API

Include: `#include "resource/resource.hpp"` (resource_types, buffer_manager, image_manager, sampler_cache, descriptor_manager, staging). Requires `VkDeviceWrapper` (from gpu module).

---

## 1. Resource types (resource_types.hpp)

### Buffer

- **GpuBuffer:** `buffer` (VkBuffer), `memory` (VkDeviceMemory), `size`, `usage`, `memoryUsage`, `mapped` (for persistent map), `valid`.
- **BufferDesc:** `size`, `usage` (BufferUsage), `memoryUsage` (MemoryUsage).
- **BufferUsage:** flags `Vertex`, `Index`, `Uniform`, `Storage`, `Indirect`, `TransferSrc`, `TransferDst` (composable with `|`).
- **MemoryUsage:** `GpuOnly`, `CpuOnly`, `CpuToGpu`, `GpuToCpu`.
- **BufferId:** `u32` handle.

### Image

- **GpuImage:** `image`, `memory`, `view` (VkImageView), `desc` (ImageDesc), `currentLayout`, `valid`.
- **ImageDesc:** `width`, `height`, `mipLevels`, `arrayLayers`, `format`, `usage`, `type` (ImageType), `memoryUsage`, `samples`.
- **ImageType:** `Texture2D`, `DepthStencil`, `RenderTarget`, `Storage`.
- **ImageId:** `u32` handle.

### Sampler

- **GpuSampler:** `sampler`, `desc`, `valid`.
- **SamplerDesc:** `magFilter`, `minFilter`, `mipFilter` (FilterMode), `wrapU`/`wrapV`/`wrapW` (WrapMode), `maxAnisotropy`, `anisotropyEnable`, `minLod`, `maxLod`, `mipLodBias`; equality comparable.
- **FilterMode:** `Nearest`, `Linear`.
- **WrapMode:** `Repeat`, `MirroredRepeat`, `ClampToEdge`, `ClampToBorder`.
- **SamplerId:** `u32` handle.

### Descriptor

- **DescriptorBinding:** `binding`, `type` (VkDescriptorType), `count`, `stageFlags`.
- **DescriptorLayoutDesc:** `vector<DescriptorBinding>`.
- **DescriptorPoolSize:** `type`, `count`.
- **DescLayoutId**, **DescSetId:** `u32` handles.

### Constants

- **kInvalidResourceId** — sentinel invalid ID.
- **kTextureBytesPerPixelRGBA** — 4 (for R8G8B8A8).

---

## 2. BufferManager (buffer_manager.hpp)

Manages Vulkan buffers: create, map, write, flush.

### Lifecycle

- **init(device)** — set device (from VkDeviceWrapper).
- **shutdown()** / **destroyAll()**.

### Create / destroy

- **createBuffer(desc)** → `Result<BufferId>`.
- **destroyBuffer(id)** → `Result<void>`.
- **destroyAll()**.

### Mapping

- **map(id, offset, size)** → `Result<void*>` (persistent map for CpuToGpu; use **VK_WHOLE_SIZE** for full buffer).
- **unmap(id)**.
- **isMapped(id)**.

### Write

- **write(id, data, size, offset)** → `Result<void>` (copies to buffer; for mapped buffers may need **flush** for coherence).
- **flush(id, offset, size)** → `Result<void>` (flush mapped range).

### Getters

- **get(id)** → `const GpuBuffer*`, **handle(id)** → `VkBuffer`, **exists(id)**.

```cpp
vapi::BufferManager bufMgr;
bufMgr.init(gpu.device());
vapi::BufferDesc desc;
desc.size = 1024 * 1024;
desc.usage = vapi::BufferUsage::Vertex | vapi::BufferUsage::TransferDst;
desc.memoryUsage = vapi::MemoryUsage::GpuOnly;
auto id = bufMgr.createBuffer(desc);
VkBuffer vkBuf = bufMgr.handle(*id);
// ... use vkBuf; when done:
bufMgr.destroyBuffer(*id);
bufMgr.shutdown();
```

---

## 3. ImageManager (image_manager.hpp)

Manages Vulkan images and image views.

### Lifecycle

- **init(device)**, **shutdown()**, **destroyAll()**.

### Create / destroy

- **createImage(desc)** → `Result<ImageId>` (creates image and view).
- **createDepthImage(width, height, format)** → `Result<ImageId>` (depth/stencil; default format D32_SFLOAT).
- **destroyImage(id)** → `Result<void>`.

### Getters

- **get(id)** → `const GpuImage*`, **handle(id)** → `VkImage`, **view(id)** → `VkImageView`, **exists(id)**.

```cpp
vapi::ImageManager imgMgr;
imgMgr.init(gpu.device());
vapi::ImageDesc idesc;
idesc.width = 1024;
idesc.height = 1024;
idesc.format = VK_FORMAT_R8G8B8A8_SRGB;
idesc.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
auto imgId = imgMgr.createImage(idesc);
auto depthId = imgMgr.createDepthImage(800, 600);
VkImageView colorView = imgMgr.view(*imgId);
// Use colorView in descriptor set or as framebuffer attachment
```

---

## 4. SamplerCache (sampler_cache.hpp)

Caches VkSampler by SamplerDesc (same descriptor → one sampler).

- **init(device)**, **shutdown()**, **destroyAll()**.
- **getOrCreate(desc)** → `Result<VkSampler>`.

```cpp
vapi::SamplerCache samplers;
samplers.init(gpu.device());
vapi::SamplerDesc sdesc;
sdesc.magFilter = vapi::FilterMode::Linear;
sdesc.minFilter = vapi::FilterMode::Linear;
sdesc.wrapU = sdesc.wrapV = vapi::WrapMode::Repeat;
VkSampler samp = *samplers.getOrCreate(sdesc);
// Use with descriptor write: writeImage(set, binding, imageView, samp, ...);
```

---

## 5. DescriptorManager (descriptor_manager.hpp)

Descriptor set layouts, pool, set allocation, and write operations.

### Lifecycle

- **init(device)**, **shutdown()**, **destroyAll()**.

### Layouts

- **createLayout(DescriptorLayoutDesc)** → `Result<DescLayoutId>`.
- **getLayout(id)** → `VkDescriptorSetLayout`.

### Pool

- **createPool(sizes, maxSets)** → `Result<void>` (sizes = vector of DescriptorPoolSize).
- **resetPool()**.

### Sets

- **allocateSet(layoutId)** → `Result<VkDescriptorSet>`.
- **allocateSets(layoutId, count)** → `Result<vector<VkDescriptorSet>>`.

### Write

- **writeBuffer(set, binding, buffer, size, offset, type)** — for uniform/storage buffers.
- **writeImage(set, binding, view, sampler, layout, type)** — for combined image sampler (and similar).

```cpp
vapi::DescriptorManager descMgr;
descMgr.init(gpu.device());
vapi::DescriptorLayoutDesc layoutDesc;
layoutDesc.bindings = {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT}};
auto layoutId = descMgr.createLayout(layoutDesc);
descMgr.createPool({{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}}, 10);
auto set = descMgr.allocateSet(*layoutId);
descMgr.writeBuffer(*set, 0, uniformBuffer, sizeof(UniformData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

// Combined image sampler (e.g. texture + SamplerCache result)
descMgr.writeImage(*set, 1, textureView, sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
```

---

## 6. StagingManager (staging.hpp)

One-shot CPU-to-GPU uploads via a staging buffer and transfer commands. Needs a **VkCommandManager** (e.g. from RenderContext) for submission.

- **init(device, commandManager)**, **shutdown()**.
- **uploadBuffer(dstBuffer, data, dstOffset)** → `Result<void>` — upload bytes into a GPU buffer (e.g. vertex/index).
- **uploadImage(dstImage, width, height, data, finalLayout)** → `Result<void>` — initial image upload (UNDEFINED → finalLayout).
- **updateImage(dstImage, width, height, data, currentLayout)** → `Result<void>` — update existing image (e.g. texture or video frame; transitions SHADER_READ_ONLY → TRANSFER_DST, copy, back).

```cpp
vapi::StagingManager staging;
staging.init(gpu.device(), &renderCtx.commandManager());
staging.uploadBuffer(vertexBuffer, std::span<const u8>(vertexData));
staging.uploadImage(textureImage, 256, 256, std::span<const u8>(pixels));

// Update existing texture (e.g. next video frame)
staging.updateImage(textureImage, 256, 256, std::span<const u8>(newPixels));
staging.shutdown();
```

---

## 7. Complete example: buffer + staging + descriptor

```cpp
#include "resource/resource.hpp"
#include "gpu/gpu_context.hpp"

vapi::BufferManager bufMgr;
vapi::StagingManager staging;
vapi::DescriptorManager descMgr;
bufMgr.init(gpu.device());
staging.init(gpu.device(), &renderCtx.commandManager());
descMgr.init(gpu.device());

// Create vertex buffer (GPU-only), upload via staging
vapi::BufferDesc vbDesc;
vbDesc.size = vertices.size() * sizeof(Vertex);
vbDesc.usage = vapi::BufferUsage::Vertex | vapi::BufferUsage::TransferDst;
vbDesc.memoryUsage = vapi::MemoryUsage::GpuOnly;
auto vbId = bufMgr.createBuffer(vbDesc);
staging.uploadBuffer(bufMgr.handle(*vbId), std::as_bytes(std::span(vertices)));

// Layout and set for uniform + texture
vapi::DescriptorLayoutDesc layoutDesc;
layoutDesc.bindings = {
    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT}
};
auto layoutId = descMgr.createLayout(layoutDesc);
descMgr.createPool({{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}}, 1);
auto set = descMgr.allocateSet(*layoutId);
descMgr.writeBuffer(*set, 0, uniformBuf, sizeof(UniformData));
descMgr.writeImage(*set, 1, imgMgr.view(texId), *samplers.getOrCreate(sdesc));
// Bind set in pipeline: vkCmdBindDescriptorSets(cmd, ..., 1, &*set, ...);
```

---

## 8. Usage order

1. **BufferManager** / **ImageManager**: init with device; create buffers/images; use handles/views in pipelines and descriptor writes.
2. **SamplerCache**: init; getOrCreate(SamplerDesc) for texture sampling.
3. **DescriptorManager**: init; createLayout; createPool; allocateSet(s); writeBuffer/writeImage; use sets in pipeline.
4. **StagingManager**: init with device and VkCommandManager; uploadBuffer/uploadImage/updateImage (submit command buffer as needed); shutdown when done.
5. On shutdown: destroy resources (or destroyAll), then shutdown each manager.

See **Section 7** above for a full code example combining BufferManager, StagingManager, and DescriptorManager.
