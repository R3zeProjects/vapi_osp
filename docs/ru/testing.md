# Запуск тестов (Testing)

Тесты собираются только при `VAPI_BUILD_TESTS=ON` (по умолчанию включено). См. [integration.md](integration.md).

## Уровни тестов и поведение в CI

| Уровень | Описание | Требует дисплей | В CI (headless) |
|---------|----------|------------------|------------------|
| **Unit** | Логика без GPU/окна (vec2, error, file_utils, emitter, layout, font metrics, …) | Нет | Запускаются |
| **GPU unit** | Vulkan/обёртки без окна (vk_gpu_profiler, vk_gpu_fence, gpu_abi, …) | Нет | Запускаются |
| **Integration (display)** | Окно/платформа/рендер (platform_integration, render_correctness, buffer_manager_integration, …) | Да | Пропускаются (возврат 0 и сообщение в stderr) |
| **Headless** | `main_headless` (vapi_main --headless) | Нет | Запускается |

В среде без дисплея интеграционные тесты, зависящие от окна, сами выходят с кодом 0 и сообщением «skip (no display)». Для полного прогона используйте среду с доступом к дисплею или исключите display-тесты через `ctest -E "test_render_correctness|test_platform_integration|..."`.

## Полная проверка (все юнит- и интеграционные тесты)

Из корня проекта:

```
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

- **`ctest`** запускает все цели, зарегистрированные через `add_test(NAME ... COMMAND ...)` в CMakeLists.txt.
- **`--output-on-failure`** выводит вывод теста при падении.

## Запуск одного теста

Из каталога `build`:

```
ctest --test-dir build -R test_gpu_tools --output-on-failure
```

Или запуск исполняемого файла напрямую:

```
.\test_gpu_tools.exe    # из каталога build или build/Debug
```

## Список тестов (примеры)

| Исполняемый файл | Описание |
|------------------|----------|
| `test_gpu_tools` | Стабы: IGpuProfiler, IShaderCache, IGpuFence, IShaderSourceCompiler |
| `test_vk_gpu_profiler` | VkGpuProfiler: ошибки создания (null device/phys/zero ranges), getResult(NotInitialized) |
| `test_vk_gpu_fence` | VkGpuFence: create(null), wait/isSignaled без инициализации |
| `test_vk_gpu_tools_integration` | Интеграция: реальный Vulkan (окно + устройство), профайлер + fence + command buffer |
| `test_render_correctness` | Корректность рендера: очистка offscreen-изображения в красный цвет, readback, проверка центрального пикселя |
| `test_resize_reveal_cache` | ResizeRevealCache: setShiftStep, pushFrame, frame, variantForSize, empty |
| `test_resize_config` | ResizeMode, ResizeConfig, WindowConfig.resize (значения по умолчанию и кастом) |
| `test_gpu_abi` | Кросс-API ABI: PixelFormat, ShaderStage, vertex descriptors, конвертация в Vulkan (toVkFormat и др.) |
| `test_file_utils` | Файловые утилиты, в т.ч. **isPathKeySafe**, **resolveSecure** (jail пути) |
| `test_security_layer` | Слой безопасности: isPathKeySafe, resolveSecure (несуществующий путь, файл под корнем) |
| `test_shader_cache` | FileShaderCache: небезопасный ключ отклоняется, путь под basePath, лимит SPIR-V, setMaxSpirvBytes |
| `test_buffer_manager_integration` | Интеграция: BufferManager — createBuffer(0 / >256 MiB) ошибка, map() проверка границ |
| `test_shader_limits_integration` | Интеграция: ShaderModule — loadFromMemory(0 / >4 MiB) ошибка, maxSpirvBytes, **LoadShaderOptions** |
| `test_staging_integration` | Интеграция: StagingManager — uploadBuffer(dstSize), uploadBuffer(BufferManager&, BufferId), uploadImage/updateImage(ImageManager&, ImageId) |
| `test_cpu_bridge` | CpuServicesHolder (set/get pool, alloc, profiler), SimpleCpuFrameDriver (beginFrame dt, frameIndex, reset) |
| `test_load_shader_options` | LoadShaderOptions: значения по умолчанию и кастом (allowedRoot, maxSpirvBytes) |

Интеграционные тесты **test_vk_gpu_tools_integration**, **test_render_correctness**, **test_buffer_manager_integration**, **test_shader_limits_integration**, **test_staging_integration** требуют дисплей (окно создаётся скрытым). В headless-среде (CI без GUI) они пропускаются с сообщением в stderr и возвращают 0.

## Headless / CI

- В CI (например `.github/workflows/ci.yml`) сборка и `ctest` запускаются без дисплея; тесты, требующие окно, пропускаются и возвращают 0.
- Тест **main_headless**: `vapi_main --headless` — проверка основного приложения без окна; всегда выполняется в CI.
- Для локального полного прогона с дисплеем запустите `ctest --test-dir build --output-on-failure`.
