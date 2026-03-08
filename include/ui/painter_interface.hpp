/** @file painter_interface.hpp
 *  @brief Abstract painter for UI: rect, text. App implements for actual rendering. */

#ifndef VAPI_UI_PAINTER_INTERFACE_HPP
#define VAPI_UI_PAINTER_INTERFACE_HPP

#include "core/types.hpp"
#include <string_view>

namespace vapi::ui {

/// Implement this to draw widgets (e.g. with Vulkan + font module). Pass via DrawContext::userData.
class IPainter {
public:
    virtual ~IPainter() = default;
    virtual void drawRect(const rect& r, color4 fill, color4 border) = 0;
    virtual void drawText(f32 x, f32 y, std::string_view text, color4 color, f32 pixelHeight) = 0;
    virtual void drawTextInRect(const rect& r, std::string_view text, color4 color, f32 pixelHeight, u32 align) = 0;
};

} // namespace vapi::ui

#endif // VAPI_UI_PAINTER_INTERFACE_HPP
