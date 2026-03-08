/** @file radio_button.hpp
 *  @brief Radio button: one of a group. Original implementation. */

#ifndef VAPI_UI_RADIO_BUTTON_HPP
#define VAPI_UI_RADIO_BUTTON_HPP

#include "ui/widget.hpp"
#include "ui/emitter.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi::ui {

class RadioGroup;

class RadioButton : public Widget {
public:
    explicit RadioButton(Object* parent = nullptr);
    void setText(const std::string& text) { m_text = text; }
    [[nodiscard]] const std::string& text() const { return m_text; }
    void setChecked(bool c);
    [[nodiscard]] bool isChecked() const { return m_checked; }
    void setGroup(RadioGroup* g);
    Emitter<bool>& toggled() { return m_toggled; }
protected:
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    bool m_checked{false};
    RadioGroup* m_group{nullptr};
    Emitter<bool> m_toggled;
};

class RadioGroup : public Object {
public:
    explicit RadioGroup(Object* parent = nullptr);
    void addButton(RadioButton* b);
    [[nodiscard]] RadioButton* checkedButton() const { return m_checked; }
    void setChecked(RadioButton* b);
    Emitter<RadioButton*>& buttonToggled() { return m_buttonToggled; }
private:
    std::vector<RadioButton*> m_buttons;
    RadioButton* m_checked{nullptr};
    Emitter<RadioButton*> m_buttonToggled;
    friend class RadioButton;
};

} // namespace vapi::ui

#endif // VAPI_UI_RADIO_BUTTON_HPP
