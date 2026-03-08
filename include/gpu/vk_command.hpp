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

    /** Create per-frame command pools and buffers. device must remain valid until destroy().
     *  @param device Valid VkDevice; must outlive this manager. */
    [[nodiscard]] Result<void> create(VkDevice device, u32 queueFamily, u32 framesInFlight);
    /** Destroy pools and buffers. device must be the same as passed to create(). */
    void destroy(VkDevice device);

    /** Reset command pool for the given frame. */
    [[nodiscard]] Result<void> resetPool(VkDevice device, u32 frameIndex);

    [[nodiscard]] VkCommandBuffer buffer(u32 frameIndex) const { return m_buffers[frameIndex]; }

    /** Begin recording command buffer for frame. */
    [[nodiscard]] Result<void> begin(u32 frameIndex, VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    /** End recording. */
    [[nodiscard]] Result<void> end(u32 frameIndex);

    /** Submit command buffer for frame. queue must be the graphics queue used with this manager. */
    [[nodiscard]] Result<void> submit(VkQueue queue, u32 frameIndex,
        VkSemaphore waitSem, VkSemaphore signalSem, VkFence fence,
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    /** Allocate and begin a single-time command buffer. Call endSingleTime to submit and free. */
    [[nodiscard]] Result<VkCommandBuffer> beginSingleTime(VkDevice device);
    /** Submit single-time buffer and wait; then free. cmd must be from beginSingleTime(). */
    [[nodiscard]] Result<void> endSingleTime(VkDevice device, VkQueue queue, VkCommandBuffer cmd);

    [[nodiscard]] VkCommandPool pool(u32 frameIndex) const { return m_pools[frameIndex]; }

private:
    std::vector<VkCommandPool>   m_pools;
    std::vector<VkCommandBuffer> m_buffers;
};

} // namespace vapi

#endif
