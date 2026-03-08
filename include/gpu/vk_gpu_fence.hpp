/** @file vk_gpu_fence.hpp
 *  @brief Vulkan implementation of IGpuFence. */

#ifndef VAPI_GPU_VK_GPU_FENCE_HPP
#define VAPI_GPU_VK_GPU_FENCE_HPP

#include "core/interfaces/i_gpu_tools.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>

namespace vapi {

/** Vulkan-based GPU fence. Use nativeFence() for vkQueueSubmit; then wait() or isSignaled(). */
class VkGpuFence : public IGpuFence {
public:
    VkGpuFence() = default;
    ~VkGpuFence() override;

    VkGpuFence(const VkGpuFence&) = delete;
    VkGpuFence& operator=(const VkGpuFence&) = delete;

    /** Create fence in unsignaled state. */
    [[nodiscard]] Result<void> create(VkDevice device);
    void destroy(VkDevice device);

    [[nodiscard]] Result<void> wait(u64 timeoutMs = ~0u) override;
    [[nodiscard]] bool isSignaled() const override;
    void reset() override;

    /** For vkQueueSubmit(..., fence). */
    [[nodiscard]] VkFence nativeFence() const { return m_fence; }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkFence  m_fence{VK_NULL_HANDLE};
};

} // namespace vapi

#endif
