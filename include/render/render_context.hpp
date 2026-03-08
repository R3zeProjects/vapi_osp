/** @file render_context.hpp
 *  @brief Per-frame render context: sync, command buffers, dynamic rendering. */

#ifndef VAPI_RENDER_RENDER_CONTEXT_HPP
#define VAPI_RENDER_RENDER_CONTEXT_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_device.hpp"
#include "gpu/vk_swapchain.hpp"
#include "gpu/vk_sync.hpp"
#include "gpu/vk_command.hpp"
#include "gpu/vk_image_utils.hpp"
#include <vulkan/vulkan.h>

namespace vapi {

/** Default and recommended for low latency. Supported range 2..8 for flexibility. */
inline constexpr u32 kDefaultFramesInFlight = 2;
inline constexpr u32 kMaxFramesInFlight = 8;

struct FrameData {
    u32              frameIndex{0};
    u32              imageIndex{0};
    VkCommandBuffer  cmd{VK_NULL_HANDLE};
    VkExtent2D       extent{0, 0};
    VkFormat         colorFormat{VK_FORMAT_UNDEFINED};
};

struct RenderContextConfig {
    /** Number of frames to overlap (2..8). Default 2; use 3 for smoother resize. */
    u32  framesInFlight{kDefaultFramesInFlight};
    bool vsync{true};
};

class RenderContext {
public:
    RenderContext() = default;
    ~RenderContext() = default;

    /** Initialize render context. Creates per-frame sync objects and command pools.
     *  @param device   Non-null; must remain valid and outlive this RenderContext.
     *  @param swapchain Non-null; must remain valid and outlive this RenderContext.
     *  @param cfg      Frame count (2..8) and vsync.
     *  @return Ok on success; error on sync/command creation failure.
     *  @pre device != nullptr, swapchain != nullptr. */
    [[nodiscard]] Result<void> init(VkDeviceWrapper* device, VkSwapchainWrapper* swapchain, const RenderContextConfig& cfg);

    /** Release sync and command resources. Safe to call when not initialized. */
    void shutdown();

    /** Start next frame: wait for fence, acquire swapchain image, begin command buffer.
     *  @return FrameData for this frame, or error (e.g. SwapchainOutOfDate).
     *  @pre init() succeeded; call handleResize() when needsResize() is true before next beginFrame(). */
    [[nodiscard]] Result<FrameData> beginFrame();

    /** End frame: end commands, submit, present. Advances current frame index.
     *  @param frame The FrameData from beginFrame() for this frame.
     *  @return Ok on success; error on submit/present failure.
     *  @pre beginFrame() returned Ok for this frame; frame matches current frame. */
    [[nodiscard]] Result<void> endFrame(const FrameData& frame);

    /** Start dynamic rendering (color only). */
    void beginRendering(const FrameData& frame, const VkClearColorValue& clearColor);
    /** Start dynamic rendering with color and depth attachments. */
    void beginRendering(const FrameData& frame, const VkClearColorValue& clearColor,
                        VkImageView depthView, VkClearDepthStencilValue depthClear = {1.f, 0});
    /** End dynamic rendering and transition swapchain image to present layout. */
    void endRendering(const FrameData& frame);

    /** Set viewport and scissor to full swapchain extent on the given command buffer. */
    void setFullViewport(VkCommandBuffer cmd) const;
    /** Set viewport and scissor to frame extent. */
    void setFullViewport(const FrameData& frame) const;
    /** Set viewport/scissor from extent (avoids desync on resize). */
    void setFullViewport(VkCommandBuffer cmd, VkExtent2D extent) const;

    [[nodiscard]] u32 currentFrame() const { return m_currentFrame; }
    [[nodiscard]] u32 framesInFlight() const { return m_framesInFlight; }
    [[nodiscard]] bool needsResize() const { return m_needsResize; }
    void requestResize() { m_needsResize = true; }

    [[nodiscard]] VkSyncManager&    syncManager()    { return m_sync; }
    [[nodiscard]] VkCommandManager& commandManager() { return m_commands; }

    /** Recreate swapchain with new size. Call with framebuffer dimensions (e.g. from resize callback).
     *  @param framebufferWidth  New width (must be > 0).
     *  @param framebufferHeight New height (must be > 0).
     *  @return Ok on success; error if extent zero or recreation fails.
     *  @pre init() succeeded; device and swapchain still valid. */
    [[nodiscard]] Result<void> handleResize(u32 framebufferWidth, u32 framebufferHeight);

private:

    VkDeviceWrapper*    m_device{nullptr};
    VkSwapchainWrapper* m_swapchain{nullptr};

    VkSyncManager    m_sync;
    VkCommandManager m_commands;

    u32  m_framesInFlight{kDefaultFramesInFlight};
    u32  m_currentFrame{0};
    bool m_needsResize{false};
};

} // namespace vapi

#endif
