# Примеры использования VAPI OSP

Краткие примеры типичных сценариев: окно с текстом и интерактивное приложение. Все примеры используют только API **FontRenderApp** и **IPlatform** (без прямого Vulkan и GLFW в коде).

---

## 1. Минимальное окно с текстом

Один вызов `app.run()` — блокирующий цикл до закрытия окна. Текст задаётся до `run()` и не меняется.

```cpp
#include "render/font_render_app.hpp"
#include "core/error.hpp"
#include <cstdio>

int main() {
    vapi::FontRenderApp app;
    // shaderDir — каталог с font_quad.vert.spv и font_quad.frag.spv
    auto r = app.init(800, 400, "Заголовок", "path/to/shaders");
    if (!r) {
        std::fprintf(stderr, "init failed: %s\n", vapi::errorMessage(r.error()).data());
        return vapi::kRunError;
    }
    app.setFont("Arial");
    app.setFontSize(24.f);

    app.addText(50.f, 80.f, "Первая строка", vapi::color4(1.f, 1.f, 1.f, 1.f));
    app.addText(50.f, 110.f, "Вторая строка", vapi::color4(0.9f, 0.9f, 1.f, 1.f));

    return app.run();
}
```

**CMake:** линковать с `vapi_font_render`, добавить `target_compile_definitions(my_app PRIVATE SHADER_DIR="${VAPI_FONT_SHADER_DIR}")` если шейдеры берутся из дерева сборки VAPI.

---

## 2. Интерактивное приложение (свой цикл)

Каждый кадр: обработка событий, начало кадра, очистка, отрисовка текста и «кнопок», конец кадра. Ввод — через `InputCallbacks` и `getCursorPosInFramebuffer()`.

```cpp
#include "render/font_render_app.hpp"
#include "platform/platform_codes.hpp"
#include "core/types.hpp"
#include "core/error.hpp"
#include <cstdio>
#include <string>

#ifdef SHADER_DIR
#define VAPI_SHADER_DIR SHADER_DIR
#else
#define VAPI_SHADER_DIR "."
#endif

int main() {
    using namespace vapi;

    FontRenderApp app;
    auto initRes = app.init(320, 200, "Example", VAPI_SHADER_DIR);
    if (!initRes) {
        std::fprintf(stderr, "init failed: %s\n", errorMessage(initRes.error()).data());
        return kRunError;
    }
    app.setFont("Arial");
    app.setFontSize(22.f);

    IPlatform* platform = app.getPlatform();
    WindowId winId = app.getWindowId();
    if (!platform || winId == 0) return kRunError;

    int counter = 0;
    InputCallbacks icb;
    icb.onKey = [platform, winId](WindowId id, s32 key, s32, s32 action, s32) {
        if (key == static_cast<s32>(Key::Escape) && action == static_cast<s32>(KeyAction::Press))
            platform->requestClose(id);
    };
    icb.onMouseButton = [&counter, platform, winId](WindowId id, s32 button, s32 action, s32) {
        if (button != static_cast<s32>(MouseButton::Left) || action != static_cast<s32>(KeyAction::Press))
            return;
        auto [fx, fy] = platform->getCursorPosInFramebuffer(id);
        WindowState ws = platform->getWindowState(id);
        float w = static_cast<float>(ws.framebufferWidth > 0 ? ws.framebufferWidth : 1);
        float h = static_cast<float>(ws.framebufferHeight > 0 ? ws.framebufferHeight : 1);
        // Кнопка в центре (примерно 100x40)
        float bx = w * 0.5f - 50.f, by = h * 0.5f - 20.f;
        if (fx >= bx && fx <= bx + 100.f && fy >= by && fy <= by + 40.f)
            counter++;
    };
    platform->setInputCallbacks(winId, icb);

    while (platform->anyWindowOpen()) {
        platform->pollEvents();
        auto frameRes = app.beginFrame();
        if (!frameRes) {
            if (frameRes.error() == errors::NotInitialized) break;
            continue;
        }
        FrameInfo fi = *frameRes;
        app.clear();

        float w = static_cast<float>(fi.width), h = static_cast<float>(fi.height);
        float lineH = app.getLineHeight();
        app.addText(20.f, 20.f, "Clicks: " + std::to_string(counter), color4(0.95f, 0.95f, 1.f, 1.f));
        app.addText(w * 0.5f - 30.f, h * 0.5f - lineH * 0.5f, "Click", color4(0.4f, 0.7f, 1.f, 1.f));

        if (!app.endFrame()) break;
    }
    return kRunSuccess;
}
```

---

## 3. Структура как в калькуляторе

В [examples/calculator_example.cpp](../../examples/calculator_example.cpp):

- Сетка кнопок задаётся относительными координатами (доли ширины/высоты окна).
- Клик обрабатывается в `onMouseButton`: нормализованные `(fx/fbW, fy/fbH)` сравниваются с границами ячеек.
- Анимация нажатия: запоминаются последняя нажатая кнопка и время; в цикле отрисовки считается `pressFactor` через `applyEasing(Easing::EaseOutCubic, t)` и сдвигается текст/цвет.

Тот же подход в [counter_example.cpp](../../examples/counter_example.cpp) (одна кнопка) и [stopwatch_example.cpp](../../examples/stopwatch_example.cpp) (три кнопки, таймер).

---

## 4. Зависимости и сборка

| Сценарий        | Цель CMake       | Заголовки                          |
|-----------------|------------------|------------------------------------|
| Окно с текстом  | `vapi_font_render` | `render/font_render_app.hpp`, `core/error.hpp` |
| Платформа/ввод  | (входит в font_render) | `platform/platform_codes.hpp` для Key, MouseButton, KeyAction |

Путь к шейдерам передаётся в `app.init(width, height, title, shaderDir)`. При сборке через CMake VAPI можно использовать `SHADER_DIR="${VAPI_FONT_SHADER_DIR}"` (каталог с собранными `font_quad.vert.spv`, `font_quad.frag.spv`).

См. также: [integration.md](integration.md), [quick_start.md](quick_start.md).
