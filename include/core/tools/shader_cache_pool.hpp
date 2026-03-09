/** @file shader_cache_pool.hpp
 *  @brief Helper to run IShaderCache::getOrCompile on ICpuServices thread pool when available. */

#ifndef VAPI_CORE_TOOLS_SHADER_CACHE_POOL_HPP
#define VAPI_CORE_TOOLS_SHADER_CACHE_POOL_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_cpu_bridge.hpp"
#include "core/interfaces/i_gpu_tools.hpp"
#include <string_view>
#include <vector>

namespace vapi {

/** If cpuServices and getThreadPool() are non-null, runs cache.getOrCompile(key, source, options) on the pool and returns the result; otherwise calls getOrCompile on the current thread. */
[[nodiscard]] Result<std::vector<u32>> getOrCompileOnPool(ICpuServices* cpuServices,
                                                         IShaderCache& cache,
                                                         std::string_view key,
                                                         std::string_view source,
                                                         ShaderCompileOptions options = {});

} // namespace vapi

#endif
