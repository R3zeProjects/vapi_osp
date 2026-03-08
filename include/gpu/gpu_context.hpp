/** @file gpu_context.hpp
 *  @brief Single GPU facade: Vulkan instance, device, surface, and swapchain in one init flow. */

#ifndef VAPI_GPU_GPU_CONTEXT_HPP
#define VAPI_GPU_GPU_CONTEXT_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_instance.hpp"
#include "gpu/vk_device.hpp"
#include "gpu/vk_surface.hpp"
#include "gpu/vk_swapchain.hpp"
#include <memory>
#include <vector>

namespace vapi {

/** Configuration for the full GPU context (instance + device + optional surface + optional swapchain).
 *  windowHandle must remain valid for the lifetime of the context if non-null (used for surface). */
struct GpuContextConfig {
    InstanceConfig instance;
    /** Device config; surface is set by the context when windowHandle is non-null. */
    DeviceConfig device;
    /** Optional native window handle for surface creation (e.g. GLFWwindow*). If non-null, must be valid and outlive the context. Pass nullptr for headless. */
    void* windowHandle{nullptr};
    /** Swapchain extent; used only when windowHandle is non-null and createSwapchain is true. */
    u32 swapchainWidth{0};
    u32 swapchainHeight{0};
    /** Create swapchain after device (requires windowHandle and non-zero extent). */
    bool createSwapchain{false};
    /** VSync for swapchain. */
    bool vsync{true};
};

/**
 * Single facade that owns Vulkan instance, logical device, optional surface, and optional swapchain.
 * Init order: instance -> surface (if window) -> device -> swapchain (if requested).
 * Shutdown order: swapchain -> surface -> device -> instance.
 */
class GpuContext {
public:
    GpuContext() = default;
    ~GpuContext();

    GpuContext(const GpuContext&) = delete;
    GpuContext& operator=(const GpuContext&) = delete;
    GpuContext(GpuContext&&) noexcept = default;
    GpuContext& operator=(GpuContext&&) noexcept = default;

    /** Initialize instance, optional surface (from windowHandle), device, and optional swapchain. cfg.windowHandle must be valid and outlive the context if non-null.
     *  @param cfg Config; windowHandle (if non-null) must outlive this GpuContext.
     *  @return Ok on success.
     *  @pre cfg.windowHandle either nullptr or a valid window handle for the context lifetime. */
    [[nodiscard]] Result<void> init(const GpuContextConfig& cfg);
    /** Shutdown in reverse order: swapchain -> surface -> device -> instance. */
    void shutdown();

    [[nodiscard]] bool isInitialized() const { return m_instance && m_instance->handle() != VK_NULL_HANDLE; }

    /** Non-owning; valid until shutdown(). */
    [[nodiscard]] VkInstanceWrapper*       instance() { return m_instance.get(); }
    [[nodiscard]] const VkInstanceWrapper* instance() const { return m_instance.get(); }
    [[nodiscard]] VkDeviceWrapper*        device() { return m_device.get(); }
    [[nodiscard]] const VkDeviceWrapper*  device() const { return m_device.get(); }
    /** Non-null only when created with a non-null windowHandle. Valid until shutdown(). */
    [[nodiscard]] VkSurfaceWrapper*        surface() { return m_surface.get(); }
    [[nodiscard]] const VkSurfaceWrapper* surface() const { return m_surface.get(); }
    /** Non-null only when createSwapchain was true and init succeeded. Valid until shutdown(). */
    [[nodiscard]] VkSwapchainWrapper*     swapchain() { return m_swapchain.get(); }
    [[nodiscard]] const VkSwapchainWrapper* swapchain() const { return m_swapchain.get(); }

    /** Recreate primary swapchain (e.g. after resize). @pre init() succeeded; width, height > 0. */
    [[nodiscard]] Result<void> recreateSwapchain(u32 width, u32 height);
    /** Recreate swapchain by pointer. swapchain must be primary swapchain() or one returned by createSwapchain().
     *  @param swapchain Non-null, must be owned by this context. */
    [[nodiscard]] Result<void> recreateSwapchain(VkSwapchainWrapper* swapchain, u32 width, u32 height);

    /** Create an additional surface. windowHandle must remain valid until destroySurface().
     *  @return Non-owning pointer; valid until destroySurface() for this surface. */
    [[nodiscard]] Result<VkSurfaceWrapper*> createSurface(void* windowHandle);
    /** Create swapchain for an additional surface. surface must be from createSurface(). surface must outlive this swapchain.
     *  @return Non-owning pointer; valid until destroySwapchain() for this swapchain. */
    [[nodiscard]] Result<VkSwapchainWrapper*> createSwapchain(VkSurfaceWrapper* surface, u32 width, u32 height, bool vsync = true);
    /** Destroy additional swapchain. swapchain must be from createSwapchain(), not primary. */
    [[nodiscard]] Result<void> destroySwapchain(VkSwapchainWrapper* swapchain);
    /** Destroy additional surface. Call destroySwapchain for any swapchains using it first. */
    [[nodiscard]] Result<void> destroySurface(VkSurfaceWrapper* surface);

private:
    std::unique_ptr<VkInstanceWrapper>   m_instance;
    std::unique_ptr<VkDeviceWrapper>    m_device;
    std::unique_ptr<VkSurfaceWrapper>   m_surface;
    std::unique_ptr<VkSwapchainWrapper> m_swapchain;
    std::vector<std::unique_ptr<VkSurfaceWrapper>>   m_additionalSurfaces;
    std::vector<std::unique_ptr<VkSwapchainWrapper>>  m_additionalSwapchains;
    GpuContextConfig                     m_lastConfig{};
};

} // namespace vapi

#endif // VAPI_GPU_GPU_CONTEXT_HPP
