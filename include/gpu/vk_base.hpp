#ifndef VAPI_GPU_VK_BASE_HPP
#define VAPI_GPU_VK_BASE_HPP

#include "core/types.hpp"
#include "core/interfaces/i_gpu.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace vapi {

[[nodiscard]] inline VkExtent2D toVkExtent(u32 w, u32 h) { return {w, h}; }
[[nodiscard]] inline VkExtent2D toVkExtent(SizeElement s) { return {s.width, s.height}; }
[[nodiscard]] inline SizeElement toSizeElement(VkExtent2D e) { return SizeElement(e.width, e.height); }

[[nodiscard]] inline VkClearColorValue toVkClearColor(color4 c) {
    VkClearColorValue v{};
    v.float32[0] = c.r; v.float32[1] = c.g; v.float32[2] = c.b; v.float32[3] = c.a;
    return v;
}

[[nodiscard]] inline DeviceType toDeviceType(VkPhysicalDeviceType t) {
    switch (t) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return DeviceType::Integrated;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return DeviceType::Discrete;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return DeviceType::Virtual;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return DeviceType::CPU;
        default: return DeviceType::Unknown;
    }
}

struct QueueFamilyIndices {
    u32 graphics{UINT32_MAX};
    u32 present{UINT32_MAX};
    u32 compute{UINT32_MAX};
    u32 transfer{UINT32_MAX};

    [[nodiscard]] bool isComplete() const {
        return graphics != UINT32_MAX && present != UINT32_MAX;
    }

    [[nodiscard]] std::vector<u32> uniqueFamilies() const {
        std::vector<u32> fams;
        auto add = [&](u32 f) {
            if (f != UINT32_MAX) {
                for (auto x : fams) if (x == f) return;
                fams.push_back(f);
            }
        };
        add(graphics); add(present); add(compute); add(transfer);
        return fams;
    }
};

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    [[nodiscard]] bool isAdequate() const {
        return !formats.empty() && !presentModes.empty();
    }
};

} // namespace vapi

#endif
