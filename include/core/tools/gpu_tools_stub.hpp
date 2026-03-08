/** @file gpu_tools_stub.hpp
 *  @brief Stub implementations of IGpuProfiler, IShaderCache, IGpuFence for testing and default behavior. */

#ifndef VAPI_CORE_TOOLS_GPU_TOOLS_STUB_HPP
#define VAPI_CORE_TOOLS_GPU_TOOLS_STUB_HPP

#include "core/interfaces/i_gpu_tools.hpp"

namespace vapi::tools {

/** GPU profiler stub; getResult returns zero duration. */
class GpuProfilerStub : public IGpuProfiler {
public:
    void writeTimestamp(u32 /*queryIndex*/, void* /*nativeCommandBuffer*/) override {}
    [[nodiscard]] Result<GpuTimingResult> getResult(u32 /*queryIndex*/) const override {
        return GpuTimingResult{};
    }
    void reset(void* /*nativeCommandBuffer*/) override {}
    [[nodiscard]] u32 queryCount() const override { return 0; }
};

/** Shader cache stub; getOrCompile returns NotImplemented. */
class ShaderCacheStub : public IShaderCache {
public:
    [[nodiscard]] Result<std::vector<u32>> getOrCompile(
        std::string_view /*key*/, std::string_view /*source*/,
        ShaderCompileOptions /*options*/) override {
        return std::unexpected(errors::NotImplemented);
    }
    void clear() override {}
};

/** Fence stub; always signaled. */
class GpuFenceStub : public IGpuFence {
public:
    [[nodiscard]] Result<void> wait(u64 /*timeoutMs*/) override { return {}; }
    [[nodiscard]] bool isSignaled() const override { return true; }
    void reset() override {}
};

/** Shader source compiler stub; compileFromSource returns NotImplemented. */
class ShaderSourceCompilerStub : public IShaderSourceCompiler {
public:
    [[nodiscard]] Result<std::vector<u32>> compileFromSource(
        std::string_view /*source*/, std::string_view /*stageHint*/,
        ShaderSourceCompileOptions /*options*/) override {
        return std::unexpected(errors::NotImplemented);
    }
};

} // namespace vapi::tools

#endif
