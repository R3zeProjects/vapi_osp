/** @file cpu_tools_stub.hpp
 *  @brief Stub implementations of IProfiler, IThreadPool, IHostAllocator for testing and default behavior. */

#ifndef VAPI_CORE_TOOLS_CPU_TOOLS_STUB_HPP
#define VAPI_CORE_TOOLS_CPU_TOOLS_STUB_HPP

#include "core/interfaces/i_cpu_tools.hpp"
#include <chrono>
#include <unordered_map>
#include <cstdlib>

namespace vapi::tools {

/** Simple CPU profiler using std::chrono. */
class CpuProfilerStub : public IProfiler {
public:
    void begin(std::string_view name) override {
        m_start[std::string(name)] = std::chrono::steady_clock::now();
    }
    void end(std::string_view name) override {
        auto it = m_start.find(std::string(name));
        if (it != m_start.end()) {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - it->second).count();
            m_elapsed[std::string(name)] = static_cast<f64>(us) / 1e6;
            m_start.erase(it);
        }
    }
    [[nodiscard]] f64 getElapsedSeconds(std::string_view name) const override {
        auto it = m_elapsed.find(std::string(name));
        return it != m_elapsed.end() ? it->second : 0.0;
    }
    void reset() override { m_elapsed.clear(); m_start.clear(); }
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_start;
    std::unordered_map<std::string, f64> m_elapsed;
};

/** Runs tasks synchronously in submit() (no extra threads). */
class ThreadPoolStub : public IThreadPool {
public:
    void submit(std::function<void()> task) override { if (task) task(); }
    void waitIdle() override {}
    [[nodiscard]] u32 threadCount() const override { return 1; }
};

/** Host allocator using malloc/free (alignment up to alignof(std::max_align_t)). */
class HostAllocatorStub : public IHostAllocator {
public:
    [[nodiscard]] Result<void*> allocate(usize size, usize /*alignment*/) override {
        if (size == 0) return nullptr;
        void* p = std::malloc(size);
        if (!p) return std::unexpected(errors::OutOfMemory);
        return p;
    }
    void deallocate(void* ptr) override { std::free(ptr); }
    [[nodiscard]] bool owns(void* /*ptr*/) const override { return true; }
};

} // namespace vapi::tools

#endif
