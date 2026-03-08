#ifndef VAPI_GPU_VK_CHECK_HPP
#define VAPI_GPU_VK_CHECK_HPP

#include "core/logger.hpp"
#include <vulkan/vulkan.h>
#include <string>

namespace vapi {

#define VK_CHECK_LOG(call) do { \
    VkResult _r = (call); \
    if (_r != VK_SUCCESS) { \
        vapi::logError("Vulkan", "Vulkan error " + std::to_string(static_cast<int>(_r))); \
    } \
} while(0)

} // namespace vapi

#endif
