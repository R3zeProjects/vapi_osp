# Документация VAPI (на русском)

**VAPI** — платформа для построения собственных API с сохранением полного контроля над Vulkan (контекст, обёртки, инструменты, кросс-API ABI, тесты). Не подменяет Vulkan, а упорядочивает его использование и даёт единые контракты. В комплекте: окно и ввод, режимы resize и кеш кадров, рендер-контекст и пайплайны.

**Быстрый старт:** [Быстрый старт (quick_start.md)](quick_start.md) — сборка, первый запуск и минимальный код на одной странице. [Примеры использования (usage_examples.md)](usage_examples.md) — готовые фрагменты кода.

- **Установка в систему:** после сборки выполните `cmake --install build`; затем в проекте используйте `find_package(vapi_osp)` и цели `vapi_osp::vapi_*`. См. [integration.md](integration.md).
- **Схема версий:** [VERSIONING.md](VERSIONING.md) (0. = релиз, 0.0 = бета, 0.0.1 = альфа, b = правка). **Текущая версия API:** 0.0.1-alpha.
- **Запуск тестов:** [testing.md](testing.md). Тесты собираются при `VAPI_BUILD_TESTS=ON`.
- **Поддерживаемые конфигурации и релиз:** [user_guide.md](user_guide.md), [RELEASING.md](RELEASING.md).

---

## Модули и назначение

| Модуль | Назначение |
|--------|-------------|
| **vapi_core** | Типы, ошибки (Result), файлы, логгер, математика, интерфейсы |
| **vapi_platform** | Окно и ввод (IPlatform, createPlatform); реализация — GLFW |
| **vapi_gpu** | Vulkan: instance, device, surface, swapchain (GpuContext) |
| **vapi_render** | Шейдеры, пайплайны, RenderContext |
| **vapi_resource** | Буферы, изображения, дескрипторы, staging |
| **vapi_font** | Шрифты, layout, атлас глифов, FontDrawList (без GPU) |
| **vapi_font_render** | Готовое приложение «окно + Vulkan + текст» (FontRenderApp) |
| **vapi_ui** | Виджеты, layout, сигналы (без Vulkan) |

Сценарий → цели и заголовки: [integration.md](integration.md#3-выбор-библиотек-по-сценарию).

---

## Оглавление (русская документация)

- **[user_guide.md](user_guide.md)** — Руководство пользователя: установка, сборка, примеры, типичные сценарии, частые проблемы.
- **[architecture.md](architecture.md)** — Обзор архитектуры: слои, сценарии, порядок вызовов API.
- **[integration.md](integration.md)** — Интеграция в проект: требования, add_subdirectory, выбор библиотек по сценарию, минимальные примеры, шейдеры.
- **[quick_start.md](quick_start.md)** — Быстрый старт: сборка, первый запуск, минимальный код.
- **[usage_examples.md](usage_examples.md)** — Примеры: окно с текстом, интерактивное приложение.
- **[quick_reference.md](quick_reference.md)** — Краткая таблица: основные классы и методы.
- **[testing.md](testing.md)** — Запуск тестов, уровни, CI, headless.
- **[VERSIONING.md](VERSIONING.md)** — Схема версий (0.0.1 = альфа, 0.0 = бета, 0. = релиз, b = правка).
- **[RELEASING.md](RELEASING.md)** — Процесс релиза: шаги, теги, поддерживаемые конфигурации.

Английская документация (основная): [../README.md](../README.md).
