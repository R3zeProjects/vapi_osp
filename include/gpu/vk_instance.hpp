/** @file vk_instance.hpp
 *  @brief Vulkan instance wrapper with optional validation layer. */

#ifndef VAPI_GPU_VK_INSTANCE_HPP
#define VAPI_GPU_VK_INSTANCE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace vapi {

struct InstanceConfig {
    std::string appName{"VAPI Application"};
    u32 appVersion{VK_MAKE_VERSION(1, 0, 0)};
    bool enableValidation{true};
    std::vector<const char*> requiredExtensions;  // от платформы
    std::vector<const char*> extraLayers;
};

class VkInstanceWrapper {
public:
    VkInstanceWrapper() = default;
    ~VkInstanceWrapper();

    VkInstanceWrapper(const VkInstanceWrapper&) = delete;
    VkInstanceWrapper& operator=(const VkInstanceWrapper&) = delete;
    VkInstanceWrapper(VkInstanceWrapper&&) noexcept;
    VkInstanceWrapper& operator=(VkInstanceWrapper&&) noexcept;

    /** Create Vulkan instance. requiredExtensions and extraLayers pointers must remain valid for the duration of create().
     *  @param cfg App name, version, validation, extensions, layers.
     *  @return Ok on success. */
    [[nodiscard]] Result<void> create(const InstanceConfig& cfg);
    /** Destroy instance. Safe when not created. */
    void destroy();

    [[nodiscard]] VkInstance handle() const { return m_instance; }
    [[nodiscard]] bool validationEnabled() const { return m_validationEnabled; }
    explicit operator bool() const { return m_instance != VK_NULL_HANDLE; }

private:
    [[nodiscard]] bool checkValidationLayerSupport() const;
    void setupDebugMessenger();
    void destroyDebugMessenger();

    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
    bool m_validationEnabled{false};
};

} // namespace vapi

#endif
