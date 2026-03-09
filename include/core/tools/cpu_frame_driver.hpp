/** @file cpu_frame_driver.hpp
 *  @brief Implementation of ICpuFrameDriver: frame boundaries and delta time from a time source. */

#ifndef VAPI_CORE_TOOLS_CPU_FRAME_DRIVER_HPP
#define VAPI_CORE_TOOLS_CPU_FRAME_DRIVER_HPP

#include "core/interfaces/i_cpu_bridge.hpp"
#include "core/types.hpp"
#include <functional>

namespace vapi::tools {

/** ICpuFrameDriver implementation: uses a time source (e.g. platform->getTimeSeconds()) for dt. */
class CpuFrameDriver : public ICpuFrameDriver {
public:
    /** Set time source (e.g. [&platform]{ return platform->getTimeSeconds(); }). Called each beginFrame() if set. */
    void setTimeSource(std::function<double()> fn) { m_timeSource = std::move(fn); }

    /** Start a new frame; returns delta time. Uses time source if set; otherwise pass now to beginFrame(now). */
    [[nodiscard]] f64 beginFrame() override;
    /** Use explicit current time (e.g. platform->getTimeSeconds()). Use when time source is not set. */
    [[nodiscard]] f64 beginFrame(double nowSeconds);

    void endFrame() override;
    [[nodiscard]] u64 frameIndex() const override { return m_frameIndex; }

private:
    std::function<double()> m_timeSource;
    double m_lastTime{0.0};
    u64 m_frameIndex{0};
};

} // namespace vapi::tools

#endif
