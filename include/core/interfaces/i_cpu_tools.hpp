/** @file i_cpu_tools.hpp
 *  @brief CPU-side tool interfaces: profiling, thread pool, host allocator. */

#ifndef VAPI_CORE_I_CPU_TOOLS_HPP
#define VAPI_CORE_I_CPU_TOOLS_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <functional>
#include <string_view>

namespace vapi {

/** CPU timing / profiling. */
class IProfiler {
public:
    virtual ~IProfiler() = default;
    virtual void begin(std::string_view name) = 0;
    virtual void end(std::string_view name) = 0;
    /** Elapsed time in seconds for the last completed scope with this name. */
    [[nodiscard]] virtual f64 getElapsedSeconds(std::string_view name) const = 0;
    virtual void reset() = 0;
};

/** Task submission and execution (CPU). */
class IThreadPool {
public:
    virtual ~IThreadPool() = default;
    /** Submit a void task. */
    virtual void submit(std::function<void()> task) = 0;
    virtual void waitIdle() = 0;
    [[nodiscard]] virtual u32 threadCount() const = 0;
};

/** Host (CPU) memory allocator. */
class IHostAllocator {
public:
    virtual ~IHostAllocator() = default;
    [[nodiscard]] virtual Result<void*> allocate(usize size, usize alignment = 1) = 0;
    virtual void deallocate(void* ptr) = 0;
    [[nodiscard]] virtual bool owns(void* ptr) const = 0;
};

} // namespace vapi

#endif
