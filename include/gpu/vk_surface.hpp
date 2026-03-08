/** @file vk_surface.hpp
 *  @brief Vulkan surface wrapper (e.g. from GLFW window). */

#ifndef VAPI_GPU_VK_SURFACE_HPP
#define VAPI_GPU_VK_SURFACE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace vapi {

class VkSurfaceWrapper {
public:
    VkSurfaceWrapper() = default;
    ~VkSurfaceWrapper();

    VkSurfaceWrapper(const VkSurfaceWrapper&) = delete;
    VkSurfaceWrapper& operator=(const VkSurfaceWrapper&) = delete;
    VkSurfaceWrapper(VkSurfaceWrapper&&) noexcept;
    VkSurfaceWrapper& operator=(VkSurfaceWrapper&&) noexcept;

    /** Create surface from GLFW window. instance and window must be valid; both must outlive this wrapper (instance required for destroy()).
     *  @param instance Valid VkInstance; must outlive this surface.
     *  @param window Valid GLFW window; must outlive this surface.
     *  @pre instance != VK_NULL_HANDLE, window != nullptr. */
    [[nodiscard]] Result<void> create(VkInstance instance, GLFWwindow* window);
    /** Destroy surface. Safe when not created. */
    void destroy();

    [[nodiscard]] VkSurfaceKHR handle() const { return m_surface; }
    explicit operator bool() const { return m_surface != VK_NULL_HANDLE; }

private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkInstance    m_instance{VK_NULL_HANDLE};  // for cleanup
};

} // namespace vapi

#endif
