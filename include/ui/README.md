# UI system

Original implementation: event/reaction system (analog of signals and slots), object tree, widgets, layouts, Assistant module (neural network connection), and MathArea (formula work area).

## Uniqueness and rights

- **Original design and code**: not derived from Qt or any third-party UI API. Naming (Emitter, Reaction, bind, Connection) and architecture are independent.
- **Replaceable**: you can swap implementations (e.g. another backend for Assistant) without changing the public API.

## Event system (reactions)

- **Emitter\<Args...\>**: type-safe event source. `bind(callable)` returns a **Connection**; `emit(args...)` invokes all bound reactions.
- **Connection**: `disconnect()` to unbind; connections are invalidated when the emitter is destroyed.
- Use from any object: e.g. `button->clicked().bind([]() { ... });`

## Object tree

- **Object**: parent/child, `objectName`, `findChild(name)`, `destroyed()` emitter.
- **addChildRaw(child)** / **addChild(unique_ptr)** for ownership. Creating with parent: `new Button(parent)`.

## Widgets and layouts

- **Widget**: geometry, visible, enabled, focus, layout; **draw(ctx)** calls **paint(ctx)** and draws children; **IPainter** (via `ctx.userData`) for actual rendering.
- **Layouts**: **BoxLayout** (horizontal/vertical), **GridLayout**, **StackLayout**.
- **Widgets**: Button, Label, LineEdit, CheckBox, RadioButton (+ RadioGroup), Slider, SpinBox, ComboBox, ProgressBar, TextEdit, ScrollArea, GroupBox, TabWidget.

## Assistant module

- **IAssistantBackend**: `submit(prompt, systemContext, replyCallback)`. Implement to connect your LLM/API (OpenAI-compatible HTTP, local model, etc.).
- **AssistantWidget**: UI panel; set backend with **setBackend()**; **responseReceived(text, done)** emitter.
- **createEchoBackend()**: simple echo backend for tests; replace with your backend in production.

## MathArea

- Formula input with segment parsing (numbers, identifiers, operators, parentheses). **formula()** / **setFormula()**, **formulaChanged()** emitter, cursor, keyboard navigation. Style similar to a MathCad-style work area.

## Integration with DrawContext

Pass an **IPainter*** in **DrawContext::userData** when drawing the widget tree; widgets will use it in **paint()** for rects and text (e.g. with your font module).
