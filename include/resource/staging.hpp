#ifndef VAPI_RESOURCE_STAGING_HPP
#define VAPI_RESOURCE_STAGING_HPP

#include "core/error.hpp"
#include "resource/resource_types.hpp"
#include <vulkan/vulkan.h>
#include <span>
#include <optional>
#include <vector>

namespace vapi {

class VkDeviceWrapper;
class VkCommandManager;
class BufferManager;
class ImageManager;

/**
 * Staging manager for one-shot CPU-to-GPU copies.
 *
 * Contract:
 * - Ownership: the caller owns the VkDeviceWrapper* and VkCommandManager* passed to init(); the manager only stores the pointers.
 *   They must remain valid until shutdown() is called.
 * - On init() failure: the object state is undefined; the caller must not use the manager until init() succeeds.
 *   It is safe to call shutdown() or to call init() again after a failed init().
 */
class StagingManager {
public:
    StagingManager() = default;
    ~StagingManager() = default;

    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device, VkCommandManager* commands);
    void shutdown();

    /// Upload data to GPU-only buffer via staging buffer. If dstSize is set, validates dstOffset + data.size() <= dstSize.
    [[nodiscard]] Result<void> uploadBuffer(VkBuffer dstBuffer, std::span<const u8> data, VkDeviceSize dstOffset = 0,
                                            std::optional<VkDeviceSize> dstSize = std::nullopt);

    /// Overload: get buffer handle and size from BufferManager; validates bounds automatically.
    [[nodiscard]] Result<void> uploadBuffer(BufferManager& bufferManager, BufferId dstId, std::span<const u8> data,
                                            VkDeviceSize dstOffset = 0);

    /// Upload data to GPU image via staging (initial upload: UNDEFINED -> finalLayout)
    [[nodiscard]] Result<void> uploadImage(VkImage dstImage, u32 width, u32 height,
                                            std::span<const u8> data, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    /// Overload: image and dimensions from ImageManager; use for managed textures.
    [[nodiscard]] Result<void> uploadImage(ImageManager& imageManager, ImageId dstId, std::span<const u8> data,
                                            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    /// Update existing texture (SHADER_READ_ONLY -> TRANSFER_DST, copy, -> SHADER_READ_ONLY). For video/frame updates.
    [[nodiscard]] Result<void> updateImage(VkImage dstImage, u32 width, u32 height,
                                           std::span<const u8> data,
                                           VkImageLayout currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    /// Overload: image and dimensions from ImageManager.
    [[nodiscard]] Result<void> updateImage(ImageManager& imageManager, ImageId dstId, std::span<const u8> data,
                                           VkImageLayout currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // ── Batch upload (one submit for many copies) ─────────────────────────
    /** Start recording a batch. Call uploadBufferBatch/uploadImageBatch/updateImageBatch, then endBatchAndSubmit(). */
    [[nodiscard]] Result<void> beginBatch();
    [[nodiscard]] Result<void> uploadBufferBatch(VkBuffer dstBuffer, std::span<const u8> data, VkDeviceSize dstOffset = 0,
                                                  std::optional<VkDeviceSize> dstSize = std::nullopt);
    [[nodiscard]] Result<void> uploadBufferBatch(BufferManager& bufferManager, BufferId dstId, std::span<const u8> data, VkDeviceSize dstOffset = 0);
    [[nodiscard]] Result<void> uploadImageBatch(VkImage dstImage, u32 width, u32 height, std::span<const u8> data,
                                                 VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] Result<void> uploadImageBatch(ImageManager& imageManager, ImageId dstId, std::span<const u8> data,
                                                 VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    /** Submit the batch and wait. Call only after beginBatch() and one or more *Batch calls. */
    [[nodiscard]] Result<void> endBatchAndSubmit();

private:
    struct StagingBuffer {
        VkBuffer       buffer{VK_NULL_HANDLE};
        VkDeviceMemory memory{VK_NULL_HANDLE};
        VkDeviceSize   size{0};
    };

    [[nodiscard]] Result<StagingBuffer> createStagingBuffer(VkDeviceSize size);
    void destroyStagingBuffer(StagingBuffer& buf);

    const VkDeviceWrapper* m_device{nullptr};
    VkCommandManager*      m_commands{nullptr};

    bool                           m_batchActive{false};
    VkCommandBuffer                m_batchCmd{VK_NULL_HANDLE};
    std::vector<StagingBuffer>     m_batchStagingBuffers;
};

} // namespace vapi

#endif
