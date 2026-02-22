# VAPI — Python (Windows)

Python 3.14+ bindings for VAPI: window, Vulkan rendering, and Worksheet (canvas with blocks). The **vapi** adapter lives in **bin/**; no compilation.

**Version:** 0.0.1  
**Platform:** Windows (Python 3.14+)

---

## Requirements

- **Python 3.14+**
- **Windows**
- **bin/** must contain: `libvapi_shared.dll`, `shaders/`, and (if needed) `vulkan-1.dll`, `glfw3.dll`.

No pip dependencies.

---

## Contents

| Path | Description |
|------|-------------|
| **bin/** | `libvapi_shared.dll`, `shaders/`, and the **vapi** Python adapter (package `vapi`). Add `vulkan-1.dll`, `glfw3.dll` when needed. |
| **docs/** | Python documentation: overview, API, Worksheet. |
| **examples/** | Python example: **main.py**. |
| **licenses/** | VAPI, GLFW, Vulkan. |
| **CONTACT.txt** | Licensing (including commercial). |

---

## Quick start

From repo root:

```cmd
set PATH=%CD%\bin;%PATH%
set PYTHONPATH=%CD%\bin
python examples/main.py
```

Or run the package as a module (same PATH and PYTHONPATH):

```cmd
set PATH=%CD%\bin;%PATH%
set PYTHONPATH=%CD%\bin
python -m vapi
```

---

## Package (pip)

Install the **vapi** package in development mode:

```cmd
pip install -e .
```

Or install for the user (no `-e`):

```cmd
pip install .
```

After install, ensure **bin/** is on `PATH` (so `libvapi_shared.dll` and `shaders/` are found). Then you can run:

```cmd
vapi
```
or `python -m vapi`, or `from vapi import App, AppConfig, Worksheet` in your code.

Build a source distribution or wheel (optional):

```cmd
pip install build
python -m build
```

Artifacts appear in `dist/`. The package contains only the Python adapter; the user must have **bin/** (DLL and shaders) on `PATH` at runtime.

---

## API

| Python | Description |
|--------|-------------|
| `App()` | Create application. |
| `app.init(config)` | Init with `AppConfig(title=..., width=..., height=..., validation=..., vsync=...)`. |
| `app.push_layer(ws)` | Add layer (e.g. Worksheet). |
| `app.run()` | Main loop (blocking). |
| `app.destroy()` | Release (or use `with App() as app: ...`). |
| `Worksheet.create()` | Create Worksheet layer. |
| `check_debugger()` | True if debugger attached. |

Example:

```python
from vapi import App, AppConfig, Worksheet

with App() as app:
    app.init(AppConfig(title="My App", width=1280, height=720))
    app.push_layer(Worksheet.create())
    app.run()
```

---

## Docs

See **docs/README.md** for the full index; **docs/01-overview.md**, **docs/02-api.md**, **docs/03-worksheet.md**.

---

## License

See **licenses/**; **CONTACT.txt** for commercial use.
