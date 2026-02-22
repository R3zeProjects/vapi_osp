# VAPI Distribution — Windows

This package contains **libvapi_shared.dll**, documentation, examples, header and shader sources, license, and contact. **Dependency DLLs** (vulkan-1.dll, glfw3.dll) are not included; copy them into **bin/** from your VAPI build or system as described below.

**Version:** 0.0.1b1 (alpha)  
**Platform:** Windows (DLL, MSVC / MinGW)

## Contents

| Folder / file   | Description |
|-----------------|-------------|
| **bin/**        | **libvapi_shared.dll**, `vapi_c_api.hpp`, `shaders/`. Add dependency DLLs (vulkan-1.dll, glfw3.dll) from your VAPI build or Vulkan/GLFW install if not already in `bin/`. |
| **docs/**       | API documentation (overview, C API, worksheet, etc.). |
| **examples/**   | Minimal example source (C API); build and run instructions for Windows. |
| **licenses/**   | VAPI license and third-party licenses (GLFW, Vulkan). |
| **CONTACT.txt** | Licensing inquiries (including commercial use). |

## Quick start

1. **Dependencies in bin/:** If needed, add vulkan-1.dll and glfw3.dll to `bin/` (from your VAPI build `dll/` output or Vulkan SDK / GLFW).
2. **Build the example:** See `examples/README.md` (link against libvapi_shared: vapi.lib for MSVC or libvapi_shared.dll.a for MinGW; include path = bin/).
3. **Run:** Run your executable from `bin/` or add `bin/` to PATH so libvapi_shared.dll and `shaders/` are found.

## Requirements

- Windows: Vulkan Loader (vulkan-1.dll) and GLFW (glfw3.dll) if not in `bin/`; copy from your VAPI build or install.
- See **docs/** for API and **licenses/LICENSE_VAPI.txt** for terms of use.
