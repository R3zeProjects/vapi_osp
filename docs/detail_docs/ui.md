# vapi_ui — Detail Documentation

**Target:** `vapi_osp::vapi_ui`  
**Header:** `#include "ui/ui.hpp"`  
**Dependencies:** `vapi_core` only (GPU-agnostic by design)

---

## Overview

`vapi_ui` provides a lightweight, GPU-agnostic widget system. Rendering is injected at runtime via `IPainter` passed through `DrawContext`. Use `VulkanUiPainter` (from `vapi_ui_vulkan`) to render with Vulkan + font module.

---

## Signal / Slot System (`Emitter<T>`)

### `Emitter<Args...>`

Typesafe signal that delivers values to connected handlers.

```cpp
#include "ui/emitter.hpp"
```

| Method | Description |
|--------|-------------|
| `Connection bind(Fn&&)` | Connect a callable; returns a `Connection` handle |
| `void emit(Args...)` | Call all connected handlers |

### `Connection`

| Method | Description |
|--------|-------------|
| `bool isConnected() const` | True while connected |
| `void disconnect()` | Disconnect this handler |

**Example:**
```cpp
Emitter<int> sig;
int value = 0;
Connection c = sig.bind([&value](int x) { value = x; });
sig.emit(42);    // value == 42
c.disconnect();
sig.emit(99);    // value still 42
```

---

## Object Tree

### `Object`

Base for all UI objects. Provides parent/child ownership tree.

```cpp
#include "ui/object.hpp"
```

| Method | Description |
|--------|-------------|
| `Object(Object* parent = nullptr)` | Constructor; registers as child of parent |
| `Object* parent() const` | Parent pointer (nullptr for roots) |
| `std::vector<Object*> childPointers() const` | Direct children (returned by value) |
| `Object* findChild(const std::string& name) const` | Recursive name search |
| `void setObjectName(std::string name)` | Set debug name |
| `const std::string& objectName() const` | Get debug name |

---

## Widget

### `Widget : public Object, public IDrawable`

Base class for all visible widgets. Manages geometry, visibility, focus, and layout.

```cpp
#include "ui/widget.hpp"
```

#### Geometry

| Method | Description |
|--------|-------------|
| `void setGeometry(const rect& r)` | Set position and size |
| `void setGeometry(f32 x, f32 y, f32 w, f32 h)` | Convenience overload |
| `const rect& geometry() const` | Current rect |
| `f32 x(), y(), width(), height()` | Individual components |

#### Visibility and State

| Method | Description |
|--------|-------------|
| `void setVisible(bool)` | Show/hide |
| `bool isVisible() const` | Current visibility |
| `void show() / hide()` | Convenience wrappers |
| `void setEnabled(bool)` | Enable/disable interaction |
| `bool isEnabled() const` | Current enabled state |

#### Focus

| Method | Description |
|--------|-------------|
| `void setFocusPolicy(FocusPolicy)` | `NoFocus`, `TabFocus`, `ClickFocus`, `StrongFocus` |
| `void setFocus()` | Request keyboard focus |
| `void clearFocus()` | Release focus |
| `bool hasFocus() const` | True if focused |

#### Events (Emitters)

| Emitter | Trigger |
|---------|---------|
| `mousePress()` | Mouse button pressed over widget |
| `mouseRelease()` | Mouse button released |
| `mouseMove()` | Mouse moved over widget |
| `mouseEnter()` / `mouseLeave()` | Mouse enters/leaves widget area |
| `keyPress()` / `keyRelease()` | Keyboard events (when focused) |
| `resized()` | Widget geometry changed |
| `focusGained()` / `focusLost()` | Focus state changed |

#### Drawing

Override `paint(const DrawContext&)` in subclasses to draw the widget:
```cpp
void paint(const DrawContext& ctx) override {
    if (auto* p = static_cast<IPainter*>(ctx.userData))
        p->drawRect(geometry(), {0.2f,0.2f,0.2f,1.f}, {0.5f,0.5f,0.5f,1.f});
}
```

