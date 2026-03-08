/** @file vulkan_gpu_backend.hpp
 *  @brief Vulkan implementation of IGpuBackend. Uses GpuContext internally. Include this when you need the Vulkan backend. */

#ifndef VAPI_GPU_VULKAN_GPU_BACKEND_HPP
#define VAPI_GPU_VULKAN_GPU_BACKEND_HPP

#include "gpu/i_gpu_backend.hpp"
#include "gpu/gpu_context.hpp"
#include <memory>

namespace vapi {

/** Vulkan implementation of the graphics backend. Hold as unique_ptr<IGpuBackend> or shared_ptr<IGpuBackend>; cast to VulkanGpuBackend* when you need Vulkan-specific access (e.g. VkDeviceWrapper*). */
class VulkanGpuBackend : public IGpuBackend {
public:
    VulkanGpuBackend() = default;
    ~VulkanGpuBackend() override;

    [[nodiscard]] GpuBackendType type() const override { return GpuBackendType::Vulkan; }
    /** Initialize Vulkan context. config.windowHandle (if non-null) must outlive until shutdown().
     *  @param config GpuBackendConfig; window handle must remain valid if non-null. */
    [[nodiscard]] Result<void> init(const GpuBackendConfig& config) override;
    void shutdown() override;
    [[nodiscard]] bool isInitialized() const override;

    /** Non-owning; valid until shutdown(). Cast to VkDeviceWrapper*. */
    [[nodiscard]] void* getDevice() override;
    [[nodiscard]] void* getInstance() override;
    [[nodiscard]] void* getSurface() override;
    [[nodiscard]] void* getSwapchain() override;

    [[nodiscard]] Result<void> recreateSwapchain(u32 width, u32 height) override;

    /** Vulkan-specific: access the full context. Non-owning; valid until shutdown(). */
    [[nodiscard]] GpuContext* context() { return m_ctx.get(); }
    [[nodiscard]] const GpuContext* context() const { return m_ctx.get(); }

private:
    std::unique_ptr<GpuContext> m_ctx;
};

/** Create Vulkan backend (unique ownership). */
[[nodiscard]] std::unique_ptr<IGpuBackend> createVulkanGpuBackend();

/** Create Vulkan backend (shared ownership). */
[[nodiscard]] std::shared_ptr<IGpuBackend> createVulkanGpuBackendShared();

} // namespace vapi

#endif // VAPI_GPU_VULKAN_GPU_BACKEND_HPP
