#ifndef VAPI_RESOURCE_DESCRIPTOR_MANAGER_HPP
#define VAPI_RESOURCE_DESCRIPTOR_MANAGER_HPP

#include "core/error.hpp"
#include "resource/resource_types.hpp"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace vapi {

class VkDeviceWrapper;

/**
 * Descriptor manager: layouts, pool, and descriptor set allocation.
 *
 * Contract:
 * - Ownership: the caller owns the VkDeviceWrapper* passed to init(); the manager only stores the pointer.
 *   The device must remain valid until shutdown() is called.
 * - On init() failure: the object state is undefined; the caller must not use the manager until init() succeeds.
 *   It is safe to call shutdown() or to call init() again after a failed init().
 * - Descriptor shortage: allocateSet() and allocateSets() return an error (e.g. errors::LimitReached or
 *   gpu_errors::DescriptorPool) when the pool has insufficient descriptors; behaviour is not undefined.
 */
class DescriptorManager {
public:
    DescriptorManager() = default;
    ~DescriptorManager() = default;

    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device);
    void shutdown();

    // ── Layouts ───────────────────────────────────────────────
    [[nodiscard]] Result<DescLayoutId> createLayout(const DescriptorLayoutDesc& desc);
    [[nodiscard]] VkDescriptorSetLayout getLayout(DescLayoutId id) const;

    // ── Pool ──────────────────────────────────────────────────
    [[nodiscard]] Result<void> createPool(const std::vector<DescriptorPoolSize>& sizes, u32 maxSets);
    void resetPool();

    // ── Sets ──────────────────────────────────────────────────
    /** Allocates one descriptor set. Returns error (e.g. LimitReached) when pool has insufficient descriptors. */
    [[nodiscard]] Result<VkDescriptorSet> allocateSet(DescLayoutId layoutId);
    /** Allocates multiple descriptor sets. Returns error when pool has insufficient descriptors. */
    [[nodiscard]] Result<std::vector<VkDescriptorSet>> allocateSets(DescLayoutId layoutId, u32 count);

    // ── Write ─────────────────────────────────────────────────
    void writeBuffer(VkDescriptorSet set, u32 binding, VkBuffer buffer, VkDeviceSize size,
                     VkDeviceSize offset = 0, VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    void writeImage(VkDescriptorSet set, u32 binding, VkImageView view, VkSampler sampler,
                    VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VkDescriptorType type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    void destroyAll();

private:
    Result<VkDescriptorSet> allocateSetFromPool(VkDescriptorSetLayout layout, VkDescriptorPool pool);
    Result<void> createOnePool();
    [[nodiscard]] VkDescriptorSetLayout getLayoutUnlocked(DescLayoutId id) const;

    const VkDeviceWrapper* m_device{nullptr};
    std::unordered_map<DescLayoutId, VkDescriptorSetLayout> m_layouts;
    std::vector<VkDescriptorPool> m_pools;
    std::vector<VkDescriptorPoolSize> m_poolSizes;
    u32 m_maxSets{0};
    u32 m_nextLayoutId{1};
    mutable std::mutex m_mutex;
};

} // namespace vapi

#endif
