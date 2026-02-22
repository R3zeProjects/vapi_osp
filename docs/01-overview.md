# VAPI overview (Python)

VAPI is a library for applications with a window and UI: window (GLFW), rendering (Vulkan), and application layers. The Python distribution exposes a small API via the **vapi** package in **bin/**.

## What you get

- **App** — create a window, init with config (title, size, validation, vsync), run the main loop.
- **Worksheet** — a layer that draws a canvas with grid, blocks, and selection (pan/zoom).
- **No compilation** — ctypes loads **libvapi_shared.dll** from **bin/**; you only need Python 3.14+ and Windows.

## Layout

- **bin/** — `libvapi_shared.dll`, `shaders/`, and the **vapi** Python package. Add `vulkan-1.dll`, `glfw3.dll` here if needed.
- **examples/main.py** — minimal runnable example.
- **docs/** — this documentation.

## Dependencies

- **Vulkan** (vulkan-1.dll) and **GLFW** (glfw3.dll) when not in **bin/**; see root README.
