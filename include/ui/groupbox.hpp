/** @file groupbox.hpp
 *  @brief Group box: titled frame around content. Original implementation. */

#ifndef VAPI_UI_GROUPBOX_HPP
#define VAPI_UI_GROUPBOX_HPP

#include "ui/widget.hpp"
#include "core/types.hpp"
#include <string>

namespace vapi::ui {

class GroupBox : public Widget {
public:
    explicit GroupBox(Object* parent = nullptr);
    void setTitle(const std::string& t) { m_title = t; }
    [[nodiscard]] const std::string& title() const { return m_title; }
protected:
    void paint(const DrawContext& ctx) override;
private:
    std::string m_title;
};

} // namespace vapi::ui

#endif // VAPI_UI_GROUPBOX_HPP
