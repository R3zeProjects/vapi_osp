# Руководство пользователя VAPI

Это руководство описывает, как установить VAPI, собрать проект, запустить примеры и начать использовать платформу в своих приложениях.

---

## 1. О платформе

**VAPI** — платформа для построения собственных API поверх Vulkan с сохранением контроля над контекстом, обёртками и инструментами. Она не подменяет Vulkan, а упорядочивает его использование и даёт единые контракты.

**Основные возможности:**

- **Окно и ввод** — через GLFW (создание окна, события, опрос).
- **Vulkan** — instance, device, surface, swapchain, синхронизация, команды.
- **Ресурсы** — буферы, изображения, дескрипторы, сэмплеры, staging-загрузка.
- **Рендер** — шейдеры, пайплайны (graphics/compute), контекст кадра и динамический рендер.
- **Шрифты** — загрузка шрифтов (TTF/OTF), измерение и раскладка текста, атлас глифов, отрисовка текста в окне или headless (без GPU).

**Типичные сценарии:**

1. **Окно с текстом** — готовое приложение `FontRenderApp`: минимум кода, окно + Vulkan + шрифт + цикл отрисовки.
2. **Только шрифты (headless)** — измерение и раскладка текста, получение геометрии (вершины + атлас) без окна и Vulkan.
3. **Свой рендер** — полный контроль: своё окно, `GpuContext`, `RenderContext`, ресурсы, свои пайплайны и геометрия (в т.ч. из `FontDrawList` или `TextPainter`).

Подробнее об архитектуре — в [architecture.md](architecture.md).

---

## 2. Требования

- **Компилятор C++23** (MSVC или MinGW/GCC на Windows).
- **CMake** 3.14 или новее.
- **Vulkan SDK** — заголовки и библиотеки Vulkan; в PATH или задаётся `Vulkan_ROOT` при конфигурации.
- **GLFW 3.x** — для окна и ввода. Либо устанавливается в системе и находится через `find_package(glfw3)`, либо указывается `GLFW3_ROOT`.
- **glslangValidator** (из Vulkan SDK) — для компиляции GLSL-шейдеров в SPIR-V; нужен для примеров (calculator, counter, stopwatch).

**Поддерживаемые платформы:** Windows (MSVC, MinGW). Библиотека распространяется в виде скомпилированных бинарных файлов для **Windows**. Сборка из исходников поддерживается на Windows.

**Публичный API:** заголовки в `include/`. До версии 1.0 стабильность контрактов не гарантируется; возможны несовместимые изменения.

---

## 3. Сборка проекта

Из корня репозитория:

```
cmake -B build -S .
cmake --build build
```

При необходимости укажите пути к зависимостям:

```
cmake -B build -S . -DGLFW3_ROOT=C:/GLFW -DVulkan_ROOT=C:/VulkanSDK/1.4.321.1
```

**Что собирается:** библиотеки (`vapi_core`, `vapi_platform`, `vapi_gpu`, `vapi_render`, `vapi_resource`, `vapi_font`, `vapi_font_render`, `vapi_ui`), исполняемые файлы (`vapi_main`, `calculator_example`, `counter_example`, `stopwatch_example`), тесты. Шейдеры для примеров компилируются в SPIR-V в каталог сборки (`examples/shaders`), если найден `glslangValidator`.

---

## 4. Быстрый старт

**Запуск примера «окно с текстом»:** после сборки выполните `build\Debug\calculator_example.exe`. Должно открыться окно калькулятора.

**Минимальный код с FontRenderApp:** подключите заголовки и скомпонуйте с `vapi_font_render`; создайте приложение, инициализируйте, задайте шрифт и размер, добавьте строки, запустите цикл. Порядок вызовов: **init** → опционально **setFont** / **setFontSize** → **addText** (0 или больше раз) → **run**. Подробнее — [quick_start.md](quick_start.md), [architecture.md](architecture.md).

---

## 5. Основные сценарии

- **Окно с текстом (FontRenderApp):** init → setFont / setFontSize → addText → run. Заголовок: `render/font_render_app.hpp`. Примеры: calculator_example.cpp, counter_example.cpp, stopwatch_example.cpp.
- **Только шрифты (без окна и Vulkan):** TextPainter (measureText, layoutText, drawText/paint → TextGeometry) или FontDrawList (setFont → addText → getVertices(), fillAtlasRgba/getAtlasRgba).
- **Свой рендер:** createPlatform → окно → GpuContext::init → RenderContext::init → ресурсы → цикл beginFrame → beginRendering → draw → endRendering → endFrame; при resize — recreateSwapchain и handleResize. Детали — в [architecture.md](architecture.md), [../detail_docs/render.md](../detail_docs/render.md).

---

## 6. Ошибки и проверка результатов

Многие функции VAPI возвращают `Result<T>`. Проверка: `if (!result)` — ошибка; `result.error()` — код ошибки; `vapi::errorMessage(result.error())` — текстовое описание для логов и вывода пользователю.

---

## 7. Тестирование

Полный прогон тестов из корня проекта:

```
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

Часть тестов требует дисплей (окно); в headless-среде они пропускаются. Подробнее — [testing.md](testing.md).

---

## 8. Частые проблемы

- **«init failed» / не создаётся окно** — проверьте, что GLFW и Vulkan SDK установлены и доступны; при конфигурации задайте `-DGLFW3_ROOT` и `-DVulkan_ROOT`, если библиотеки не в PATH.
- **Шейдеры не найдены** — для `FontRenderApp` нужна директория с скомпилированными SPIR-V (`font_quad.vert.spv`, `font_quad.frag.spv`). В примерах передаётся каталог из сборки (`SHADER_DIR`); в своём приложении укажите корректный путь.
- **setFont не находит шрифт** — можно передать полный путь к `.ttf`/`.otf` или имя шрифта (поиск по системным путям).
- **Тесты падают в CI без дисплея** — часть тестов требует окно; они описаны в [testing.md](testing.md). Используйте `--headless` для `vapi_main` и при необходимости исключайте GUI-тесты в headless-среде.

---

*Руководство актуально для версии API VAPI 0.0.2b3 (версия в `CMakeLists.txt`, документация в `docs/`). Быстрый старт в одну страницу — [quick_start.md](quick_start.md).*
