/** @file events.hpp
 *  @brief UI event types: input, focus, resize. Original implementation. */

#ifndef VAPI_UI_EVENTS_HPP
#define VAPI_UI_EVENTS_HPP

#include "core/types.hpp"
#include "core/interfaces/i_drawable.hpp"

namespace vapi::ui {

struct PointEvent {
    f32 x{0.f};
    f32 y{0.f};
    u32 buttonMask{0};  // 1=left, 2=right, 4=middle
    u32 modifiers{0};   // shift, ctrl, alt bits
};

struct KeyEvent {
    u32 keyCode{0};
    u32 scanCode{0};
    bool repeat{false};
    u32 modifiers{0};
};

struct ResizeEvent {
    f32 oldWidth{0.f};
    f32 oldHeight{0.f};
    f32 newWidth{0.f};
    f32 newHeight{0.f};
};

enum class FocusReason : u8 { Mouse, Tab, Backtab, Other };

} // namespace vapi::ui

#endif // VAPI_UI_EVENTS_HPP
