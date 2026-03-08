/** @file scroll_area.hpp
 *  @brief Scroll area: widget inside scrollable viewport. Original implementation. */

#ifndef VAPI_UI_SCROLL_AREA_HPP
#define VAPI_UI_SCROLL_AREA_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"

namespace vapi::ui {

class ScrollArea : public Widget {
public:
    explicit ScrollArea(Object* parent = nullptr);
    void setWidget(Widget* w);
    [[nodiscard]] Widget* widget() const { return m_widget; }
    void setScrollX(f32 x) { m_scrollX = x; }
    void setScrollY(f32 y) { m_scrollY = y; }
    [[nodiscard]] f32 scrollX() const { return m_scrollX; }
    [[nodiscard]] f32 scrollY() const { return m_scrollY; }
protected:
    void resizeEvent(const ResizeEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    Widget* m_widget{nullptr};
    f32 m_scrollX{0.f}, m_scrollY{0.f};
};

} // namespace vapi::ui

#endif // VAPI_UI_SCROLL_AREA_HPP
