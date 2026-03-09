/**
 * @file run_ui_loop.hpp
 * @brief High-level UI run loop: platform + window + Vulkan + widget tree until window close.
 *        Creates GPU backend, RenderContext, VulkanUiPainter and runs the event/render loop.
 */

#ifndef VAPI_UI_RUN_UI_LOOP_HPP
#define VAPI_UI_RUN_UI_LOOP_HPP

#include "core/types.hpp"
#include "core/interfaces/i_cpu_bridge.hpp"
#include <functional>
#include <string_view>

namespace vapi {
class IPlatform;
namespace ui {
class Widget;
class BoxLayout;
}
}

namespace vapi::ui {

/** Options for runUILoop. All optional; defaults are suitable for most apps. */
struct RunUILoopOptions {
    std::string_view shaderDir{"."};
    std::string_view appName{"VAPI App"};
    std::string_view fontName{"Arial"};
    f32 fontSize{18.f};
    color4 clearColor{0.92f, 0.92f, 0.94f, 1.f};

    /** Called each frame after root geometry and layout are updated (framebuffer width, height). Use for custom layout or state. */
    std::function<void(vapi::WindowId, u32 width, u32 height)> onResize;

    /** Called once when the window is about to close (before exiting the loop). Use e.g. to save state. */
    std::function<void(vapi::WindowId)> onClose;

    /** Called at the start of each frame (before beginFrame), with frame context (cpu frame index, dt, gpu frame index).
     *  In this callback gpuFrameData is always nullptr (GPU frame not yet acquired). */
    std::function<void(const vapi::FrameContext&)> onBeginFrame;

    /** Called after beginFrame() with full FrameContext (including gpuFrameData). Use when you need the current command buffer / FrameData at frame start. */
    std::function<void(const vapi::FrameContext&)> onFrameAcquired;

    /** Called at the end of each frame (after endFrame), with frame context. */
    std::function<void(const vapi::FrameContext&)> onEndFrame;

    /** If non-null, onBeginFrame/update(dt)/onEndFrame are called each frame in addition to the callbacks above. */
    vapi::IAppLogic* appLogic{nullptr};

    /** Optional CPU services (thread pool, profiler). When set, may be used for async load/compile. */
    vapi::ICpuServices* cpuServices{nullptr};

    /** Optional frame driver for dt and frame index. When set, runUILoop uses it instead of internal timing. Set time source on the driver (e.g. platform->getTimeSeconds) before calling runUILoop. */
    vapi::ICpuFrameDriver* frameDriver{nullptr};

    /** Optional: prepare data for next frame (double-buffer model). Called at start of iteration; use for overlap CPU/GPU. */
    std::function<void(const vapi::FrameContext&)> onPrepareNextFrame;

    /** Optional path to pipeline cache file. When non-empty, pipeline cache is loaded and used for pipeline creation. */
    std::string_view pipelineCachePath;
};

/**
 * Run the full UI loop: poll events, resize handling, root.draw with VulkanUiPainter, present.
 * Creates Vulkan backend, RenderContext and VulkanUiPainter internally; runs until the window is closed.
 * Does not create or destroy the window — caller owns platform and window.
 * Window callbacks (setCallbacks) are set internally for resize; use opts.onResize / opts.onClose for app logic.
 *
 * @param platform  Initialized platform (e.g. from createPlatform()).
 * @param winId     Window id from platform->createWindow().
 * @param root      Root widget; geometry is updated each frame to framebuffer size.
 * @param mainLayout Optional; if non-null, activate() is called each frame after root resize.
 * @param opts      Shader dir, font, clear color, optional onResize/onClose; use {} for defaults.
 */
void runUILoop(vapi::IPlatform* platform, vapi::WindowId winId,
               Widget& root, BoxLayout* mainLayout,
               const RunUILoopOptions& opts = {});

} // namespace vapi::ui

#endif // VAPI_UI_RUN_UI_LOOP_HPP
