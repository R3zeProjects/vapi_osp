# GPU module — Detailed API

Include: `#include "gpu/gpu.hpp"` (includes errors, base, instance, device, surface, swapchain, sync, command, image utils, **gpu_context**). Or use root `#include "vapi.hpp"`.

---

## 1. GpuContext (single facade)

**Header:** `gpu/gpu_context.hpp`

Owns Vulkan instance, logical device, optional surface, and optional swapchain. One init flow for the typical “window + Vulkan + swapchain” setup.

### GpuContextConfig

- **instance** — `InstanceConfig` (app name, version, validation, required extensions, extra layers).
- **device** — `DeviceConfig` (surface and instance are filled by GpuContext when using window).
- **windowHandle** — `void*` (e.g. `GLFWwindow*` from `platform->getNativeHandle(winId)`). `nullptr` = headless (no surface).
- **swapchainWidth**, **swapchainHeight** — extent for swapchain (used only if `createSwapchain` is true and windowHandle non-null).
- **createSwapchain** — if true and window + extent set, creates swapchain after device.
- **vsync** — swapchain present mode (vsync on/off).

### GpuContext methods

- **init(cfg)** → `Result<void>`. Order: instance → surface (if windowHandle) → device → swapchain (if requested).
- **shutdown()** — destroys swapchain, surface, device, instance in that order.
- **isInitialized()** — true when instance handle is non-null.
- **instance()**, **device()**, **surface()**, **swapchain()** — raw pointers to the wrappers (nullable).
- **recreateSwapchain(width, height)** → `Result<void>` — for window resize.

```cpp
vapi::GpuContext gpu;
vapi::GpuContextConfig cfg;
cfg.instance.appName = "MyApp";
cfg.instance.requiredExtensions = platform->getRequiredVulkanExtensions();
cfg.windowHandle = platform->getNativeHandle(winId);
cfg.swapchainWidth = 1280;
cfg.swapchainHeight = 720;
cfg.createSwapchain = true;
cfg.vsync = true;
if (!gpu.init(cfg).has_value()) return;
VkDevice dev = gpu.device()->device();
VkSwapchainKHR sw = gpu.swapchain()->handle();
// On resize:
gpu.recreateSwapchain(newWidth, newHeight);
```

---

## 2. Instance (VkInstanceWrapper)

**Header:** `gpu/vk_instance.hpp`

### InstanceConfig

- **appName**, **appVersion** (VkVersion).
- **enableValidation** — request validation layers.
- **requiredExtensions** — e.g. from `platform->getRequiredVulkanExtensions()`.
- **extraLayers** — optional instance layers.

### VkInstanceWrapper

- **create(cfg)** → `Result<void>`.
- **destroy()**.
- **handle()** → `VkInstance`.
- **validationEnabled()** — true if validation was enabled.
- Move-only; not copyable.

```cpp
vapi::VkInstanceWrapper instance;
vapi::InstanceConfig instCfg;
instCfg.appName = "MyApp";
instCfg.requiredExtensions = platform->getRequiredVulkanExtensions();
instCfg.enableValidation = true;
if (!instance.create(instCfg).has_value()) return;
VkInstance vkInst = instance.handle();
```

---

## 3. Device (VkDeviceWrapper)

**Header:** `gpu/vk_device.hpp`

### DeviceConfig

- **instance** — `VkInstance`.
- **surface** — `VkSurfaceKHR` (for present queue; set by GpuContext when using a window).
- **requiredExtensions** — default includes `VK_KHR_SWAPCHAIN_EXTENSION_NAME`.
- **preferDiscrete** — prefer discrete GPU when picking physical device.

### VkDeviceWrapper

- **create(cfg)** → `Result<void>`.
- **destroy()**.
- **device()**, **physicalDevice()**, **graphicsQueue()**, **presentQueue()**, **computeQueue()**, **transferQueue()**.
- **queueFamilies()** → `QueueFamilyIndices` (graphics, present, compute, transfer).
- **properties()** → `DeviceProperties` (name, vendor/device ID, type, limits).
- **waitIdle()** → `Result<void>`.
- **querySwapchainSupport(surface)** → `SwapchainSupportDetails`.
- **findMemoryType(typeBits, propertyFlags)** → `Result<u32>`.

```cpp
vapi::VkDeviceWrapper device;
vapi::DeviceConfig devCfg;
devCfg.instance = instance.handle();
devCfg.surface = surface.handle();
if (!device.create(devCfg).has_value()) return;
VkQueue graphicsQueue = device.graphicsQueue();
auto support = device.querySwapchainSupport(surface.handle());
```

---

## 4. Surface (VkSurfaceWrapper)

**Header:** `gpu/vk_surface.hpp`

- **create(instance, GLFWwindow*)** → `Result<void>`.
- **destroy()**.
- **handle()** → `VkSurfaceKHR`.

Typically you do not create this manually when using **GpuContext**; it is created from `windowHandle`.

---

## 5. Swapchain (VkSwapchainWrapper)

**Header:** `gpu/vk_swapchain.hpp`

### SwapchainConfig

- **surface**, **width**, **height**, **vsync**, **preferredFormat**, **preferredColorSpace**.

### VkSwapchainWrapper

