#ifndef VAPI_RESOURCE_BUFFER_MANAGER_HPP
#define VAPI_RESOURCE_BUFFER_MANAGER_HPP

#include "core/error.hpp"
#include "resource/resource_types.hpp"
#include <vulkan/vulkan.h>
#include <mutex>
#include <span>
#include <unordered_map>
#include <vector>

namespace vapi {

class VkDeviceWrapper;

/** Configuration for BufferManager. Allows tuning limits without recompilation. */
struct BufferManagerConfig {
    /** Max size per buffer in bytes. Default 256 MiB. Set to 0 to use default. */
    VkDeviceSize maxBufferSize{256ull * 1024 * 1024};
};

/**
 * Buffer manager: creates and manages GPU buffers.
 *
 * Contract:
 * - Ownership: the caller owns the VkDeviceWrapper* passed to init(); the manager only stores the pointer.
 *   The device must remain valid until shutdown() is called.
 * - On init() failure: the object state is undefined; the caller must not use the manager until init() succeeds.
 *   It is safe to call shutdown() or to call init() again after a failed init().
 */
class BufferManager {
public:
    BufferManager() = default;
    ~BufferManager() { shutdown(); }
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;
    BufferManager(BufferManager&&) noexcept = default;
    BufferManager& operator=(BufferManager&&) noexcept = default;

    /** Initialize with default config (max buffer size 256 MiB). */
    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device);
    /** Initialize with custom config (e.g. higher maxBufferSize for large assets). */
    [[nodiscard]] Result<void> init(const VkDeviceWrapper* device, const BufferManagerConfig& config);
    void shutdown();

    /// Create buffer
    [[nodiscard]] Result<BufferId> createBuffer(const BufferDesc& desc);

    /// Destroy buffer
    [[nodiscard]] Result<void> destroyBuffer(BufferId id);

    /// Destroy all buffers
    void destroyAll();

    // ── Mapping ───────────────────────────────────────────────
    [[nodiscard]] Result<void*> map(BufferId id, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
    void unmap(BufferId id);
    [[nodiscard]] bool isMapped(BufferId id) const;

    // ── Write/Read ────────────────────────────────────────────
    [[nodiscard]] Result<void> write(BufferId id, const void* data, usize size, usize offset = 0);
    [[nodiscard]] Result<void> write(BufferId id, std::span<const u8> data, usize offset = 0);
    [[nodiscard]] Result<void> flush(BufferId id, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

    // ── Getters ───────────────────────────────────────────────
    [[nodiscard]] const GpuBuffer* get(BufferId id) const;
    [[nodiscard]] VkBuffer handle(BufferId id) const;
    [[nodiscard]] bool exists(BufferId id) const;

    /** Current max buffer size (from config or default). */
    [[nodiscard]] VkDeviceSize maxBufferSize() const { return m_maxBufferSize; }

private:
    [[nodiscard]] VkMemoryPropertyFlags toVkMemoryProps(MemoryUsage usage) const;
    [[nodiscard]] VkBufferUsageFlags toVkBufferUsage(BufferUsage usage) const;

    const VkDeviceWrapper* m_device{nullptr};
    VkDeviceSize          m_maxBufferSize{256ull * 1024 * 1024};
    std::unordered_map<BufferId, GpuBuffer> m_buffers;
    BufferId m_nextId{1};
    std::vector<BufferId> m_freeIds;
    mutable std::mutex m_mutex;
};

} // namespace vapi

#endif
