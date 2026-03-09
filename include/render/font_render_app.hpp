/**
 * @file font_render_app.hpp
 * @brief Text-rendering app API: window, Vulkan, font, single loop.
 *        All init and logic are hidden; main() only uses the API.
 *        Lifecycle: destructor performs shutdown (release resources).
 *        Calling run() again after window close is not allowed.
 *
 * Copyright (c) 2025–2026 R3zeProjects. All rights reserved.
 * Non-commercial use is free. Commercial use requires prior written agreement.
 * See licenses/LICENSE_VAPI.txt. Repository: https://github.com/R3zeProjects/vapi_osp
 */

#ifndef VAPI_RENDER_FONT_RENDER_APP_HPP
#define VAPI_RENDER_FONT_RENDER_APP_HPP

#include "core/types.hpp"
#include "font/font_types.hpp"
#include "font/font_draw_list.hpp"
#include "platform/i_platform.hpp"
#include "core/interfaces/i_cpu_bridge.hpp"
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace vapi {

/// run() return constants: 0 = success / window closed normally; other codes = error.
constexpr int kRunSuccess = 0;
constexpr int kRunError = 1;

/// Frame parameters for interactive loop (beginFrame): window size and time.
struct FrameInfo {
    u32 width{0};
    u32 height{0};
    double timeSeconds{0.0};
};

/// App: window + Vulkan + UI text rendering (FontDrawList). Init and loop in one API.
class FontRenderApp {
public:
    FontRenderApp();
    ~FontRenderApp();

    FontRenderApp(const FontRenderApp&) = delete;
    FontRenderApp& operator=(const FontRenderApp&) = delete;

    /// Init: platform, window, GPU, context.
    /// title — window title; shaderDir — directory with font_quad.vert.spv and font_quad.frag.spv (e.g. from CMake SHADER_DIR).
    /// On error returns concrete Error; caller can use errorMessage().
    [[nodiscard]] Result<void> init(u32 width, u32 height, std::string_view title, std::string_view shaderDir);

    /// Set font by name or file path (e.g. "Arial", "Consolas", "/path/to/font.ttf").
    /// If fontNameOrPath is empty, default system font is used.
    /// Must be called after init() and before addText() / run().
    /// On success returns loaded font id.
    /// On failure (font not found) built-in fallback font is used, returns errors::file::NotFound.
    [[nodiscard]] Result<FontId> setFont(std::string_view fontNameOrPath);

    /// Set font size in pixels (default 48).
    /// Must be called after init() and before addText() / run().
    void setFontSize(f32 pixelHeight);

    /// Optional CPU services (thread pool, profiler). When set, may be used for async load/compile. Call after init() if needed.
    void setCpuServices(ICpuServices* cpuServices);

    /// Add a text line (x, y = top-left of first line; color).
    void addText(f32 x, f32 y, std::string_view text, color4 color);

    /// Clear the draw list (for interactive loop: before each new frame).
    void clear();

    /// Line height in pixels (for placing following lines).
    [[nodiscard]] f32 getLineHeight() const;

    /// Platform (for pollEvents, setInputCallbacks, getCursorPosInFramebuffer, getTimeSeconds, etc.).
    [[nodiscard]] IPlatform* getPlatform();
    [[nodiscard]] const IPlatform* getPlatform() const;

    /// Application window id (for setCallbacks, setInputCallbacks).
    [[nodiscard]] WindowId getWindowId() const;

    /// Start of interactive frame: handle resize, return size and time. Does not acquire swapchain.
    [[nodiscard]] Result<FrameInfo> beginFrame();

    /// Draw current draw list and present frame. Call after addText().
    [[nodiscard]] Result<void> endFrame();

    /// Blocking loop: event handling and drawing until window close.
    /// Returns: kRunSuccess (0) — success, window closed normally; kRunError (1) and others — error.
    int run();

    /// Callback for custom frame: called each frame after clear(); fills drawList and sets geometry from (width, height).
    /// If set, run() does not require addText() beforehand; content is drawn by the callback.
    using FrameDrawCallback = std::function<void(FontDrawList&, u32 width, u32 height)>;
    void setFrameDrawCallback(FrameDrawCallback cb);

    /// Callback on mouse button press: (WindowId, button, action, mods). Get position via getPlatform()->getCursorPosInFramebuffer(id).
    using MouseButtonCallback = std::function<void(WindowId, s32 button, s32 action, s32 mods)>;
    void setMouseButtonCallback(MouseButtonCallback cb);

    /// Optional: called at the start of each frame (before beginFrame) with FrameContext. In onBeginFrame, gpuFrameData is nullptr.
    void setOnBeginFrame(std::function<void(const FrameContext&)> cb);
    /// Optional: called at the end of each frame (after endFrame) with FrameContext.
    void setOnEndFrame(std::function<void(const FrameContext&)> cb);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vapi

#endif // VAPI_RENDER_FONT_RENDER_APP_HPP
