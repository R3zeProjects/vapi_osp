/** @file gpu_tools_stub.hpp
 *  @brief Stub implementations of IGpuProfiler, IShaderCache, IGpuFence, IShaderSourceCompiler for testing and default behavior. */

#ifndef VAPI_CORE_TOOLS_GPU_TOOLS_STUB_HPP
#define VAPI_CORE_TOOLS_GPU_TOOLS_STUB_HPP

#include "core/interfaces/i_gpu_tools.hpp"
#include <unordered_map>

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

/** Shader cache stub: in-memory cache only; getOrCompile returns cached SPIR-V if key was injected, else NotImplemented. */
class ShaderCacheStub : public IShaderCache {
public:
    /** Preload cache for key (e.g. for tests or precompiled shaders). getOrCompile(key, ...) will return this bytecode. */
    void inject(std::string_view key, std::vector<u32> spirv) {
        m_cache[std::string(key)] = std::move(spirv);
    }

    [[nodiscard]] Result<std::vector<u32>> getOrCompile(
        std::string_view key, std::string_view /*source*/,
        ShaderCompileOptions /*options*/) override {
        auto it = m_cache.find(std::string(key));
        if (it != m_cache.end())
            return it->second;
        return std::unexpected(errors::NotImplemented);
    }
    void clear() override { m_cache.clear(); }

private:
    std::unordered_map<std::string, std::vector<u32>> m_cache;
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

/**
 * Non-owning holder for GPU tools (profiler, shader cache, fence, compiler).
 * Use with stub or real implementations; any pointer may be nullptr.
 */
class GpuServicesHolder {
public:
    GpuServicesHolder(IGpuProfiler* profiler = nullptr, IShaderCache* shaderCache = nullptr,
                      IGpuFence* fence = nullptr, IShaderSourceCompiler* compiler = nullptr)
        : m_profiler(profiler), m_shaderCache(shaderCache), m_fence(fence), m_compiler(compiler) {}

    void setProfiler(IGpuProfiler* p) { m_profiler = p; }
    void setShaderCache(IShaderCache* p) { m_shaderCache = p; }
    void setFence(IGpuFence* p) { m_fence = p; }
    void setShaderSourceCompiler(IShaderSourceCompiler* p) { m_compiler = p; }

    [[nodiscard]] IGpuProfiler* getProfiler() const { return m_profiler; }
    [[nodiscard]] IShaderCache* getShaderCache() const { return m_shaderCache; }
    [[nodiscard]] IGpuFence* getFence() const { return m_fence; }
    [[nodiscard]] IShaderSourceCompiler* getShaderSourceCompiler() const { return m_compiler; }

private:
    IGpuProfiler*            m_profiler{nullptr};
    IShaderCache*            m_shaderCache{nullptr};
    IGpuFence*               m_fence{nullptr};
    IShaderSourceCompiler*   m_compiler{nullptr};
};

} // namespace vapi::tools

#endif
