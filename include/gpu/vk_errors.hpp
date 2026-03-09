#ifndef VAPI_GPU_VK_ERRORS_HPP
#define VAPI_GPU_VK_ERRORS_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/logger.hpp"
#include <optional>
#include <string>
#include <vulkan/vulkan.h>

namespace vapi {

namespace gpu_errors {
    inline constexpr Error VkFailed{0x4001};
    inline constexpr Error DeviceNotFound{0x4002};
    inline constexpr Error NoSuitableQueue{0x4003};
    inline constexpr Error SurfaceCreation{0x4004};
    inline constexpr Error SwapchainCreation{0x4005};
    inline constexpr Error SwapchainOutOfDate{0x4006};
    inline constexpr Error ImageAcquire{0x4007};
    inline constexpr Error PresentFailed{0x4008};
    inline constexpr Error FenceTimeout{0x4009};
    inline constexpr Error CommandPoolCreation{0x400A};
    inline constexpr Error CommandBufferAlloc{0x400B};
    inline constexpr Error AllocatorCreation{0x400C};
    inline constexpr Error AllocationFailed{0x400D};
    inline constexpr Error MapFailed{0x400E};
    inline constexpr Error ValidationLayerNotAvailable{0x400F};
    inline constexpr Error ExtensionNotAvailable{0x4010};
    inline constexpr Error ShaderModuleCreation{0x4011};
    inline constexpr Error PipelineCreation{0x4012};
    inline constexpr Error RenderPassCreation{0x4013};
    inline constexpr Error FramebufferCreation{0x4014};
    inline constexpr Error DescriptorSetLayout{0x4015};
    inline constexpr Error DescriptorPool{0x4016};
    inline constexpr Error BufferCreation{0x4017};
    inline constexpr Error ImageCreation{0x4018};
    inline constexpr Error SamplerCreation{0x4019};
    inline constexpr Error SemaphoreCreation{0x401A};
    inline constexpr Error FenceCreation{0x401B};
    inline constexpr Error QueryPoolCreation{0x401C};
}

[[nodiscard]] inline Error toError(VkResult r) {
    switch (r) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            return errors::Ok;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return errors::OutOfMemory;
        case VK_ERROR_DEVICE_LOST:
            return gpu_errors::VkFailed;
        case VK_TIMEOUT:
            return errors::Timeout;
        case VK_NOT_READY:
            return errors::NotReady;
        case VK_ERROR_OUT_OF_DATE_KHR:
            return gpu_errors::SwapchainOutOfDate;
        default:
            return gpu_errors::VkFailed;
    }
}

/** Shared VkResult check: returns error if not success/suboptimal; optionally logs when logIfError. */
[[nodiscard]] inline std::optional<Error> vkCheckResult(VkResult vr, bool logIfError = false) {
    if (vr == VK_SUCCESS || vr == VK_SUBOPTIMAL_KHR) return std::nullopt;
    Error e = toError(vr);
    if (logIfError) logError("Vulkan", "Vulkan error " + std::to_string(static_cast<int>(vr)));
    return e;
}

#define VK_CHECK(expr) \
    do { VkResult _vr = (expr); auto _vkErr = vapi::vkCheckResult(_vr); if (_vkErr) return std::unexpected(*_vkErr); } while(0)

#define VK_CHECK_BOOL(expr) \
    do { VkResult _vr = (expr); if (_vr != VK_SUCCESS) return false; } while(0)

} // namespace vapi

#endif
