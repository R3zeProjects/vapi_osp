/** @file line_edit.hpp
 *  @brief Single-line text edit: text, cursor, textChanged/editingFinished. Original implementation. */

#ifndef VAPI_UI_LINE_EDIT_HPP
#define VAPI_UI_LINE_EDIT_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>

namespace vapi::ui {

class LineEdit : public Widget {
public:
    explicit LineEdit(Object* parent = nullptr);
    void setText(const std::string& text);
    [[nodiscard]] const std::string& text() const { return m_text; }
    void setPlaceholder(const std::string& p) { m_placeholder = p; }
    [[nodiscard]] const std::string& placeholder() const { return m_placeholder; }
    void setMaxLength(u32 max) { m_maxLength = max; }
    [[nodiscard]] u32 maxLength() const { return m_maxLength; }
    /// Emitted when text changes (every key).
    Emitter<const std::string&>& textChanged() { return m_textChanged; }
    /// Emitted when focus is lost or Enter.
    Emitter<>& editingFinished() { return m_editingFinished; }
protected:
    bool eventKey(const KeyEvent& ev) override;
    void focusOutEvent() override;
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    std::string m_placeholder;
    u32 m_cursorPos{0};
    u32 m_maxLength{0};
    Emitter<const std::string&> m_textChanged;
    Emitter<> m_editingFinished;
    void insertChar(char c);
    void backspace();
    void moveCursor(s32 delta);
};

} // namespace vapi::ui

#endif // VAPI_UI_LINE_EDIT_HPP
