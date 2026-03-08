/** @file text_edit.hpp
 *  @brief Multi-line text edit: plain text, textChanged. Original implementation. */

#ifndef VAPI_UI_TEXT_EDIT_HPP
#define VAPI_UI_TEXT_EDIT_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi::ui {

class TextEdit : public Widget {
public:
    explicit TextEdit(Object* parent = nullptr);
    void setPlainText(const std::string& text);
    [[nodiscard]] std::string toPlainText() const;
    void setReadOnly(bool r) { m_readOnly = r; }
    [[nodiscard]] bool isReadOnly() const { return m_readOnly; }
    Emitter<const std::string&>& textChanged() { return m_textChanged; }
protected:
    bool eventKey(const KeyEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::vector<std::string> m_lines;
    u32 m_cursorRow{0}, m_cursorCol{0};
    bool m_readOnly{false};
    Emitter<const std::string&> m_textChanged;
    void insertChar(char c);
    void backspace();
    void newLine();
    std::string joinLines() const;
};

} // namespace vapi::ui

#endif // VAPI_UI_TEXT_EDIT_HPP
