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

    /** When transferQueue and transferPool are both non-null, copy commands are submitted to the transfer queue instead of graphics. */
    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device, VkCommandManager* commands,
                                    VkQueue transferQueue = VK_NULL_HANDLE,
                                    VkCommandPool transferPool = VK_NULL_HANDLE);
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

    // ── Deferred upload (submit on flush) ─────────────────────────────
    /** Start deferred uploads. Call uploadBufferDeferred/uploadImageDeferred, then flushStaging(). Data is available on GPU only after flushStaging(). */
    [[nodiscard]] Result<void> startDeferred();
    [[nodiscard]] Result<void> uploadBufferDeferred(VkBuffer dstBuffer, std::span<const u8> data, VkDeviceSize dstOffset = 0,
                                                     std::optional<VkDeviceSize> dstSize = std::nullopt);
    [[nodiscard]] Result<void> uploadBufferDeferred(BufferManager& bufferManager, BufferId dstId, std::span<const u8> data,
                                                     VkDeviceSize dstOffset = 0);
    [[nodiscard]] Result<void> uploadImageDeferred(VkImage dstImage, u32 width, u32 height, std::span<const u8> data,
                                                    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] Result<void> uploadImageDeferred(ImageManager& imageManager, ImageId dstId, std::span<const u8> data,
                                                    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    /** Submit all deferred uploads and wait. Call after startDeferred() and one or more *Deferred calls. */
    [[nodiscard]] Result<void> flushStaging();

private:
    struct StagingBuffer {
        VkBuffer       buffer{VK_NULL_HANDLE};
        VkDeviceMemory memory{VK_NULL_HANDLE};
        VkDeviceSize   size{0};
    };

    struct RingSlot {
        StagingBuffer buf;
        VkFence       fence{VK_NULL_HANDLE};
    };

    [[nodiscard]] Result<StagingBuffer> createStagingBuffer(VkDeviceSize size);
    void destroyStagingBuffer(StagingBuffer& buf);
    [[nodiscard]] Result<void> ensurePersistentStaging(VkDeviceSize minSize);
    void destroyPersistentStaging();
    [[nodiscard]] Result<RingSlot*> getRingSlot(VkDeviceSize minSize);
    void destroyRingSlots();

    const VkDeviceWrapper* m_device{nullptr};
    VkCommandManager*     m_commands{nullptr};
    VkQueue                m_transferQueue{VK_NULL_HANDLE};
    VkCommandPool          m_transferPool{VK_NULL_HANDLE};

    StagingBuffer          m_persistentStaging;
    static constexpr VkDeviceSize kPersistentStagingSize = 2ull * 1024 * 1024;

    static constexpr u32    kRingSlots = 4;
    std::vector<RingSlot>  m_ringSlots;
    u32                     m_ringNextIndex{0};

    bool                           m_batchActive{false};
    VkCommandBuffer                m_batchCmd{VK_NULL_HANDLE};
    std::vector<StagingBuffer>     m_batchStagingBuffers;
};

} // namespace vapi

#endif
