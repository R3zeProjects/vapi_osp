/** @file progressbar.hpp
 *  @brief Progress bar: value range, value. Original implementation. */

#ifndef VAPI_UI_PROGRESSBAR_HPP
#define VAPI_UI_PROGRESSBAR_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <algorithm>

namespace vapi::ui {

class ProgressBar : public Widget {
public:
    explicit ProgressBar(Object* parent = nullptr);
    void setRange(f32 minVal, f32 maxVal);
    void setValue(f32 v) { m_value = std::clamp(v, m_min, m_max); }
    [[nodiscard]] f32 value() const { return m_value; }
    [[nodiscard]] f32 minimum() const { return m_min; }
    [[nodiscard]] f32 maximum() const { return m_max; }
protected:
    void paint(const DrawContext& ctx) override;
private:
    f32 m_min{0.f}, m_max{100.f}, m_value{0.f};
};

} // namespace vapi::ui

#endif // VAPI_UI_PROGRESSBAR_HPP
