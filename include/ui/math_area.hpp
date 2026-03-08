/** @file math_area.hpp
 *  @brief Math work area: formula input, structured segments (numbers, identifiers, operators).
 *  Original implementation — MathCad-style work area. */

#ifndef VAPI_UI_MATH_AREA_HPP
#define VAPI_UI_MATH_AREA_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>
#include <cstddef>

namespace vapi::ui {

/// Segment kind for formula rendering/layout.
enum class MathSegmentKind : u8 { Number, Identifier, Operator, Paren, Space, Unknown };

struct MathSegment {
    std::string text;
    MathSegmentKind kind{MathSegmentKind::Unknown};
    u32 start{0};
    u32 end{0};
};

/** Math area: editable formula line(s), parsed into segments for display.
 *  Supports numbers, identifiers (a-z, Greek), operators (+ - * / ^ =), parentheses. */
class MathArea : public Widget {
public:
    explicit MathArea(Object* parent = nullptr);
    void setFormula(const std::string& formula);
    [[nodiscard]] std::string formula() const { return m_text; }
    void setCursorPosition(u32 pos);
    [[nodiscard]] u32 cursorPosition() const { return m_cursorPos; }
    /// Emitted when formula text changes.
    Emitter<const std::string&>& formulaChanged() { return m_formulaChanged; }
protected:
    bool eventKey(const KeyEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    u32 m_cursorPos{0};
    Emitter<const std::string&> m_formulaChanged;
    void parseSegments(std::vector<MathSegment>& out) const;
    void insertChar(char c);
    void backspace();
    void moveCursor(s32 delta);
};

} // namespace vapi::ui

#endif // VAPI_UI_MATH_AREA_HPP