- **create(device, cfg)** → `Result<void>`.
- **recreate(device, width, height)** → `Result<void>` (e.g. on resize).
- **destroy(device)** — call before destroying device.
- **acquireNextImage(device, semaphore, timeout)** → `Result<u32>` (image index); may return swapchain-out-of-date.
- **present(queue, imageIndex, waitSemaphore)** → `Result<void>`.
- **handle()**, **format()**, **extent()**, **imageCount()**, **image(i)**, **imageView(i)**, **images()**, **imageViews()**, **needsRecreation()**.

```cpp
vapi::VkSwapchainWrapper swapchain;
vapi::SwapchainConfig scCfg;
scCfg.surface = surface.handle();
scCfg.width = 1280;
scCfg.height = 720;
scCfg.vsync = true;
swapchain.create(device, scCfg);

// In render loop:
auto idxRes = swapchain.acquireNextImage(device.device(), imageAvailableSemaphore);
if (!idxRes) { /* handle swapchain out of date */ }
u32 imageIndex = *idxRes;
// ... record and submit ...
swapchain.present(device.presentQueue(), imageIndex, renderFinishedSemaphore);
```

---

## 6. Base helpers (vk_base.hpp)

- **toVkExtent(w, h)** / **toVkExtent(SizeElement)** → `VkExtent2D`.
- **toSizeElement(VkExtent2D)** → `SizeElement`.
- **toVkClearColor(color4)** → `VkClearColorValue`.
- **toDeviceType(VkPhysicalDeviceType)** → `DeviceType`.
- **QueueFamilyIndices** — graphics, present, compute, transfer indices; **isComplete()**, **uniqueFamilies()**.
- **SwapchainSupportDetails** — capabilities, formats, present modes; **isAdequate()**.

```cpp
vapi::VkExtent2D ext = vapi::toVkExtent(1280u, 720u);
vapi::color4 clearColor(0.1f, 0.1f, 0.15f, 1.f);
VkClearColorValue vkClear = vapi::toVkClearColor(clearColor);
```

---

## 7. Sync (VkSyncManager) and Command (VkCommandManager)

**Headers:** `gpu/vk_sync.hpp`, `gpu/vk_command.hpp`

- **VkSyncManager:** create/destroy per-frame semaphores and fences; **waitForFrame**, **resetFrame**; **frame(index)**, **renderFinished(imageIndex)**.
- **VkCommandManager:** allocates and manages command buffers (often used via **RenderContext**, which holds sync and command managers).

```cpp
vapi::VkSyncManager sync;
sync.create(device.device(), framesInFlight, swapchain.imageCount());
sync.waitForFrame(device.device(), frameIndex);
// ... submit work ...
sync.resetFrame(device.device(), frameIndex);
VkSemaphore renderFin = sync.renderFinished(imageIndex);
```

---

## 8. GPU errors (vk_errors.hpp)

Namespace **vapi::gpu_errors**: error codes for Vulkan operations (e.g. surface creation, swapchain creation, swapchain out of date). Use with **Result** and **errorMessage()** for user-facing messages.

```cpp
auto r = swapchain.acquireNextImage(device.device(), sem, 0);
if (!r) {
    if (r.error().value() == /* swapchain out of date */)
        recreateSwapchain();
    else
        vapi::logError("GPU", std::string(vapi::errorMessage(r.error())));
}
```

---

## 9. Image utils (vk_image_utils.hpp)

Helpers for creating Vulkan images and views (e.g. depth buffers, render targets). See header for function list; often used alongside **ImageManager** in the resource module.

---

## 10. GPU tools (VkGpuProfiler, VkGpuFence)

**Headers:** `gpu/vk_gpu_profiler.hpp`, `gpu/vk_gpu_fence.hpp`. Implement **IGpuProfiler** and **IGpuFence** from `core/interfaces/i_gpu_tools.hpp`.

**VkGpuProfiler:** Timestamp query pool. Create with `create(device, physicalDevice, numRanges)` (each range = begin/end pair). When recording a frame command buffer, call `reset(cmd)` once, then `writeTimestamp(0, cmd)` and `writeTimestamp(1, cmd)` (and so on) at the points you want to measure. After submit and wait, use `getResult(0)` to get `GpuTimingResult` in seconds. Pass the **current** `VkCommandBuffer` from your record site as `void*`; no extra layer required.

**VkGpuFence:** Wraps `VkFence`. Create with `create(device)` (unsignaled). Before submit call `reset()`, then pass `nativeFence()` to `vkQueueSubmit`. Use `wait(timeoutMs)` or `isSignaled()` after submit.

```cpp
vapi::VkGpuProfiler profiler;
profiler.create(ctx->device()->device(), ctx->device()->physicalDevice(), 1);
vapi::VkGpuFence fence;
fence.create(ctx->device()->device());

// In frame record:
VkCommandBuffer cmd = cmdMgr.buffer(frameIndex);
profiler.reset(cmd);
profiler.writeTimestamp(0, cmd);
// ... draw ...
profiler.writeTimestamp(1, cmd);

fence.reset();
cmdMgr.submit(queue, frameIndex, ..., fence.nativeFence());
fence.wait(5000);
auto timing = profiler.getResult(0);
```
