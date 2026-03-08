# Руководство по интеграции VAPI

Документ описывает, как подключить VAPI к вашему проекту: требования, сборка, выбор библиотек и минимальный код для типичных сценариев.

> **Платформа:** VAPI OSP распространяется в виде бинарных файлов для **Windows**. Сборка из исходников поддерживается на Windows.

---

## 1. Требования

| Компонент | Версия / примечание |
|-----------|----------------------|
| **C++** | C++23 (проект использует `std::expected`, сборка с `CMAKE_CXX_STANDARD 23`) |
| **CMake** | 3.14+ |
| **Vulkan** | Vulkan SDK (заголовки и loader). Переменная `Vulkan_ROOT` или стандартный `find_package(Vulkan)` |
| **GLFW** | 3.x (окно и ввод). Опционально, если используете только core/font без окна |

При необходимости укажите пути при конфигурации:
- `-DGLFW3_ROOT=C:/path/to/glfw`
- `-DVulkan_ROOT=C:/VulkanSDK/1.4.321.1`

---

## 2. Способы интеграции

### 2.1. Добавление как подпроект (add_subdirectory)

Подключите репозиторий в ваш проект (git submodule, копия каталога и т.п.) и в корневом `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/vapi_osp)
```

Все цели VAPI (`vapi_core`, `vapi_platform`, `vapi_gpu`, `vapi_render`, `vapi_resource`, `vapi_font`, `vapi_font_render`, `vapi_ui`) станут доступны. Подключайте только нужные (см. раздел 3).

### 2.2. Установка и find_package

После установки VAPI (`cmake --install build` или при упаковке) в вашем проекте:

```cmake
find_package(vapi_osp 0.0.1 REQUIRED)  # текущая версия API
# ...
target_link_libraries(your_app PRIVATE vapi_osp::vapi_font_render)
```

Убедитесь, что `vapi_osp_DIR` указывает на каталог с `vapi_ospConfig.cmake` (обычно `<prefix>/lib/cmake/vapi_osp`). Зависимость Vulkan подтягивается автоматически; GLFW должен быть доступен в вашем проекте (find_package(glfw3) или `GLFW3_ROOT`). Имена целей для потребителей — с пространством имён: `vapi_osp::vapi_core`, `vapi_osp::vapi_font_render` и т.д.

Опции сборки при сборке самого VAPI: `VAPI_BUILD_EXAMPLES` (по умолчанию ON), `VAPI_BUILD_TESTS` (по умолчанию ON), `VAPI_BUILD_SHARED_LIBS` (по умолчанию OFF). Для сборки в виде динамических библиотек (DLL на Windows) задайте `-DVAPI_BUILD_SHARED_LIBS=ON`. При использовании как подпроекта можно отключить примеры/тесты: `-DVAPI_BUILD_EXAMPLES=OFF -DVAPI_BUILD_TESTS=OFF`.

---

## 3. Выбор библиотек по сценарию

Подключайте только те цели, которые реально используете. Заголовки указываются относительно каталога `include/` репозитория VAPI.

| Сценарий | Цели для `target_link_libraries` | Основные заголовки |
|----------|-----------------------------------|--------------------|
| Только ядро (типы, ошибки, файлы) | `vapi_core` | `vapi.hpp` или `core/types.hpp`, `core/error.hpp`, `core/file_utils.hpp` |
| Окно и ввод (GLFW) | `vapi_platform` (тянет `vapi_core`, glfw) | `platform/platform.hpp` |
| Vulkan: instance, device, swapchain | `vapi_gpu` (тянет `vapi_core`, Vulkan::Vulkan, glfw) | `gpu/gpu.hpp`, `gpu/gpu_context.hpp` |
| Рендер: шейдеры, пайплайны, контекст кадра | `vapi_render` (тянет `vapi_gpu`) | `render/render.hpp`, `render/render_context.hpp` |
| Буферы, изображения, дескрипторы, staging | `vapi_resource` (тянет `vapi_gpu`) | `resource/resource.hpp` |
| Шрифты, layout, атлас, FontDrawList (без GPU) | `vapi_font` (тянет `vapi_core`) | `font/text_painter.hpp`, `font/font_draw_list.hpp`, `font/font_types.hpp` |
| Готовое приложение «окно + Vulkan + текст» | `vapi_font_render` (тянет render, resource, font, platform) | `render/font_render_app.hpp` |
| UI-виджеты (без Vulkan) | `vapi_ui` (тянет `vapi_core`) | `ui/ui.hpp`, `ui/widget.hpp`, `ui/button.hpp` и т.д. |
| UI-рендеринг через Vulkan | `vapi_ui_vulkan` (тянет ui, render, resource, font) | `ui/vulkan_ui_painter.hpp` |

