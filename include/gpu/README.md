# GPU module

Vulkan instance, device, surface, swapchain, sync, and command buffers.

- **vk_errors.hpp** — `gpu_errors::*` constants, `toError(VkResult)`, `VK_CHECK` / `VK_CHECK_BOOL`.
- **vk_base.hpp** — `QueueFamilyIndices`, `SwapchainSupportDetails`, `toVkExtent`, `toSizeElement`, `toDeviceType`, `toVkClearColor`.
- **vk_instance.hpp/cpp** — `VkInstance` creation, validation layers, extensions.
- **vk_device.hpp/cpp** — Physical device selection, logical device, queues.
- **vk_surface.hpp/cpp** — Surface creation (e.g. GLFW).
- **vk_swapchain.hpp/cpp** — Swapchain create/resize, image views.
- **vk_sync.hpp/cpp** — Fences and semaphores.
- **vk_command.hpp/cpp** — Command pool and buffer allocation.

Header-only: **vk_check.hpp**, **vk_image_utils.hpp**. Include: `#include "gpu/…"`. Link: `vapi_gpu` (Vulkan, glfw, vapi_core).
