/**
 * @file vulkan_ui_painter.hpp
 * @brief Concrete IPainter backed by vapi_font + Vulkan dynamic rendering.
 *
 * Copyright (c) 2025–2026 R3zeProjects. All rights reserved.
 * Non-commercial use is free. Commercial use requires prior written agreement.
 * See licenses/LICENSE_VAPI.txt. Repository: https://github.com/R3zeProjects/vapi_osp
 *
 *  Usage:
 *  @code
 *  // In your frame setup:
 *  VulkanUiPainter painter;
 *  painter.init(&device, &renderCtx, fontSource, 16.f, shaderDir);
 *
 *  // Per frame:
 *  auto frame = renderCtx.beginFrame().value();
 *  renderCtx.beginRendering(frame, clearColor);
 *
 *  painter.beginFrame(frame);
 *  DrawContext ctx{ &painter };
 *  rootWidget.draw(ctx);
 *  painter.endFrame(frame);     // uploads atlas + vertices, issues draw call
 *
 *  renderCtx.endRendering(frame);
 *  renderCtx.endFrame(frame);
 *  @endcode
 *
 *  @note VulkanUiPainter owns its GPU resources and must be shutdown() before device destruction. */

#ifndef VAPI_UI_VULKAN_UI_PAINTER_HPP
#define VAPI_UI_VULKAN_UI_PAINTER_HPP

#include "ui/painter_interface.hpp"
#include "core/types.hpp"
#include "core/error.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <string_view>
#include <vector>

namespace vapi {

class VkDeviceWrapper;
class RenderContext;
class IFontSource;
struct FrameData;
class ICpuServices;

} // namespace vapi

namespace vapi::ui {

/** Vulkan-backed implementation of IPainter.
 *
 *  Internally uses FontDrawList to accumulate draw commands (rects + text) and
 *  flushes them to a single Vulkan draw call at endFrame().
 *
 *  Thread safety: not thread-safe; call from a single render thread only. */
class VulkanUiPainter final : public IPainter {
public:
    VulkanUiPainter();
    ~VulkanUiPainter() override;

    VulkanUiPainter(const VulkanUiPainter&) = delete;
    VulkanUiPainter& operator=(const VulkanUiPainter&) = delete;
    VulkanUiPainter(VulkanUiPainter&&) noexcept;
    VulkanUiPainter& operator=(VulkanUiPainter&&) noexcept;

    /** Initialize GPU resources.
     *  @param device      Valid VkDeviceWrapper; must outlive this painter.
     *  @param renderCtx   Active RenderContext; must outlive this painter.
     *  @param font        Font source for text rendering; may be nullptr (text draws will be no-ops).
     *  @param fontSize    Default font size in pixels.
     *  @param shaderDir   Directory containing font_quad.vert.spv and font_quad.frag.spv.
     *  @return Ok on success. */
    [[nodiscard]] Result<void> init(VkDeviceWrapper* device,
                                   RenderContext*   renderCtx,
                                   IFontSource*     font,
                                   f32              fontSize,
                                   std::string_view shaderDir);

    /** Release all Vulkan resources. Safe to call when not initialized. */
    void shutdown();

    /** Optional CPU services (thread pool, etc.). When set, shader load may use getOrCompileOnPool. Call after init() if needed. */
    void setCpuServices(ICpuServices* cpuServices);

    /** Optional pipeline cache for pipeline creation. Call after init() if needed. */
    void setPipelineCache(VkPipelineCache cache);

    /** Set font source and size (can be changed between frames, not mid-frame).
     *  @param font        New font source; nullptr disables text rendering.
     *  @param fontSize    Font size in pixels (>0). */
    void setFont(IFontSource* font, f32 fontSize);

    /** Call before drawing any widgets for this frame.
     *  @param frame Active frame data from RenderContext::beginFrame(). */
    void beginFrame(const FrameData& frame);

    /** Flush accumulated draw commands to GPU and issue a single draw call.
     *  Rendering must have begun (RenderContext::beginRendering) before this call.
     *  @param frame Same FrameData passed to beginFrame(). */
    void endFrame(const FrameData& frame);

    // ── IPainter ──────────────────────────────────────────────

    /** Draw a filled rectangle with an optional border.
     *  @param r      Rectangle in screen coordinates (pixels).
     *  @param fill   Fill color (RGBA).
     *  @param border Border color (RGBA); set alpha=0 to skip border. */
    void drawRect(const rect& r, color4 fill, color4 border) override;

    /** Draw text at (x, y) in screen pixels.
     *  @param x,y        Top-left baseline position.
     *  @param text       UTF-8 string.
     *  @param color      Text color.
     *  @param pixelHeight Font size override in pixels; 0 = use painter default. */
    void drawText(f32 x, f32 y, std::string_view text, color4 color, f32 pixelHeight) override;

    /** Draw text clipped to a rectangle with horizontal/vertical alignment.
     *  @param r           Clipping and layout rectangle.
     *  @param text        UTF-8 string.
     *  @param color       Text color.
     *  @param pixelHeight Font size override; 0 = use painter default.
     *  @param align       Bitfield: bit 0 = right-align, bit 1 = center-align horizontally,
     *                     bit 2 = center-align vertically. Default (0) = top-left. */
    void drawTextInRect(const rect& r, std::string_view text, color4 color,
                        f32 pixelHeight, u32 align) override;

    [[nodiscard]] bool isInitialized() const { return m_impl != nullptr; }

private:
    struct Impl;
    Impl* m_impl{nullptr};
};

} // namespace vapi::ui

#endif // VAPI_UI_VULKAN_UI_PAINTER_HPP
