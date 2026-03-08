#ifndef VAPI_RESOURCE_SAMPLER_CACHE_HPP
#define VAPI_RESOURCE_SAMPLER_CACHE_HPP

#include "core/error.hpp"
#include "resource/resource_types.hpp"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <mutex>

namespace vapi {

class VkDeviceWrapper;

/**
 * Sampler cache: same SamplerDesc yields one VkSampler.
 *
 * Contract:
 * - Ownership: the caller owns the VkDeviceWrapper* passed to init(); the cache only stores the pointer.
 *   The device must remain valid until shutdown() is called.
 * - On init() failure: the object state is undefined; the caller must not use the cache until init() succeeds.
 *   It is safe to call shutdown() or to call init() again after a failed init().
 */
class SamplerCache {
public:
    SamplerCache() = default;
    ~SamplerCache() = default;

    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device);
    void shutdown();

    /// Get or create sampler
    [[nodiscard]] Result<VkSampler> getOrCreate(const SamplerDesc& desc);

    void destroyAll();

private:
    struct DescHash {
        usize operator()(const SamplerDesc& d) const;
    };

    [[nodiscard]] VkFilter toVkFilter(FilterMode m) const;
    [[nodiscard]] VkSamplerMipmapMode toVkMipmap(FilterMode m) const;
    [[nodiscard]] VkSamplerAddressMode toVkWrap(WrapMode m) const;

    const VkDeviceWrapper* m_device{nullptr};
    std::unordered_map<SamplerDesc, VkSampler, DescHash> m_cache;
    mutable std::mutex m_mutex;
};

} // namespace vapi

#endif
