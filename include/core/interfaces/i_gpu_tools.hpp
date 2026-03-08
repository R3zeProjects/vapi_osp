/** @file i_gpu_tools.hpp
 *  @brief GPU-side tool interfaces: timestamp/query pool, shader cache.
 *  When using Vulkan implementations (VkGpuProfiler, VkGpuFence): pass the current VkCommandBuffer
 *  from the place where you record commands as nativeCommandBuffer; for the fence, use nativeFence()
 *  in vkQueueSubmit. No separate injection layer is required. */

#ifndef VAPI_CORE_I_GPU_TOOLS_HPP
#define VAPI_CORE_I_GPU_TOOLS_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vapi {

/** GPU timestamp / timing query. */
struct GpuTimingResult {
    f64 beginSeconds{0};
    f64 endSeconds{0};
    f64 durationSeconds{0};
};

/** GPU timing / query pool abstraction. */
class IGpuProfiler {
public:
    virtual ~IGpuProfiler() = default;
    /** Write timestamp at current position. nativeCommandBuffer: backend-specific (e.g. VkCommandBuffer for Vulkan); can be null for stub. */
    virtual void writeTimestamp(u32 queryIndex, void* nativeCommandBuffer = nullptr) = 0;
    /** Get result for a pair of timestamps (begin=queryIndex, end=queryIndex+1). */
    [[nodiscard]] virtual Result<GpuTimingResult> getResult(u32 queryIndex) const = 0;
    /** Reset query pool for next frame. nativeCommandBuffer: backend-specific; can be null for stub. */
    virtual void reset(void* nativeCommandBuffer = nullptr) = 0;
    [[nodiscard]] virtual u32 queryCount() const = 0;
};

/** Shader bytecode cache / compiler abstraction. */
struct ShaderCompileOptions {
    std::string entryPoint{"main"};
    bool debug{false};
};

class IShaderCache {
public:
    virtual ~IShaderCache() = default;
    /** Get or compile shader; returns SPIR-V bytecode. */
    [[nodiscard]] virtual Result<std::vector<u32>> getOrCompile(
        std::string_view key,
        std::string_view source,
        ShaderCompileOptions options = {}) = 0;
    virtual void clear() = 0;
};

/** Fence / timeline abstraction for GPU sync. */
class IGpuFence {
public:
    virtual ~IGpuFence() = default;
    /** Wait for fence to be signaled (timeout in ms; ~0u = no timeout). */
    [[nodiscard]] virtual Result<void> wait(u64 timeoutMs = ~0u) = 0;
    /** Check if signaled without blocking. */
    [[nodiscard]] virtual bool isSignaled() const = 0;
    /** Reset fence to unsignaled for reuse (e.g. before next submit). */
    virtual void reset() = 0;
};

/** Compile shader from source (e.g. GLSL) to SPIR-V. No caching; use IShaderCache for cache. */
struct ShaderSourceCompileOptions {
    std::string entryPoint{"main"};
    bool debug{false};
    std::string sourceLanguage{"glsl"};  // "glsl", "hlsl", etc.
};

class IShaderSourceCompiler {
public:
    virtual ~IShaderSourceCompiler() = default;
    /** Compile source to SPIR-V bytecode. */
    [[nodiscard]] virtual Result<std::vector<u32>> compileFromSource(
        std::string_view source,
        std::string_view stageHint,  // "vertex", "fragment", "compute", etc.
        ShaderSourceCompileOptions options = {}) = 0;
};

} // namespace vapi

#endif
