# VAPI quick start

One page: build, first run, and minimal code for a window with text.

> **Platform:** VAPI OSP is distributed as binaries for **Windows**. Building from source is described below.

---

## 1. Requirements

**C++23**, **CMake 3.14+**, **Vulkan SDK** (headers, loader; for building examples also `glslangValidator`), **GLFW 3.x**. Set paths if needed: `-DGLFW3_ROOT=...` and `-DVulkan_ROOT=...`.

---

## 2. Build (Windows)

From the repository root:

```
cmake -B build -S .
cmake --build build
```

If Vulkan SDK or GLFW are not in PATH:

```
cmake -B build -S . -DGLFW3_ROOT=C:/GLFW -DVulkan_ROOT=C:/VulkanSDK/1.4.321.1
cmake --build build
```

---

## 3. First run

After building, run the “window with text” example:

```
build\Debug\calculator_example.exe
```

A window with text should open. Shaders for this example are built into the `build` directory (CMake passes the path to the executable).

---

## 4. Shader path (for your app)

`FontRenderApp::init(..., shaderDir)` expects a directory with `font_quad.vert.spv` and `font_quad.frag.spv`. When building with our CMake they are in `build/examples/shaders` (or the directory set in `SHADER_DIR`). In your project either copy these files or build SPIR-V and pass the path to `init()`.

---

## 5. Minimal code

Link `vapi_font_render` and include `render/font_render_app.hpp`. Example:

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    // shaderDir — directory with font_quad.vert.spv and font_quad.frag.spv
    auto initRes = app.init(800, 600, "My window", "/path/to/shaders");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    app.setFont("Arial");   // name or path to .ttf/.otf
    app.setFontSize(24.f);
    app.addText(50.f, 80.f, "Hello, world!", vapi::color4(1.f, 1.f, 1.f, 1.f));

    return app.run();
}
```

Order: **init** → **setFont** / **setFontSize** (optional) → **addText** → **run**.

**Using as dependency:** After install (`cmake --install build`), in your project: `find_package(vapi_osp REQUIRED)` and `target_link_libraries(my_app PRIVATE vapi_osp::vapi_font_render)`. Shaders are installed to `<prefix>/share/vapi_osp/shaders/` — compile them to SPIR-V or pass that path in `init(..., shaderDir)`. See [integration.md](integration.md) for more.

Using VAPI as a subproject (`add_subdirectory`) and choosing libraries by scenario: [integration.md](integration.md). Full guide: [user_guide.md](user_guide.md), architecture: [architecture.md](architecture.md).