---

## Layout

### `Layout`

Arranges child widgets automatically. Assigned via `Widget::setLayout(Layout*)`.

```cpp
#include "ui/layout.hpp"
```

| Method | Description |
|--------|-------------|
| `void addWidget(Widget*, u32 stretch = 0)` | Add widget to layout |
| `void setOwner(Widget*)` | Set parent widget (called by Widget automatically) |
| `void activate()` | Recompute child positions |

**`BoxLayout`** additionally provides:

| Method | Description |
|--------|-------------|
| `void setSpacing(u32 px)` | Gap between widgets in pixels |
| `void setContentsMargins(f32 l, f32 t, f32 r, f32 b)` | Outer margins |
| `void setWidgetStretch(Widget*, u32 stretch)` | Update stretch factor of an existing widget |

---

## Widgets

### `Button`

```cpp
#include "ui/button.hpp"
Button btn(parent);
btn.setText("Click me");
btn.clicked().bind([](){ /* ... */ });
```

| Method | Description |
|--------|-------------|
| `void setText(const std::string&)` | Button label |
| `const std::string& text() const` | Current label |
| `Emitter<>& clicked()` | Fired on click |

---

### `Label`

```cpp
#include "ui/label.hpp"
Label lbl(parent);
lbl.setText("Hello");
```

| Method | Description |
|--------|-------------|
| `void setText(const std::string&)` | Display text |
| `const std::string& text() const` | Current text |

---

### `LineEdit`

Single-line text input.

```cpp
#include "ui/line_edit.hpp"
LineEdit edit(parent);
edit.textChanged().bind([](const std::string& t){ /* ... */ });
```

| Method | Description |
|--------|-------------|
| `void setText(std::string_view)` | Set content |
| `const std::string& text() const` | Current content |
| `Emitter<const std::string&>& textChanged()` | Fired on change |
| `Emitter<const std::string&>& returnPressed()` | Fired on Enter |

---

### `Checkbox`

```cpp
#include "ui/checkbox.hpp"
Checkbox cb(parent);
cb.setText("Enable feature");
cb.stateChanged().bind([](bool checked){ /* ... */ });
```

| Method | Description |
|--------|-------------|
| `void setChecked(bool)` | Set state |
| `bool isChecked() const` | Current state |
| `Emitter<bool>& stateChanged()` | State change signal |

---

### `RadioButton`

```cpp
#include "ui/radio_button.hpp"
```

Same API as `Checkbox`. Groups are managed manually (only one checked at a time within a group — application responsibility).

---

### `Slider`

```cpp
#include "ui/slider.hpp"
Slider s(parent);
s.setRange(0, 100);
s.valueChanged().bind([](int v){ /* ... */ });
```

| Method | Description |
|--------|-------------|
| `void setRange(int min, int max)` | Value range |
| `void setValue(int)` | Set current value |
| `int value() const` | Current value |
| `Emitter<int>& valueChanged()` | Value change signal |

---

### `SpinBox`

```cpp
#include "ui/spinbox.hpp"
```

Integer spin box with up/down buttons. Same range/value API as Slider.

---

### `ComboBox`

```cpp
#include "ui/combobox.hpp"
ComboBox cb(parent);
cb.addItem("Option A");
cb.addItem("Option B");
cb.currentIndexChanged().bind([](int i){ /* ... */ });
```

| Method | Description |
|--------|-------------|
| `void addItem(std::string_view)` | Append item |
| `void setCurrentIndex(int)` | Select item |
| `int currentIndex() const` | Selected index |
| `std::string_view currentText() const` | Selected text |
| `Emitter<int>& currentIndexChanged()` | Selection change signal |

---

### `ProgressBar`

```cpp
#include "ui/progressbar.hpp"
ProgressBar pb(parent);
pb.setRange(0, 100);
pb.setValue(60);
```

---

### `TextEdit`

Multi-line text area.

