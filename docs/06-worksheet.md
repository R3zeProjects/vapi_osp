# Worksheet

Canvas module with blocks and selection. Namespace **vapi::ws**. Exposed via C API as **VAPI_WorksheetCreate()**.

## Purpose

**Worksheet** is an application layer that draws a canvas with grid, blocks, and selection. Canvas is rendered before main UI. Control: create/delete blocks, selection, pan/zoom.

## C API

**VAPI_WorksheetCreate()** — creates the Worksheet layer. Pass the result to **VAPI_AppPushLayer(app, layer)**.

## Example (C API)

```c
VAPI_App app = VAPI_AppCreate();
VAPI_AppInit(app, &cfg);
VAPI_AppPushLayer(app, VAPI_WorksheetCreate());
VAPI_AppRun(app);
```

## Types (internal)

BlockId, BlockType, CanvasPoint, CanvasRect, WorksheetConfig. Block, Canvas, Grid, Selection — see headers when using full C++ API.
