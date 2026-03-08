/** @file vulkan_backend_access.hpp
 *  @brief Typed accessors for Vulkan backend. Use when IGpuBackend::type() == GpuBackendType::Vulkan.
 *  Raw void* from getDevice()/getInstance()/getSurface()/getSwapchain() remain available as escape hatch. */

#ifndef VAPI_GPU_VULKAN_BACKEND_ACCESS_HPP
#define VAPI_GPU_VULKAN_BACKEND_ACCESS_HPP

#include "gpu/i_gpu_backend.hpp"
#include <vulkan/vulkan.h>

namespace vapi {

/** @return VkInstance when backend type is Vulkan, else VK_NULL_HANDLE. */
[[nodiscard]] inline VkInstance getVulkanInstance(IGpuBackend* backend) noexcept {
    if (!backend || backend->type() != GpuBackendType::Vulkan) return VK_NULL_HANDLE;
    return static_cast<VkInstance>(backend->getInstance());
}

/** @return VkDevice when backend type is Vulkan, else VK_NULL_HANDLE. */
[[nodiscard]] inline VkDevice getVulkanDevice(IGpuBackend* backend) noexcept {
    if (!backend || backend->type() != GpuBackendType::Vulkan) return VK_NULL_HANDLE;
    return static_cast<VkDevice>(backend->getDevice());
}

/** @return VkSurfaceKHR when backend type is Vulkan (may be VK_NULL_HANDLE if headless). */
[[nodiscard]] inline VkSurfaceKHR getVulkanSurface(IGpuBackend* backend) noexcept {
    if (!backend || backend->type() != GpuBackendType::Vulkan) return VK_NULL_HANDLE;
    return static_cast<VkSurfaceKHR>(backend->getSurface());
}

/** @return VkSwapchainKHR when backend type is Vulkan (may be VK_NULL_HANDLE if not created). */
[[nodiscard]] inline VkSwapchainKHR getVulkanSwapchain(IGpuBackend* backend) noexcept {
    if (!backend || backend->type() != GpuBackendType::Vulkan) return VK_NULL_HANDLE;
    return static_cast<VkSwapchainKHR>(backend->getSwapchain());
}

} // namespace vapi

#endif // VAPI_GPU_VULKAN_BACKEND_ACCESS_HPP
