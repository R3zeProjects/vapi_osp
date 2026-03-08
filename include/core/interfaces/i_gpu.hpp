#ifndef VAPI_CORE_I_GPU_HPP
#define VAPI_CORE_I_GPU_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <vector>
#include <string_view>

namespace vapi {

enum class DeviceType : u8 { Unknown, Integrated, Discrete, Virtual, CPU };

enum class QueueCapability : u8 { Graphics = 1, Compute = 2, Transfer = 4, Present = 8 };
inline QueueCapability operator|(QueueCapability a, QueueCapability b) {
    return static_cast<QueueCapability>(static_cast<u8>(a) | static_cast<u8>(b));
}

struct QueueFamilyInfo {
    u32 index{0}; u32 count{0}; u8 capabilities{0};
    bool supports(QueueCapability cap) const {
        return (capabilities & static_cast<u8>(cap)) != 0;
    }
};

struct DeviceProperties {
    std::string name;
    u32 vendorId{0}; u32 deviceId{0}; DeviceType type{DeviceType::Unknown};
    u32 maxTextureSize{0}; u32 maxUniformBufferSize{0};
    u32 maxPushConstantSize{0}; u32 maxBoundDescriptorSets{0};
};

struct GpuInitConfig {
    std::string appName{"Application"};
    bool enableValidation{true};
    u32 framesInFlight{2};
};

class IDeviceWaitable {
public:
    virtual ~IDeviceWaitable() = default;
    virtual Result<void> waitIdle() = 0;
};

class IExtensionQuery {
public:
    virtual ~IExtensionQuery() = default;
    virtual std::vector<std::string> getAvailableExtensions() const = 0;
    virtual bool isExtensionSupported(std::string_view name) const = 0;
};

class IGpuProvider {
public:
    virtual ~IGpuProvider() = default;
    virtual Result<void> init(const GpuInitConfig& config) = 0;
    virtual Result<void> shutdown() = 0;
    virtual bool isInitialized() const = 0;
    virtual DeviceProperties getDeviceProperties() const = 0;
};

} // namespace vapi

#endif
