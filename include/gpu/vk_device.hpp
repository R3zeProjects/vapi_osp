/** @file vk_device.hpp
 *  @brief Vulkan logical device wrapper: queues, physical device, swapchain support. */

#ifndef VAPI_GPU_VK_DEVICE_HPP
#define VAPI_GPU_VK_DEVICE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_gpu.hpp"
#include "gpu/vk_errors.hpp"
#include "gpu/vk_base.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace vapi {

/** Configuration for logical device creation. instance and surface must be valid at create() and remain valid for the lifetime of the device (surface used for present queue selection). */
struct DeviceConfig {
    VkInstance instance{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};   // for present queue
    std::vector<const char*> requiredExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    bool preferDiscrete{true};
};

class VkDeviceWrapper {
public:
    VkDeviceWrapper() = default;
    ~VkDeviceWrapper();

    VkDeviceWrapper(const VkDeviceWrapper&) = delete;
    VkDeviceWrapper& operator=(const VkDeviceWrapper&) = delete;
    VkDeviceWrapper(VkDeviceWrapper&&) noexcept;
    VkDeviceWrapper& operator=(VkDeviceWrapper&&) noexcept;

    /** Create logical device and obtain queue handles. cfg.instance and cfg.surface must be valid and outlive this wrapper.
     *  @param cfg Instance and surface (non-null for present), extensions, preferDiscrete.
     *  @return Ok on success.
     *  @pre cfg.instance != VK_NULL_HANDLE. */
    [[nodiscard]] Result<void> create(const DeviceConfig& cfg);
    /** Destroy device. Safe when not created. */
    void destroy();

    // Getters
    [[nodiscard]] VkDevice           device()         const { return m_device; }
    [[nodiscard]] VkPhysicalDevice   physicalDevice()  const { return m_physicalDevice; }
    [[nodiscard]] VkQueue            graphicsQueue()   const { return m_graphicsQueue; }
    [[nodiscard]] VkQueue            presentQueue()    const { return m_presentQueue; }
    [[nodiscard]] VkQueue            computeQueue()    const { return m_computeQueue; }
    [[nodiscard]] VkQueue            transferQueue()   const { return m_transferQueue; }
    [[nodiscard]] const QueueFamilyIndices& queueFamilies() const { return m_queueFamilies; }
    [[nodiscard]] const DeviceProperties& properties()     const { return m_properties; }

    /** Wait for all device queues to idle. @return Ok on success. */
    [[nodiscard]] Result<void> waitIdle() const;

    /** Query swapchain support for the given surface. surface must be valid. */
    [[nodiscard]] SwapchainSupportDetails querySwapchainSupport(VkSurfaceKHR surface) const;

    /** Find memory type index. @return Index or error if not found. */
    [[nodiscard]] Result<u32> findMemoryType(u32 typeBits, VkMemoryPropertyFlags props) const;

    explicit operator bool() const { return m_device != VK_NULL_HANDLE; }

private:
    [[nodiscard]] Result<VkPhysicalDevice> pickPhysicalDevice(const DeviceConfig& cfg);
    [[nodiscard]] QueueFamilyIndices findQueueFamilies(VkPhysicalDevice dev, VkSurfaceKHR surface) const;
    [[nodiscard]] bool checkExtensionSupport(VkPhysicalDevice dev, const std::vector<const char*>& exts) const;
    [[nodiscard]] s32 scoreDevice(VkPhysicalDevice dev, const DeviceConfig& cfg) const;

    VkDevice         m_device{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

    VkQueue m_graphicsQueue{VK_NULL_HANDLE};
    VkQueue m_presentQueue{VK_NULL_HANDLE};
    VkQueue m_computeQueue{VK_NULL_HANDLE};
    VkQueue m_transferQueue{VK_NULL_HANDLE};

    QueueFamilyIndices m_queueFamilies;
    DeviceProperties   m_properties;
    VkPhysicalDeviceMemoryProperties m_memProperties{};
};

} // namespace vapi

#endif
