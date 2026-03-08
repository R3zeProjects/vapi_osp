/** @file cpu_bridge_impl.hpp
 *  @brief Concrete implementations for CPU bridge: CpuServicesHolder, SimpleCpuFrameDriver. */

#ifndef VAPI_CORE_TOOLS_CPU_BRIDGE_IMPL_HPP
#define VAPI_CORE_TOOLS_CPU_BRIDGE_IMPL_HPP

#include "core/interfaces/i_cpu_bridge.hpp"
#include <chrono>

namespace vapi::tools {

/**
 * Non-owning holder for CPU services. Implements ICpuServices; any pointer may be nullptr.
 */
class CpuServicesHolder : public ICpuServices {
public:
    CpuServicesHolder(IThreadPool* pool = nullptr, IHostAllocator* alloc = nullptr, IProfiler* profiler = nullptr)
        : m_pool(pool), m_alloc(alloc), m_profiler(profiler) {}

    void setThreadPool(IThreadPool* p) { m_pool = p; }
    void setHostAllocator(IHostAllocator* p) { m_alloc = p; }
    void setProfiler(IProfiler* p) { m_profiler = p; }

    [[nodiscard]] IThreadPool* getThreadPool() override { return m_pool; }
    [[nodiscard]] const IThreadPool* getThreadPool() const override { return m_pool; }
    [[nodiscard]] IHostAllocator* getHostAllocator() override { return m_alloc; }
    [[nodiscard]] const IHostAllocator* getHostAllocator() const override { return m_alloc; }
    [[nodiscard]] IProfiler* getProfiler() override { return m_profiler; }
    [[nodiscard]] const IProfiler* getProfiler() const override { return m_profiler; }

private:
    IThreadPool*    m_pool{nullptr};
    IHostAllocator* m_alloc{nullptr};
    IProfiler*      m_profiler{nullptr};
};

/**
 * Simple frame driver using std::chrono. First beginFrame() returns dt = 0.
 */
class SimpleCpuFrameDriver : public ICpuFrameDriver {
public:
    [[nodiscard]] f64 beginFrame() override {
        auto now = std::chrono::steady_clock::now();
        f64 dt = 0.0;
        if (m_first) {
            m_first = false;
        } else {
            dt = std::chrono::duration<f64>(now - m_last).count();
        }
        m_last = now;
        return dt;
    }

    void endFrame() override {
        ++m_frameIndex;
    }

    [[nodiscard]] u64 frameIndex() const override { return m_frameIndex; }

    void reset() {
        m_first = true;
        m_frameIndex = 0;
    }

private:
    std::chrono::steady_clock::time_point m_last{};
    bool m_first{true};
    u64 m_frameIndex{0};
};

} // namespace vapi::tools

#endif // VAPI_CORE_TOOLS_CPU_BRIDGE_IMPL_HPP
