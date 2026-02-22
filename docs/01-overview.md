# VAPI Overview

VAPI is a C++26 library for applications with UI: window (GLFW), rendering (Vulkan), widget tree, application layers.

## Modules

- **core** — types, logger, errors, utilities
- **platform** — window and input (GLFW)
- **gpu** — Vulkan: instance, device, swapchain, sync
- **resource** — buffers, descriptors, images
- **render** — render context, shaders, pipeline
- **ui** — UIManager, widgets, themes
- **app** — Application, AppConfig, AppLayer

## C API (this distribution)

This distribution provides the **C API** (see 05-c-api.md): VAPI_AppCreate, VAPI_AppInit, VAPI_AppRun, VAPI_WorksheetCreate, etc. Link with the library in **bin/** and include **vapi_c_api.hpp**. Windows: use vapi.lib and the DLLs in bin/.

## Dependencies

- **GLFW** — window and input
- **Vulkan** — rendering (required)
