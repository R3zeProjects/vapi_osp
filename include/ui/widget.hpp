/** @file widget.hpp
 *  @brief Base widget: geometry, visible, enabled, focus, draw. Original implementation. */

#ifndef VAPI_UI_WIDGET_HPP
#define VAPI_UI_WIDGET_HPP

#include "ui/object.hpp"
#include "ui/events.hpp"
#include "core/types.hpp"
#include "core/interfaces/i_drawable.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vapi::ui {

class Layout;
class Widget;

/// Base for all widgets: rect, visibility, enabled, focus, layout, paint.
class Widget : public Object, public IDrawable {
public:
    explicit Widget(Object* parent = nullptr);
    ~Widget() override;

    void setGeometry(const rect& r);
    void setGeometry(f32 x, f32 y, f32 w, f32 h);
    [[nodiscard]] const rect& geometry() const { return m_rect; }
    [[nodiscard]] f32 x() const { return m_rect.min.x; }
    [[nodiscard]] f32 y() const { return m_rect.min.y; }
    [[nodiscard]] f32 width() const { return m_rect.width(); }
    [[nodiscard]] f32 height() const { return m_rect.height(); }

    void setVisible(bool v) { m_visible = v; }
    [[nodiscard]] bool isVisible() const override { return m_visible; }
    void show() { setVisible(true); }
    void hide() { setVisible(false); }

    void setEnabled(bool e) { m_enabled = e; }
    [[nodiscard]] bool isEnabled() const { return m_enabled; }

    enum class FocusPolicy : u8 { NoFocus, TabFocus, ClickFocus, StrongFocus };
    void setFocusPolicy(FocusPolicy p) { m_focusPolicy = p; }
    [[nodiscard]] FocusPolicy focusPolicy() const { return m_focusPolicy; }
    void setFocus();
    void clearFocus();
    [[nodiscard]] bool hasFocus() const { return m_hasFocus; }

    void setLayout(Layout* layout);
    [[nodiscard]] Layout* layout() const { return m_layout; }
    [[nodiscard]] Layout* layout() { return m_layout; }

    void draw(const DrawContext& ctx) override;

    /// Emitters for input (reactions can be bound by parent or controller).
    Emitter<const PointEvent&>& mousePress() { return m_mousePress; }
    Emitter<const PointEvent&>& mouseRelease() { return m_mouseRelease; }
    Emitter<const PointEvent&>& mouseMove() { return m_mouseMove; }
    Emitter<const PointEvent&>& mouseEnter() { return m_mouseEnter; }
    Emitter<const PointEvent&>& mouseLeave() { return m_mouseLeave; }
    Emitter<const KeyEvent&>& keyPress() { return m_keyPress; }
    Emitter<const KeyEvent&>& keyRelease() { return m_keyRelease; }
    Emitter<const ResizeEvent&>& resized() { return m_resized; }
    Emitter<FocusReason>& focusGained() { return m_focusGained; }
    Emitter<>& focusLost() { return m_focusLost; }

    [[nodiscard]] bool hitTest(f32 x, f32 y) const;
    /// Dispatch point event to this or child; returns true if accepted.
    virtual bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev);
    virtual bool eventKey(const KeyEvent& ev);

protected:
    virtual void paint(const DrawContext& ctx);
    virtual void resizeEvent(const ResizeEvent& ev);
    virtual void focusInEvent(FocusReason reason);
    virtual void focusOutEvent();

private:
    rect m_rect{0.f, 0.f, 100.f, 30.f};
    bool m_visible{true};
    bool m_enabled{true};
    bool m_hasFocus{false};
    FocusPolicy m_focusPolicy{FocusPolicy::ClickFocus};
    Layout* m_layout{nullptr};
    Emitter<const PointEvent&> m_mousePress;
    Emitter<const PointEvent&> m_mouseRelease;
    Emitter<const PointEvent&> m_mouseMove;
    Emitter<const PointEvent&> m_mouseEnter;
    Emitter<const PointEvent&> m_mouseLeave;
    Emitter<const KeyEvent&> m_keyPress;
    Emitter<const KeyEvent&> m_keyRelease;
    Emitter<const ResizeEvent&> m_resized;
    Emitter<FocusReason> m_focusGained;
    Emitter<> m_focusLost;

    /// Per-window focus: map from focus root (topmost Widget in tree) to currently focused widget.
    static std::unordered_map<Widget*, Widget*> s_focusByRoot;
    [[nodiscard]] Widget* getFocusRoot();
};

} // namespace vapi::ui

#endif // VAPI_UI_WIDGET_HPP
