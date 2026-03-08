/** @file label.hpp
 *  @brief Text label widget: read-only text, alignment. Original implementation. */

#ifndef VAPI_UI_LABEL_HPP
#define VAPI_UI_LABEL_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>

namespace vapi::ui {

class Label : public Widget {
public:
    explicit Label(Object* parent = nullptr);
    void setText(const std::string& text) { m_text = text; }
    [[nodiscard]] const std::string& text() const { return m_text; }
    enum class Align { Left, Center, Right };
    void setAlignment(Align a) { m_align = a; }
    [[nodiscard]] Align alignment() const { return m_align; }
    void setWordWrap(bool w) { m_wordWrap = w; }
    [[nodiscard]] bool wordWrap() const { return m_wordWrap; }
protected:
    void paint(const DrawContext& ctx) override;
private:
    std::string m_text;
    Align m_align{Align::Left};
    bool m_wordWrap{false};
};

} // namespace vapi::ui

#endif // VAPI_UI_LABEL_HPP
