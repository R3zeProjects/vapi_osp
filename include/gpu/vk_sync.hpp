/** @file vk_sync.hpp
 *  @brief Per-frame sync: semaphores and fences for swapchain acquire and submit. */

#ifndef VAPI_GPU_VK_SYNC_HPP
#define VAPI_GPU_VK_SYNC_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vapi {

struct FrameSync {
    VkSemaphore imageAvailable{VK_NULL_HANDLE};
    VkFence     inFlight{VK_NULL_HANDLE};
};

class VkSyncManager {
public:
    VkSyncManager() = default;
    ~VkSyncManager() = default;

    /** Create per-frame and per-image sync objects. device must remain valid until destroy().
     *  @param device Valid VkDevice; must outlive this manager. */
    [[nodiscard]] Result<void> create(VkDevice device, u32 framesInFlight, u32 swapchainImageCount);
    /** Destroy sync objects. device must be the same as passed to create(). */
    void destroy(VkDevice device);
    /** Recreate per-image semaphores (e.g. after swapchain resize). device must be valid. */
    [[nodiscard]] Result<void> recreatePerImageSync(VkDevice device, u32 swapchainImageCount);

    /** Wait for fence of the given frame. @return Ok when signaled or timeout. */
    [[nodiscard]] Result<void> waitForFrame(VkDevice device, u32 frameIndex, u64 timeout = UINT64_MAX);
    /** Reset fence for the given frame. */
    void resetFrame(VkDevice device, u32 frameIndex);

    [[nodiscard]] const FrameSync& frame(u32 index) const { return m_frames[index]; }
    [[nodiscard]] u32 count() const { return static_cast<u32>(m_frames.size()); }
    [[nodiscard]] VkSemaphore renderFinished(u32 imageIndex) const { return m_renderFinished[imageIndex]; }

private:
    void destroyPerImageSync(VkDevice device);

    std::vector<FrameSync>    m_frames;
    std::vector<VkSemaphore> m_renderFinished;
};

} // namespace vapi

#endif
