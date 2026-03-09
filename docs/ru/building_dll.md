# Сборка vapi_osp в виде DLL / shared library

## Что уже есть в vapi_osp

1. **Опция CMake**  
   `-DVAPI_BUILD_SHARED_LIBS=ON` — все цели (vapi_core, vapi_platform, vapi_gpu, vapi_render, vapi_resource, vapi_font, vapi_font_render, vapi_ui, vapi_ui_vulkan) собираются как shared (DLL на Windows, .so/.dylib на Unix).

2. **Экспорт символов**
   - **Windows (MSVC):** `WINDOWS_EXPORT_ALL_SYMBOLS ON` — экспортируются все символы без явного `__declspec(dllexport)` в коде.
   - **Windows (MinGW/GCC):** `-Wl,--export-all-symbols` — то же для MinGW.
   - При сборке библиотеки задаётся макрос **VAPI_OSP_BUILDING** (для заголовка `vapi_export.hpp`: при линковке приложения с DLL этот макрос не определять).

3. **Заголовок** `include/vapi_export.hpp`  
   Макрос **VAPI_API**: при `VAPI_OSP_BUILDING` на Windows — `__declspec(dllexport)`, иначе — `__declspec(dllimport)`. На не-Windows сейчас пустой. Его можно вручную проставлять в публичном API для более строгого ABI (см. ниже).

4. **Копирование артефактов**
   - Все собранные DLL/shared lib копируются в **`build/dll/`**.
   - Цель **export_lib** копирует в **`build/export_lib/`**: `bin/` (DLL или .so), `lib/` (импортные .lib или .a), `include/`, `docs/`, лицензии, README.

5. **POSITION_INDEPENDENT_CODE**  
   Включён для всех shared-целей (нужно для .so на Linux и корректной работы shared lib).

## Что нужно для использования DLL в приложении

- Собрать vapi_osp с **-DVAPI_BUILD_SHARED_LIBS=ON**.
- При сборке **приложения**, которое линкуется с vapi_osp, **не** определять **VAPI_OSP_BUILDING** (тогда в `vapi_export.hpp` будет подставляться dllimport).
- Положить все DLL (или .so/.dylib) рядом с исполняемым файлом приложения или в путь поиска (PATH / LD_LIBRARY_PATH / DYLD_LIBRARY_PATH). Удобно брать из `build/dll/` или `build/export_lib/bin/`.
- На Windows при использовании предсобранного GLFW как DLL — копировать и его DLL рядом с exe (в mathcad_app это уже делается через GLFW_IMPORTED_DLL).

## Что при необходимости можно добавить

- **Явный экспорт API:** если отключить `WINDOWS_EXPORT_ALL_SYMBOLS` и не использовать `--export-all-symbols`, нужно проставлять **VAPI_API** в заголовках на все классы и функции, которые должны быть видны снаружи DLL (например, `createPlatform()`, `createEchoBackend()`, `IPlatform`, `IAssistantBackend`, виджеты и т.д.). Это даёт более предсказуемый и минимальный ABI.
- **Копирование vapi_osp DLL в каталог exe приложения:** в приложении (например, mathcad_app) добавить `add_custom_command(TARGET mathcad_app POST_BUILD ...)` по аналогии с копированием GLFW DLL, чтобы все vapi_osp DLL из `build/dll/` или `export_lib/bin/` копировались в `$<TARGET_FILE_DIR:mathcad_app>`.

## ABI и инструментная цепочка (DLL)

- **Один инструмент и одна CRT:** приложение и все DLL vapi_osp должны быть собраны одной и той же версией компилятора (например, MinGW GCC 14.x или MSVC 2022) и линковаться с одной и той же C++ runtime. Смешивание разных CRT (например, статическая и динамическая, или разные версии MSVC) может приводить к падениям при выделении/освобождении памяти и при исключениях.
- **Документирование при распространении:** при распространении предсобранных DLL указывайте инструментную цепочку (например, «MinGW-w64 GCC 14.x x64», «MSVC 2022 x64»), чтобы потребители собирали приложение тем же компилятором.
