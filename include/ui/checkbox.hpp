/** @file checkbox.hpp
 *  @brief Checkbox: checked state, toggled emitter. Original implementation. */

#ifndef VAPI_UI_CHECKBOX_HPP
#define VAPI_UI_CHECKBOX_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>

namespace vapi::ui {

class CheckBox : public Widget {
public:
    explicit CheckBox(Object* parent = nullptr);
    void setText(const std::string& text) { m_text = text; }
    [[nodiscard]] const std::string& text() const { return m_text; }
    void setChecked(bool c) { if (m_checked != c) { m_checked = c; m_toggled.emit(m_checked); } }
    [[nodiscard]] bool isChecked() const { return m_checked; }
    Emitter<bool>& toggled() { return m_toggled; }
protected:
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    bool m_checked{false};
    Emitter<bool> m_toggled;
};

} // namespace vapi::ui

#endif // VAPI_UI_CHECKBOX_HPP
