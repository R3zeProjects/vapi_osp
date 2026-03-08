# Быстрый старт VAPI

Одна страница: сборка, первый запуск и минимальный код для окна с текстом.

> **Платформа:** VAPI OSP распространяется в виде бинарных файлов для **Windows**. Сборка из исходников описана ниже.

---

## 1. Требования

**C++23**, **CMake 3.14+**, **Vulkan SDK** (заголовки, loader; при сборке примеров — `glslangValidator`), **GLFW 3.x**. При необходимости укажите пути: `-DGLFW3_ROOT=...` и `-DVulkan_ROOT=...`.

---

## 2. Сборка (Windows)

Из корня репозитория:

```
cmake -B build -S .
cmake --build build
```

Если Vulkan SDK или GLFW не в PATH:

```
cmake -B build -S . -DGLFW3_ROOT=C:/GLFW -DVulkan_ROOT=C:/VulkanSDK/1.4.321.1
cmake --build build
```

---

## 3. Первый запуск

После сборки запустите пример «окно с текстом»:

```
build\Debug\calculator_example.exe
```

Должно открыться окно с текстом. Шейдеры для этого примера собираются в каталог `build` (CMake передаёт путь в исполняемый файл).

---

## 4. Путь к шейдерам (для своего приложения)

`FontRenderApp::init(..., shaderDir)` ожидает каталог с файлами `font_quad.vert.spv` и `font_quad.frag.spv`. При сборке через наш CMake они лежат в `build/examples/shaders` (или в каталоге, заданном в `SHADER_DIR`). В своём проекте либо копируйте эти файлы, либо собирайте SPIR-V и передавайте путь в `init()`.

---

## 5. Минимальный код

Подключите `vapi_font_render` и заголовок `render/font_render_app.hpp`. Пример:

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    // shaderDir — каталог с font_quad.vert.spv и font_quad.frag.spv
    auto initRes = app.init(800, 600, "Моё окно", "/путь/к/шейдерам");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    app.setFont("Arial");   // имя или путь к .ttf/.otf
    app.setFontSize(24.f);
    app.addText(50.f, 80.f, "Привет, мир!", vapi::color4(1.f, 1.f, 1.f, 1.f));

    return app.run();
}
```

Порядок: **init** → **setFont** / **setFontSize** (опционально) → **addText** → **run**.

**Использование как зависимость:** после установки (`cmake --install build`) в своём проекте: `find_package(vapi_osp REQUIRED)` и `target_link_libraries(my_app PRIVATE vapi_osp::vapi_font_render)`. Шейдеры при установке копируются в `<prefix>/share/vapi_osp/shaders/` — соберите их в SPIR-V или передайте этот путь в `init(..., shaderDir)`. Подробнее — [integration.md](integration.md).

Подключение VAPI как подпроекта (`add_subdirectory`) и выбор библиотек по сценарию — в [integration.md](integration.md). Полное руководство — [user_guide.md](user_guide.md), архитектура — [architecture.md](architecture.md).
