/** @file button.hpp
 *  @brief Push button widget: text, clicked emitter. Original implementation. */

#ifndef VAPI_UI_BUTTON_HPP
#define VAPI_UI_BUTTON_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>

namespace vapi::ui {

class Button : public Widget {
public:
    explicit Button(Object* parent = nullptr);
    void setText(const std::string& text) { m_text = text; }
    [[nodiscard]] const std::string& text() const { return m_text; }
    /// Emitted when button is activated (click or key).
    Emitter<>& clicked() { return m_clicked; }
    [[nodiscard]] const Emitter<>& clicked() const { return m_clicked; }
protected:
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    bool eventKey(const KeyEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    Emitter<> m_clicked;
    bool m_pressed{false};
};

} // namespace vapi::ui

#endif // VAPI_UI_BUTTON_HPP