```cpp
#include "ui/text_edit.hpp"
TextEdit te(parent);
te.setPlainText("Hello\nWorld");
```

| Method | Description |
|--------|-------------|
| `void setPlainText(std::string_view)` | Replace content |
| `const std::string& plainText() const` | All text |
| `void appendText(std::string_view)` | Append to end |
| `Emitter<const std::string&>& textChanged()` | Text change signal |

---

### `ScrollArea`

```cpp
#include "ui/scroll_area.hpp"
ScrollArea sa(parent);
sa.setWidget(innerWidget);
```

---

### `GroupBox`

Container with a labeled border.

```cpp
#include "ui/groupbox.hpp"
GroupBox gb(parent);
gb.setTitle("Settings");
```

---

### `TabWidget`

```cpp
#include "ui/tabwidget.hpp"
TabWidget tw(parent);
tw.addTab(page1Widget, "Tab 1");
tw.addTab(page2Widget, "Tab 2");
tw.currentChanged().bind([](int idx){ /* ... */ });
```

---

### `MathArea`

Display-only widget for mathematical formula rendering.

```cpp
#include "ui/math_area.hpp"
MathArea ma(parent);
ma.setFormula("E = mc^2");
ma.formulaChanged().bind([](const std::string& f){ /* ... */ });
```

---

### `AssistantWidget`

Chat-style assistant widget with pluggable backend.

```cpp
#include "ui/assistant_widget.hpp"
#include "ui/assistant_backend.hpp"
AssistantWidget aw(parent);
aw.setBackend(myBackend);  // implement IAssistantBackend
```

#### `IAssistantBackend`

```cpp
// AssistantResponse: text chunk + done flag + optional error
struct AssistantResponse {
    std::string text;
    bool done{false};
    Error error;
};
using AssistantReplyFn = std::function<void(const AssistantResponse&)>;

class IAssistantBackend {
public:
    virtual void submit(std::string_view prompt,
                        std::string_view systemContext,
                        AssistantReplyFn reply) = 0;
    virtual void cancel() = 0;
    [[nodiscard]] virtual bool isReady() const { return true; }
};
```

The callback `reply` is called once per streaming chunk and once more with `done == true` at the end.

`createEchoBackend()`: creates a simple echo backend that reflects input — useful for testing without an LLM.

---

## IPainter

Abstract drawing interface consumed by all widget `paint()` calls.

```cpp
#include "ui/painter_interface.hpp"
class IPainter {
public:
    virtual void drawRect(const rect& r, color4 fill, color4 border) = 0;
    virtual void drawText(f32 x, f32 y, std::string_view text, color4 color, f32 pixelHeight) = 0;
    virtual void drawTextInRect(const rect& r, std::string_view text, color4 color,
                                f32 pixelHeight, u32 align) = 0;
};
```

`align` bitfield for `drawTextInRect`:
- `0` = top-left (default)
- `1` = right-align horizontally
- `2` = center horizontally
- `4` = center vertically

### VulkanUiPainter

Concrete GPU implementation. See `include/ui/vulkan_ui_painter.hpp` and [font_render.md](font_render.md).

```cpp
#include "ui/vulkan_ui_painter.hpp"
VulkanUiPainter painter;
painter.init(&device, &renderCtx, fontSource, 16.f, shaderDir);

// Per frame:
painter.beginFrame(frame);
DrawContext ctx;
ctx.userData = &painter;
rootWidget.draw(ctx);
painter.endFrame(frame);
```

---

## DrawContext

```cpp
struct DrawContext {
    rect  viewport{};          // visible region (set by root widget)
    f32   deltaTime{0.f};      // seconds since last frame
    u32   frameIndex{0};       // monotonic frame counter
    void* userData{nullptr};   // cast to IPainter* in paint()
};
```

---

## See Also

- [architecture.md](../architecture.md) — module dependency diagram
- [integration.md](../integration.md) — linking vapi_ui and vapi_ui_vulkan
- [font_render.md](font_render.md) — FontRenderApp and FontDrawList
