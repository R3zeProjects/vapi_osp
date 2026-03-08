/** @file tabwidget.hpp
 *  @brief Tab widget: stacked pages with tab bar. Original implementation. */

#ifndef VAPI_UI_TABWIDGET_HPP
#define VAPI_UI_TABWIDGET_HPP

#include "ui/widget.hpp"
#include "ui/layout.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi::ui {

class StackLayout;
class Widget;

class TabWidget : public Widget {
public:
    explicit TabWidget(Object* parent = nullptr);
    u32 addTab(Widget* page, const std::string& label);
    void setCurrentIndex(u32 index);
    [[nodiscard]] u32 currentIndex() const;
    [[nodiscard]] u32 count() const { return static_cast<u32>(m_labels.size()); }
    Emitter<u32>& currentChanged() { return m_currentChanged; }
protected:
    void resizeEvent(const ResizeEvent& ev) override;
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    Widget* m_content{nullptr};
    StackLayout* m_stack{nullptr};
    std::vector<std::string> m_labels;
    f32 m_tabHeight{24.f};
    Emitter<u32> m_currentChanged;
    u32 hitTestTab(f32 x, f32 y) const;
};

} // namespace vapi::ui

#endif // VAPI_UI_TABWIDGET_HPP
