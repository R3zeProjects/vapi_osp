/** @file cpu_profiler.hpp
 *  @brief Реализация IProfiler: замер времени по scope (begin/end) через std::chrono. */

#ifndef VAPI_CORE_TOOLS_CPU_PROFILER_HPP
#define VAPI_CORE_TOOLS_CPU_PROFILER_HPP

#include "core/interfaces/i_cpu_tools.hpp"
#include <chrono>
#include <unordered_map>
#include <string>

namespace vapi::tools {

/** CPU-профайлер: замер времени между begin(name) и end(name), getElapsedSeconds(name). */
class CpuProfiler : public IProfiler {
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

} // namespace vapi::tools

#endif
