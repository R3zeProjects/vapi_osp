#ifndef VAPI_GPU_VK_CHECK_HPP
#define VAPI_GPU_VK_CHECK_HPP

#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>

namespace vapi {

#define VK_CHECK_LOG(call) do { (void)vapi::vkCheckResult((call), true); } while(0)

} // namespace vapi

#endif
