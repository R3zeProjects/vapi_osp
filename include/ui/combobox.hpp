/** @file combobox.hpp
 *  @brief Combo box: list of items, currentIndex/currentText, activated. Original implementation. */

#ifndef VAPI_UI_COMBOBOX_HPP
#define VAPI_UI_COMBOBOX_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi::ui {

class ComboBox : public Widget {
public:
    explicit ComboBox(Object* parent = nullptr);
    void addItem(const std::string& text);
    void clear();
    void setCurrentIndex(s32 index);
    [[nodiscard]] s32 currentIndex() const { return m_currentIndex; }
    [[nodiscard]] std::string currentText() const;
    [[nodiscard]] u32 count() const { return static_cast<u32>(m_items.size()); }
    Emitter<s32>& activated() { return m_activated; }
protected:
    bool eventPoint(f32 x, f32 y, u32 kind, const PointEvent& ev) override;
    void paint(const DrawContext& ctx) override;
private:
    std::vector<std::string> m_items;
    s32 m_currentIndex{-1};
    bool m_dropDownOpen{false};
    Emitter<s32> m_activated;
};

} // namespace vapi::ui

#endif // VAPI_UI_COMBOBOX_HPP
