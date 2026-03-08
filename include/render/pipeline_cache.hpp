/** @file pipeline_cache.hpp
 *  @brief Vulkan pipeline cache: load/save to speed up pipeline creation across runs. */

#ifndef VAPI_RENDER_PIPELINE_CACHE_HPP
#define VAPI_RENDER_PIPELINE_CACHE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <vulkan/vulkan.h>
#include <filesystem>

namespace vapi {

/**
 * RAII wrapper for VkPipelineCache. Use when building pipelines to reuse
 * compiled state across runs. Optional load from file on init, save to file at shutdown.
 */
class PipelineCache {
public:
    PipelineCache() = default;
    ~PipelineCache();

    PipelineCache(const PipelineCache&) = delete;
    PipelineCache& operator=(const PipelineCache&) = delete;
    PipelineCache(PipelineCache&& other) noexcept;
    PipelineCache& operator=(PipelineCache&& other) noexcept;

    /** Create empty pipeline cache.
     *  @param device Valid VkDevice; must remain valid for the lifetime of this cache (until shutdown() or destruction).
     *  @return Ok on success.
     *  @pre device != VK_NULL_HANDLE. */
    [[nodiscard]] Result<void> init(VkDevice device);
    /** Create pipeline cache from file (e.g. previous run). If file missing or invalid, creates empty cache (returns Ok).
     *  @param device Valid VkDevice; must remain valid for the lifetime of this cache.
     *  @param path Path to cache file (may not exist).
     *  @return Ok on success (empty cache if file unusable).
     *  @pre device != VK_NULL_HANDLE. */
    [[nodiscard]] Result<void> initFromFile(VkDevice device, const std::filesystem::path& path);
    /** Destroy cache. Safe to call when not initialized. device must be the same as passed to init. */
    void shutdown(VkDevice device);

    /** Save cache data to file (e.g. before exit). Call after pipelines are created.
     *  @param path Output file path.
     *  @return Ok on success; error if not initialized or I/O failure.
     *  @pre init() or initFromFile() succeeded; cache still valid. */
    [[nodiscard]] Result<void> saveToFile(const std::filesystem::path& path) const;

    [[nodiscard]] VkPipelineCache handle() const { return m_cache; }
    [[nodiscard]] explicit operator bool() const { return m_cache != VK_NULL_HANDLE; }

private:
    VkPipelineCache m_cache{VK_NULL_HANDLE};
    VkDevice        m_device{VK_NULL_HANDLE};
};

} // namespace vapi

#endif // VAPI_RENDER_PIPELINE_CACHE_HPP
