#ifndef VAPI_RESOURCE_IMAGE_MANAGER_HPP
#define VAPI_RESOURCE_IMAGE_MANAGER_HPP

#include "core/error.hpp"
#include "resource/resource_types.hpp"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <mutex>

namespace vapi {

class VkDeviceWrapper;
class VkCommandManager;

/**
 * Image manager: creates and manages GPU images and views.
 *
 * Contract:
 * - Ownership: the caller owns the VkDeviceWrapper* passed to init(); the manager only stores the pointer.
 *   The device must remain valid until shutdown() is called.
 * - On init() failure: the object state is undefined; the caller must not use the manager until init() succeeds.
 *   It is safe to call shutdown() or to call init() again after a failed init().
 */
class ImageManager {
public:
    ImageManager() = default;
    ~ImageManager() { shutdown(); }
    ImageManager(const ImageManager&) = delete;
    ImageManager& operator=(const ImageManager&) = delete;
    ImageManager(ImageManager&&) noexcept = default;
    ImageManager& operator=(ImageManager&&) noexcept = default;

    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device);
    void shutdown();

    /// Create image and view
    [[nodiscard]] Result<ImageId> createImage(const ImageDesc& desc);

    /// Create depth/stencil image
    [[nodiscard]] Result<ImageId> createDepthImage(u32 width, u32 height, VkFormat format = VK_FORMAT_D32_SFLOAT);

    /// Destroy image
    [[nodiscard]] Result<void> destroyImage(ImageId id);
    void destroyAll();

    // ── Getters ───────────────────────────────────────────────
    [[nodiscard]] const GpuImage* get(ImageId id) const;
    [[nodiscard]] VkImage handle(ImageId id) const;
    [[nodiscard]] VkImageView view(ImageId id) const;
    [[nodiscard]] bool exists(ImageId id) const;

private:
    [[nodiscard]] Result<VkImageView> createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, u32 mipLevels);

    const VkDeviceWrapper* m_device{nullptr};
    std::unordered_map<ImageId, GpuImage> m_images;
    ImageId m_nextId{ImageId{1}};
    std::vector<ImageId> m_freeIds;
    mutable std::mutex m_mutex;
};

} // namespace vapi

#endif
