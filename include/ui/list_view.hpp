/** @file list_view.hpp
 *  @brief List view widget: list of strings, single selection, scroll. Original implementation. */

#ifndef VAPI_UI_LIST_VIEW_HPP
#define VAPI_UI_LIST_VIEW_HPP

#include "ui/widget.hpp"
#include "ui/emitter.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi::ui {

class ScrollArea;

/** Widget that displays a list of strings. One item can be selected; click emits selected(index). */
class ListView : public Widget {
public:
    explicit ListView(Object* parent = nullptr);
    ~ListView() override;

    void setItems(const std::vector<std::string>& items);
    void addItem(const std::string& item);
    void clear();
    [[nodiscard]] const std::vector<std::string>& items() const { return m_items; }
    [[nodiscard]] size_t count() const { return m_items.size(); }

    [[nodiscard]] size_t selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(size_t index);
    [[nodiscard]] std::string selectedItem() const;

    /// Emitted when user selects an item (click). Argument is the index.
    Emitter<size_t>& selected() { return m_selected; }
    [[nodiscard]] const Emitter<size_t>& selected() const { return m_selected; }

protected:
    void resizeEvent(const ResizeEvent& ev) override;

private:
    void rebuildRows();

    std::vector<std::string> m_items;
    size_t m_selectedIndex{static_cast<size_t>(-1)};
    Emitter<size_t> m_selected;
    ScrollArea* m_scrollArea{nullptr};
    Widget* m_container{nullptr};
    static constexpr f32 kRowHeight{28.f};
};

} // namespace vapi::ui

#endif // VAPI_UI_LIST_VIEW_HPP
