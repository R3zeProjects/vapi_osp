# VAPI C API example (Windows)

Minimal example: VAPI window with Worksheet layer.

## Prerequisites

1. Fill **bin/:** Copy contents of the project `dll/` folder into **ExportVersionWindows/bin/** (DLL, vapi_c_api.hpp, import lib, shaders/, vulkan-1.dll, glfw3.dll if needed).
2. Vulkan SDK or runtime (vulkan-1.dll).

## Build (MSVC)

```cmd
cl /I..\bin main.c /link ..\bin\vapi.lib /OUT:vapi_example.exe
```

## Build (MinGW)

```cmd
gcc -I../bin main.c -L../bin -lvapi_shared -o vapi_example.exe
```

## Run

Run from **bin/** so the VAPI DLL, vulkan-1.dll, and **shaders/** are found (or add bin/ to PATH).
