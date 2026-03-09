/** @file vk_command.hpp
 *  @brief Per-frame command pools and buffers, submit helpers. */

#ifndef VAPI_GPU_VK_COMMAND_HPP
#define VAPI_GPU_VK_COMMAND_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vapi {

class VkCommandManager {
public:
    VkCommandManager() = default;
    ~VkCommandManager() = default;
    VkCommandManager(const VkCommandManager&) = delete;
    VkCommandManager& operator=(const VkCommandManager&) = delete;
    VkCommandManager(VkCommandManager&&) noexcept = default;
    VkCommandManager& operator=(VkCommandManager&&) noexcept = default;

    /** Create per-frame command pools and buffers. device must remain valid until destroy().
     *  @param device Valid VkDevice; stored internally and used for all subsequent operations. */
    [[nodiscard]] Result<void> create(VkDevice device, u32 queueFamily, u32 framesInFlight);
    /** Destroy pools and buffers. Uses the device passed to create(). */
    void destroy();

    /** Reset command pool for the given frame. */
    [[nodiscard]] Result<void> resetPool(u32 frameIndex);

    [[nodiscard]] VkCommandBuffer buffer(u32 frameIndex) const;

    /** Begin recording command buffer for frame. */
    [[nodiscard]] Result<void> begin(u32 frameIndex, VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    /** End recording. */
    [[nodiscard]] Result<void> end(u32 frameIndex);

    /** Submit command buffer for frame. queue must be the graphics queue used with this manager. */
    [[nodiscard]] Result<void> submit(VkQueue queue, u32 frameIndex,
        VkSemaphore waitSem, VkSemaphore signalSem, VkFence fence,
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    /** Allocate and begin a single-time command buffer. Call endSingleTime to submit and free. */
    [[nodiscard]] Result<VkCommandBuffer> beginSingleTime();
    /** Submit single-time buffer and wait; then free. cmd must be from beginSingleTime(). */
    [[nodiscard]] Result<void> endSingleTime(VkQueue queue, VkCommandBuffer cmd);
    /** Submit single-time buffer with fence (no wait); then free. Caller must wait/reset fence. */
    [[nodiscard]] Result<void> endSingleTimeWithFence(VkQueue queue, VkCommandBuffer cmd, VkFence fence);

    [[nodiscard]] VkCommandPool pool(u32 frameIndex) const;

private:
    VkDevice                     m_device{VK_NULL_HANDLE};
    std::vector<VkCommandPool>   m_pools;
    std::vector<VkCommandBuffer> m_buffers;
};

} // namespace vapi

#endif
