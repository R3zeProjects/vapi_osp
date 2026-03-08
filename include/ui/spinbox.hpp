/** @file spinbox.hpp
 *  @brief Numeric spin box: value, range, step, valueChanged. Original implementation. */

#ifndef VAPI_UI_SPINBOX_HPP
#define VAPI_UI_SPINBOX_HPP

#include "ui/widget.hpp"
#include "ui/line_edit.hpp"
#include "core/types.hpp"

namespace vapi::ui {

class SpinBox : public Widget {
public:
    explicit SpinBox(Object* parent = nullptr);
    void setRange(f32 minVal, f32 maxVal);
    void setValue(f32 v);
    [[nodiscard]] f32 value() const { return m_value; }
    void setSingleStep(f32 step) { m_singleStep = step; }
    void setDecimals(u32 d) { m_decimals = d; }
    Emitter<f32>& valueChanged() { return m_valueChanged; }
protected:
    void paint(const DrawContext& ctx) override;
private:
    f32 m_min{0.f}, m_max{100.f}, m_value{0.f}, m_singleStep{1.f};
    u32 m_decimals{0};
    Emitter<f32> m_valueChanged;
    void syncText();
    void stepBy(s32 delta);
};

} // namespace vapi::ui

#endif // VAPI_UI_SPINBOX_HPP
