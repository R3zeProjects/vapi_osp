/** @file i_gpu_backend.hpp
 *  @brief Graphics backend interface (parent). Implementations: Vulkan, and later D3D/Metal. Use smart pointer to the interface to work with any backend. */

#ifndef VAPI_GPU_I_GPU_BACKEND_HPP
#define VAPI_GPU_I_GPU_BACKEND_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <vector>

namespace vapi {

/** Identifies which graphics API the backend uses (for safe casting to concrete backend). */
enum class GpuBackendType : u8 { Vulkan = 0, /* D3D12, Metal */ };

/** Backend-agnostic config for GPU context init. Concrete backends map this to their own config. */
struct GpuBackendConfig {
    std::string appName{"VAPI Application"};
    std::vector<std::string> requiredExtensions;
    /** Native window handle (e.g. GLFWwindow*). nullptr = headless. */
    void* windowHandle{nullptr};
    u32 swapchainWidth{0};
    u32 swapchainHeight{0};
    bool createSwapchain{false};
    bool vsync{true};
    /** Enable Vulkan validation layers (useful for debugging, significant runtime overhead). */
    bool enableValidation{false};
};

/**
 * Parent type for all graphics backends. Use createVulkanGpuBackend() or a future factory to get
 * a concrete implementation. Hold as unique_ptr<IGpuBackend> or shared_ptr<IGpuBackend> when
 * multiple components share the same backend.
 */
class IGpuBackend {
public:
    virtual ~IGpuBackend() = default;

    [[nodiscard]] virtual GpuBackendType type() const = 0;
    [[nodiscard]] virtual Result<void> init(const GpuBackendConfig& config) = 0;
    virtual void shutdown() = 0;
    [[nodiscard]] virtual bool isInitialized() const = 0;

    /** @deprecated Use getVulkanDevice() from vulkan_backend_access.hpp for type-safe access.
     *  Native device handle (e.g. VkDevice for Vulkan). Check type() before casting. */
    [[nodiscard]] virtual void* getDevice() = 0;
    /** @deprecated Use getVulkanInstance() from vulkan_backend_access.hpp for type-safe access. */
    [[nodiscard]] virtual void* getInstance() = 0;
    /** @deprecated Use getVulkanSurface() from vulkan_backend_access.hpp for type-safe access.
     *  Native surface (e.g. VkSurfaceKHR). nullptr if headless. */
    [[nodiscard]] virtual void* getSurface() = 0;
    /** @deprecated Use getVulkanSwapchain() from vulkan_backend_access.hpp for type-safe access.
     *  Native swapchain (e.g. VkSwapchainKHR). nullptr if not created. */
    [[nodiscard]] virtual void* getSwapchain() = 0;

    [[nodiscard]] virtual Result<void> recreateSwapchain(u32 width, u32 height) = 0;
};

} // namespace vapi

#endif // VAPI_GPU_I_GPU_BACKEND_HPP
