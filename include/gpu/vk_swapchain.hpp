/** @file vk_swapchain.hpp
 *  @brief Vulkan swapchain wrapper: images, image views, acquire, present. */

#ifndef VAPI_GPU_VK_SWAPCHAIN_HPP
#define VAPI_GPU_VK_SWAPCHAIN_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include "gpu/vk_base.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vapi {

class VkDeviceWrapper;

/** Swapchain creation config. surface must be valid at create() and remain valid for swapchain lifetime. */
struct SwapchainConfig {
    VkSurfaceKHR surface{VK_NULL_HANDLE};
    u32 width{0};
    u32 height{0};
    bool vsync{true};
    VkFormat preferredFormat{VK_FORMAT_B8G8R8A8_SRGB};
    VkColorSpaceKHR preferredColorSpace{VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
};

class VkSwapchainWrapper {
public:
    VkSwapchainWrapper() = default;
    ~VkSwapchainWrapper();

    VkSwapchainWrapper(const VkSwapchainWrapper&) = delete;
    VkSwapchainWrapper& operator=(const VkSwapchainWrapper&) = delete;

    /** Create swapchain. device must be valid and outlive this wrapper; cfg.surface must be valid and outlive this swapchain.
     *  @pre device valid; cfg.surface != VK_NULL_HANDLE; width/height > 0. */
    [[nodiscard]] Result<void> create(const VkDeviceWrapper& device, const SwapchainConfig& cfg);
    /** Recreate swapchain with new extent. device must be the same valid device used at create().
     *  @pre device valid; width, height > 0. */
    [[nodiscard]] Result<void> recreate(const VkDeviceWrapper& device, u32 width, u32 height);
    /** Destroy swapchain. device must be the same VkDevice used at create()/recreate(). */
    void destroy(VkDevice device);

    /** Get next image index. Returns SwapchainOutOfDate when swapchain needs to be recreated.
     *  @param device Valid device that owns this swapchain.
     *  @return Image index or error (e.g. SwapchainOutOfDate). */
    [[nodiscard]] Result<u32> acquireNextImage(VkDevice device, VkSemaphore semaphore, u64 timeout = UINT64_MAX);

    /** Present image. queue must be present queue for this swapchain. */
    [[nodiscard]] Result<void> present(VkQueue queue, u32 imageIndex, VkSemaphore waitSemaphore);

    // Getters
    [[nodiscard]] VkSwapchainKHR   handle()     const { return m_swapchain; }
    [[nodiscard]] VkFormat         format()     const { return m_format; }
    [[nodiscard]] VkExtent2D       extent()     const { return m_extent; }
    [[nodiscard]] u32              imageCount() const { return static_cast<u32>(m_images.size()); }
    [[nodiscard]] VkImage          image(u32 i) const { return m_images[i]; }
    [[nodiscard]] VkImageView      imageView(u32 i) const { return m_imageViews[i]; }
    [[nodiscard]] const std::vector<VkImage>&     images()     const { return m_images; }
    [[nodiscard]] const std::vector<VkImageView>& imageViews() const { return m_imageViews; }
    [[nodiscard]] bool needsRecreation() const { return m_needsRecreation; }

    explicit operator bool() const { return m_swapchain != VK_NULL_HANDLE; }

private:
    void destroyImageViews(VkDevice device);
    [[nodiscard]] Result<void> createImageViews(VkDevice device);
    [[nodiscard]] VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats, const SwapchainConfig& cfg);
    [[nodiscard]] VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& modes, bool vsync);
    [[nodiscard]] VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& caps, u32 w, u32 h);

    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    VkFormat       m_format{VK_FORMAT_UNDEFINED};
    VkExtent2D     m_extent{0, 0};
    SwapchainConfig m_lastConfig{};

    std::vector<VkImage>     m_images;
    std::vector<VkImageView> m_imageViews;
    bool m_needsRecreation{false};
};

} // namespace vapi

#endif
