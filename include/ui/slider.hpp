/** @file slider.hpp
 *  @brief Horizontal/vertical slider: value range, valueChanged. Original implementation. */

#ifndef VAPI_UI_SLIDER_HPP
#define VAPI_UI_SLIDER_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"

namespace vapi::ui {

class Slider : public Widget {
public:
    enum class Orientation { Horizontal, Vertical };
    explicit Slider(Object* parent = nullptr, Orientation o = Orientation::Horizontal);
    void setRange(f32 minVal, f32 maxVal);
    void setValue(f32 v);
    [[nodiscard]] f32 value() const { return m_value; }
    [[nodiscard]] f32 minimum() const { return m_min; }
    [[nodiscard]] f32 maximum() const { return m_max; }
    void setSingleStep(f32 step) { m_singleStep = step; }
    Emitter<f32>& valueChanged() { return m_valueChanged; }
protected:
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    Orientation m_orientation{Orientation::Horizontal};
    f32 m_min{0.f}, m_max{100.f}, m_value{0.f}, m_singleStep{1.f};
    Emitter<f32> m_valueChanged;
    f32 valueFromPosition(f32 x, f32 y) const;
};

} // namespace vapi::ui

#endif // VAPI_UI_SLIDER_HPP
