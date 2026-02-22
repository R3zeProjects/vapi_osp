# Python API

The **vapi** package lives in **bin/**. Add **bin/** to `PYTHONPATH` and to `PATH` (for the DLL), then:

```python
from vapi import App, AppConfig, Worksheet, check_debugger
```

## App

| Method / usage | Description |
|----------------|-------------|
| `App()` | Create application. Raises `RuntimeError` if creation fails. |
| `app.init(config=None)` | Initialize with `AppConfig(...)`. Default config: title `"VAPI"`, 1280×720, validation and vsync on. Raises `RuntimeError` on init failure. |
| `app.push_layer(layer)` | Add a layer (e.g. `Worksheet.create()`). Ownership passes to the app. |
| `app.run()` | Blocking main loop until the window is closed. |
| `app.destroy()` | Release the app. Idempotent. |
| `with App() as app: ...` | Context manager calls `destroy()` on exit. |

## AppConfig

`AppConfig(title="VAPI", width=1280, height=720, validation=True, vsync=True)` — all optional. Pass to `app.init(config)`.

## Worksheet

| Usage | Description |
|-------|-------------|
| `Worksheet.create()` | Create a Worksheet layer (canvas with blocks). |
| `app.push_layer(Worksheet.create())` | Add it to the app before `app.run()`. |

## check_debugger

`check_debugger() -> bool` — returns `True` if a debugger is attached.

## Example

```python
from vapi import App, AppConfig, Worksheet

with App() as app:
    app.init(AppConfig(title="My App", width=1280, height=720))
    app.push_layer(Worksheet.create())
    app.run()
```
