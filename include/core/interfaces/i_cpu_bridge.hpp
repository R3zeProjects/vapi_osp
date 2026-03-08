/** @file i_cpu_bridge.hpp
 *  @brief Interfaces for linking the application (CPU side) with VAPI: CPU services facade and frame/app callbacks. */

#ifndef VAPI_CORE_I_CPU_BRIDGE_HPP
#define VAPI_CORE_I_CPU_BRIDGE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_cpu_tools.hpp"

namespace vapi {

/**
 * Facade for CPU-side services (thread pool, allocator, profiler).
 * Pass ICpuServices* or ICpuServices& to modules that need CPU tools without depending on concrete implementations.
 * Any getter may return nullptr if the service is not available.
 */
class ICpuServices {
public:
    virtual ~ICpuServices() = default;

    [[nodiscard]] virtual IThreadPool*      getThreadPool() = 0;
    [[nodiscard]] virtual const IThreadPool* getThreadPool() const = 0;

    [[nodiscard]] virtual IHostAllocator*      getHostAllocator() = 0;
    [[nodiscard]] virtual const IHostAllocator* getHostAllocator() const = 0;

    [[nodiscard]] virtual IProfiler*      getProfiler() = 0;
    [[nodiscard]] virtual const IProfiler* getProfiler() const = 0;
};

/**
 * Application logic callbacks driven by the main loop (CPU side).
 * Implement this in your application; the frame driver or main loop calls these each frame.
 */
class IAppLogic {
public:
    virtual ~IAppLogic() = default;

    /** Called once per frame before input/poll. dt = time since last frame in seconds. */
    virtual void update(f64 dt) = 0;

    /** Called at the start of the frame (e.g. before render begin). */
    virtual void onBeginFrame() = 0;

    /** Called at the end of the frame (e.g. after render end / present). */
    virtual void onEndFrame() = 0;
};

/**
 * Optional extension of IAppLogic: fixed timestep for physics/simulation (CPU).
 * The main loop may call fixedUpdate(fixedDt) with constant dt (e.g. 1/60) in addition to update(dt).
 */
class IAppFixedTick : public IAppLogic {
public:
    ~IAppFixedTick() override = default;

    /** Called at fixed intervals (e.g. 60 Hz). Default: no-op. */
    virtual void fixedUpdate(f64 fixedDt) { (void)fixedDt; }
};

/**
 * Frame driver: owns the notion of frame boundaries and delta time on the CPU side.
 * The main loop can use this to get dt and signal begin/end frame to optional IAppLogic.
 */
class ICpuFrameDriver {
public:
    virtual ~ICpuFrameDriver() = default;

    /** Start a new frame; returns delta time since previous frame in seconds. */
    [[nodiscard]] virtual f64 beginFrame() = 0;

    /** End the current frame (e.g. for FPS / stats). */
    virtual void endFrame() = 0;

    /** Current frame index (0-based, increments each beginFrame). */
    [[nodiscard]] virtual u64 frameIndex() const = 0;
};

} // namespace vapi

#endif // VAPI_CORE_I_CPU_BRIDGE_HPP