Минимальный вариант «одно окно с текстом» — одна цель **`vapi_font_render`** и один заголовок **`render/font_render_app.hpp`**.

---

## 4. Минимальный пример: окно с текстом (FontRenderApp)

Используется один класс `FontRenderApp`: инициализация, шрифт, добавление строк, запуск цикла.

**Код приложения:**

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    auto initRes = app.init(800, 600, "Моё приложение", "/path/to/shaders");
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(initRes.error()).data());
        return vapi::kRunError;
    }

    app.setFont("Arial");   // или путь к .ttf/.otf
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "Привет, VAPI!", vapi::color4(1.f, 1.f, 1.f, 1.f));
    return app.run();
}
```

Важно: в `init(..., shaderDir)` передаётся каталог с скомпилированными `font_quad.vert.spv` и `font_quad.frag.spv`. Исходники GLSL — в репозитории в `examples/shaders/`; компиляция в SPIR-V: `glslangValidator -V font_quad.vert -o font_quad.vert.spv` (и аналогично для .frag).

---

## 5. Шейдеры для FontRenderApp

Приложение ожидает в `shaderDir` два файла: `font_quad.vert.spv`, `font_quad.frag.spv`. При установке VAPI (`cmake --install build`) исходники копируются в `<prefix>/share/vapi_osp/shaders/`. В дереве сборки VAPI при наличии `glslangValidator` создаётся цель `font_shaders` и каталог с собранными `.spv`. Свой пример может зависеть от этой цели и передавать этот каталог в `init(..., shaderDir)` через макрос `SHADER_DIR="${VAPI_FONT_SHADER_DIR}"` (см. [quick_start.md](quick_start.md)).

---

## 6. Пример: только шрифты без окна (headless)

Если нужно только измерять и раскладывать текст, получать вершины и атлас:

```cmake
target_link_libraries(my_font_tool PRIVATE vapi_font)
```

```cpp
#include "font/font_draw_list.hpp"
#include "font/font_types.hpp"
#include "font/font_service.hpp"

vapi::FontService fonts;
auto idRes = fonts.loadFont("/path/to/font.ttf");
vapi::IFontSource* fontSrc = fonts.getFont(*idRes);

vapi::FontDrawList list;
list.setFont(fontSrc, 24.f);
list.addText(0.f, 0.f, "Hello", vapi::color4(1, 1, 1, 1));
const auto& vertices = list.getVertices();
std::vector<vapi::u8> atlasRgba;
list.fillAtlasRgba(atlasRgba);
// Дальше — своя загрузка в буферы/текстуры и draw
```

---

## 7. Краткая шпаргалка по целям и заголовкам

| Что нужно | Цель | Include |
|-----------|------|--------|
| Всё в одном (окно + Vulkan + текст) | `vapi_font_render` | `render/font_render_app.hpp` |
| Только шрифты (layout, вершины, атлас) | `vapi_font` | `font/font_draw_list.hpp`, `font/text_painter.hpp` |
| Окно + ввод | `vapi_platform` | `platform/platform.hpp` |
| Vulkan (device, swapchain) | `vapi_gpu` | `gpu/gpu_context.hpp` |
| Рендер (кадр, пайплайны) | `vapi_render` | `render/render_context.hpp` |
| Буферы, текстуры, staging | `vapi_resource` | `resource/resource.hpp` |
| UI виджеты (без GPU) | `vapi_ui` | `ui/ui.hpp`, `ui/widget.hpp` |
| UI рендеринг (Vulkan) | `vapi_ui_vulkan` | `ui/vulkan_ui_painter.hpp` |
| Типы, ошибки, файлы | `vapi_core` | `vapi.hpp` или `core/error.hpp`, `core/types.hpp` |

Дополнительно: [architecture.md](architecture.md), [quick_reference.md](quick_reference.md). Детальная справка по API: [../detail_docs/](../detail_docs/).
