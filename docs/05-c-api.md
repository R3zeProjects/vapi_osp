# C API (DLL) — Windows

Minimal C API for using VAPI from a DLL. Header: **vapi_c_api.hpp** (in bin/).

## Types

**VAPI_App** — application pointer (opaque). **VAPI_Layer** — layer pointer (opaque). **VAPI_AppConfig**: title (const char*), width, height (int), validation (int, 1 = enable), vsync (int, 1 = enable).

## Functions

**VAPI_AppCreate()** — creates application. **VAPI_AppDestroy(app)** — destroys it. **VAPI_AppInit(app, &cfg)** — init; 0 = success, -1 = error. **VAPI_AppRun(app)** — main loop (blocking). **VAPI_AppPushLayer(app, layer)** — add layer; ownership transferred to app. **VAPI_WorksheetCreate()** — creates Worksheet layer; pass result to VAPI_AppPushLayer. **VAPI_CheckDebugger()** — returns 1 if debugger detected, else 0.

## Example

```c
VAPI_App app = VAPI_AppCreate();
if (!app) return 1;
VAPI_AppConfig cfg = {};
cfg.title = "My App"; cfg.width = 1280; cfg.height = 720;
cfg.validation = 1; cfg.vsync = 1;
if (VAPI_AppInit(app, &cfg) != 0) { VAPI_AppDestroy(app); return 1; }
VAPI_Layer ws = VAPI_WorksheetCreate();
if (ws) VAPI_AppPushLayer(app, ws);
VAPI_AppRun(app);
VAPI_AppDestroy(app);
```

## Linking (Windows)

Link with vapi.lib (MSVC) or libvapi_shared.dll.a (MinGW). At run time place the VAPI DLL, vulkan-1.dll, and shaders/ next to the exe (or run from bin/).
