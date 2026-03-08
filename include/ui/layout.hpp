/** @file layout.hpp
 *  @brief Layout base and concrete layouts: box, grid. Original implementation. */

#ifndef VAPI_UI_LAYOUT_HPP
#define VAPI_UI_LAYOUT_HPP

#include "ui/object.hpp"
#include "ui/widget.hpp"
#include "core/types.hpp"
#include <vector>
#include <memory>

namespace vapi::ui {

class Widget;

class Layout : public Object {
public:
    /// Creates layout as child of parent. Call widget->setLayout(this) after construction.
    explicit Layout(Widget* parent = nullptr);
    ~Layout() override;

    void setOwner(Widget* owner);
    [[nodiscard]] Widget* owner() const { return m_owner; }
    virtual void activate() = 0;  // apply geometry to children

protected:
    Widget* m_owner{nullptr};
};

/// Item in a layout: widget + stretch factor.
struct LayoutItem {
    Widget* widget{nullptr};
    u32 stretch{0};
    u32 row{0}, col{0};
    u32 rowSpan{1}, colSpan{1};
};

/// Horizontal or vertical box.
class BoxLayout : public Layout {
public:
    enum class Direction { Horizontal, Vertical };
    explicit BoxLayout(Widget* parent = nullptr, Direction dir = Direction::Horizontal);
    void addWidget(Widget* w, u32 stretch = 0);
    void addLayout(Layout* sub, u32 stretch = 0);
    /// Изменить stretch виджета (чтобы раскрытые секции получали больше места и не наезжали на другие).
    void setWidgetStretch(Widget* w, u32 stretch);
    void setSpacing(u32 px) { m_spacing = px; }
    void setContentsMargins(f32 left, f32 top, f32 right, f32 bottom);
    void activate() override;

private:
    Direction m_direction{Direction::Horizontal};
    u32 m_spacing{4};
    f32 m_margins[4]{4, 4, 4, 4};
    std::vector<LayoutItem> m_items;
    std::vector<std::pair<Layout*, u32>> m_sublayouts;
};

/// Grid of widgets.
class GridLayout : public Layout {
public:
    explicit GridLayout(Widget* parent = nullptr);
    void addWidget(Widget* w, u32 row, u32 col, u32 rowSpan = 1, u32 colSpan = 1, u32 stretch = 0);
    void setSpacing(u32 h, u32 v);
    void setContentsMargins(f32 left, f32 top, f32 right, f32 bottom);
    void activate() override;

private:
    u32 m_hSpacing{4};
    u32 m_vSpacing{4};
    f32 m_margins[4]{4, 4, 4, 4};
    std::vector<LayoutItem> m_items;
};

/// Stack: one visible child at a time.
class StackLayout : public Layout {
public:
    explicit StackLayout(Widget* parent = nullptr);
    void addWidget(Widget* w);
    void setCurrentIndex(u32 index);
    [[nodiscard]] u32 currentIndex() const { return m_current; }
    void activate() override;

private:
    std::vector<Widget*> m_widgets;
    u32 m_current{0};
};

} // namespace vapi::ui

#endif // VAPI_UI_LAYOUT_HPP
