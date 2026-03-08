/** @file vk_gpu_profiler.hpp
 *  @brief Vulkan implementation of IGpuProfiler (timestamp query pool). */

#ifndef VAPI_GPU_VK_GPU_PROFILER_HPP
#define VAPI_GPU_VK_GPU_PROFILER_HPP

#include "core/interfaces/i_gpu_tools.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>

namespace vapi {

/** Vulkan-based GPU profiler. Pass VkCommandBuffer as nativeCommandBuffer in writeTimestamp/reset. */
class VkGpuProfiler : public IGpuProfiler {
public:
    VkGpuProfiler() = default;
    ~VkGpuProfiler() override;

    VkGpuProfiler(const VkGpuProfiler&) = delete;
    VkGpuProfiler& operator=(const VkGpuProfiler&) = delete;

    /** Create timestamp query pool. numRanges = number of begin/end pairs (total queries = 2 * numRanges). */
    [[nodiscard]] Result<void> create(VkDevice device, VkPhysicalDevice physicalDevice, u32 numRanges);
    void destroy(VkDevice device);

    void writeTimestamp(u32 queryIndex, void* nativeCommandBuffer = nullptr) override;
    [[nodiscard]] Result<GpuTimingResult> getResult(u32 queryIndex) const override;
    void reset(void* nativeCommandBuffer = nullptr) override;
    [[nodiscard]] u32 queryCount() const override { return m_queryCount; }

    [[nodiscard]] VkQueryPool pool() const { return m_pool; }

private:
    VkDevice         m_device{VK_NULL_HANDLE};
    VkQueryPool      m_pool{VK_NULL_HANDLE};
    u32              m_queryCount{0};
    double           m_timestampPeriodSeconds{0.0};
};

} // namespace vapi

#endif
