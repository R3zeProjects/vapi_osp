# Как использовать VAPI DLL

## 1. Сборка DLL

В корне проекта (VAPI):

```bash
cmake -B build_dll -DVAPI_BUILD_DLL=ON -DVAPI_BUILD_EXAMPLES=OFF -DVAPI_BUILD_TESTS=OFF
cmake --build build_dll --config Release
```

В CLion: включите опцию **VAPI_BUILD_DLL** в настройках CMake и соберите проект.

Результат появится в папке **`dll/`**:
- `vapi.dll` — библиотека
- `vapi.lib` — библиотека импорта (для линковки)
- `vapi_launcher.exe` — готовый лаунчер
- `vapi_c_api.hpp` — заголовок C API
- `shaders/` — шейдеры
- `vulkan-1.dll`, при необходимости `glfw3.lib` / `glfw3.dll` (копируются автоматически при сборке)

Нужны: **VULKAN_SDK** в окружении, GLFW в `lib/glfw/` или путь в **VAPI_GLFW_DIR**.


## 2. Запуск готового приложения

Скопируйте папку **`dll/`** куда удобно и запустите из неё:

```
dll\vapi_launcher.exe
```

Рабочая директория должна быть папка `dll/` (там лежат `vapi.dll`, `vulkan-1.dll`, `shaders/`). В CLion для `vapi_launcher` working directory уже настроена на `dll/`.


## 3. Использование из своей программы (C API)

Подключите только заголовок и линкуйте с `vapi.lib`.

### Шаг 1: Подключите заголовок

Скопируйте **`vapi_c_api.hpp`** в свой проект или добавьте в include путь папку `dll/`.

### Шаг 2: Линковка

- **Visual Studio / MSVC:** добавьте в проект `dll/vapi.lib` (Linker → Input → Additional Dependencies).
- **CMake:**
  ```cmake
  target_include_directories(my_app PRIVATE path/to/dll)
  target_link_libraries(my_app PRIVATE path/to/dll/vapi.lib)
  ```

### Шаг 3: Запуск

При запуске вашего `.exe` в PATH или в одной папке с exe должны быть:
- `vapi.dll`
- `vulkan-1.dll`
- папка `shaders/` (рядом с exe или с `vapi.dll`, либо настроенный путь в коде).

Либо запускайте exe из папки `dll/`.

### Пример кода (C или C++)

```cpp
#include "vapi_c_api.hpp"

int main() {
    VAPI_App app = VAPI_AppCreate();
    if (!app) return 1;

    VAPI_AppConfig cfg = {};
    cfg.title      = "My App";
    cfg.width      = 1280;
    cfg.height     = 720;
    cfg.validation = 1;   // 1 = включить validation layers
    cfg.vsync      = 1;   // 1 = vsync

    if (VAPI_AppInit(app, &cfg) != 0) {
        VAPI_AppDestroy(app);
        return 1;
    }

    // Добавить слой Worksheet (канва с блоками)
    VAPI_Layer ws = VAPI_WorksheetCreate();
    if (ws)
        VAPI_AppPushLayer(app, ws);

    VAPI_AppRun(app);   // блокирующий цикл до закрытия окна
    VAPI_AppDestroy(app);
    return 0;
}
```

### Опционально: проверка отладчика

```cpp
if (VAPI_CheckDebugger()) {
    // отладчик подключён
    return 1;
}
```


## 4. Итог

| Действие              | Что делать |
|-----------------------|------------|
| Просто запустить      | `dll\vapi_launcher.exe` из папки `dll/` |
| Собрать DLL           | CMake с `-DVAPI_BUILD_DLL=ON`, артефакты в `dll/` |
| Своё приложение       | Подключить `vapi_c_api.hpp`, линковать `vapi.lib`, положить `vapi.dll` + `vulkan-1.dll` + `shaders/` рядом с exe или в PATH |
