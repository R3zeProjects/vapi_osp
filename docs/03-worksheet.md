# Worksheet (Python)

**Worksheet** is a layer that draws a canvas with a grid, blocks, and selection. You can pan and zoom the canvas. It is the only layer type exposed in the current Python API.

## Usage

Create the layer with `Worksheet.create()` and add it to the app before running:

```python
from vapi import App, AppConfig, Worksheet

with App() as app:
    app.init(AppConfig(title="Worksheet demo"))
    app.push_layer(Worksheet.create())
    app.run()
```

After `app.push_layer(ws)`, the app owns the layer; you do not destroy it yourself. Closing the window and exiting the context (or calling `app.destroy()`) cleans up.

## Behaviour

The canvas is rendered before the main UI. The layer provides grid, blocks, and selection; pan/zoom are handled inside the layer. For more control (blocks, types, etc.) a future API may expose additional Python bindings.
