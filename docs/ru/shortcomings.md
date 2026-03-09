# Shortcomings and narrow spots (VAPI OSP)

## Design intent: forward-looking interfaces

Part of the interfaces are **intentionally** defined without full implementations or with stub implementations. They are placeholders for future features and may be refined (signatures, semantics, or split into several interfaces) as real backends are added. The goal is to fix the API shape early so that higher-level code can depend on abstractions rather than concrete types.

## Abstraction level and design choices

- **IGpuBackend** returns `void*` for device, instance, surface, swapchain so the interface stays backend-agnostic. Cast after checking `type()` (e.g. to `VkDevice` when `type() == GpuBackendType::Vulkan`).
- **GPU tools** (IGpuProfiler, IGpuFence) use `void* nativeCommandBuffer` / `nativeFence()` so the same interface can serve multiple backends. With the Vulkan implementation, pass the current `VkCommandBuffer` from the place where you record commands; no extra injection layer is required.
- **Render and resource modules** use Vulkan types (VkFormat, VkVertexInput*, etc.) by design. Cross-API abstraction (D3D/Metal) is not in scope for the current version.

## Architecture

- **Single platform backend**: Only GLFW is implemented; Win32/SDL are not. To swap implementation in the future, use `createPlatform(PlatformKind)` (currently only `PlatformKind::Glfw`). All platform code depends on GLFW and display.
- **Headless**: Use `windowHandle = nullptr` when creating the GPU context; no surface/swapchain. Tests that require a display skip gracefully (see testing.md).
- **Single-threaded event loop**: `pollEvents()` is blocking; no built-in async I/O or job queue for loading.
- **Some interfaces still stub-only or without implementation**: `IProfiler`, `IShaderSourceCompiler` (and optionally `IShaderCache` when not using `FileShaderCache`) have only stubs or no implementation yet. These are reserved for future backends; already implemented: CPU tools (ThreadPool, HostAllocatorAligned), GPU tools (VkGpuProfiler, VkGpuFence), FileShaderCache for IShaderCache.

## Performance

- **ThreadPoolStub** (when used instead of real ThreadPool): Runs tasks synchronously in `submit()`; no parallelism.
- **HostAllocatorStub** (when used): Uses `malloc`/`free`; no pooling or alignment above `max_align_t`.
- **No batching**: Draw calls and resource uploads are not batched at API level.

## GPU

- **Vulkan-only**: No abstraction over other APIs (Direct3D, Metal). A **cross-API ABI** is defined in **core/interfaces/i_gpu_abi.hpp** (PixelFormat, ShaderStage, vertex descriptors, topology, compare op); the Vulkan backend converts via **gpu/vk_abi_convert.hpp**. Render/resource still use Vulkan types directly; the ABI is available for future backends or refactors.
- **Pipeline cache**: Optional **PipelineCache** (load/save from file) is implemented; pass its handle to pipeline builder `build(device, layout, pipelineCache)` to reuse compiled state across runs.
- **Surface from GLFW only**: `VkSurfaceWrapper::create(instance, GLFWwindow*)` ties surface to GLFW.

## Testing

- **Display required for full flow**: GUI and window tests need a display; only `--headless` path is CI-friendly.
- **Render correctness test**: `test_render_correctness` clears an offscreen image to a known color and checks the readback; no golden/screenshot comparison yet (can be added later).

## Resolved / Addressed

- **Опции загрузки шейдеров:** введена **LoadShaderOptions** (`allowedRoot`, `maxSpirvBytes`); перегрузки `loadFromFile(device, path, stage, opts)` и `loadFromMemory(device, spirv, stage, opts)` — см. `render/shader.hpp`.
- **Staging и BufferManager:** добавлена перегрузка **uploadBuffer(BufferManager&, BufferId, data, offset)** — размер берётся из менеджера, проверка границ выполняется автоматически.
- **Краткий справочник:** добавлен **docs/quick_reference.md** — таблица основных классов и методов с отсылками к detail_docs.
- **Корневой include:** в **vapi.hpp** явно указано: для рендера — `render/render.hpp`, для ресурсов — `resource/resource.hpp`.
- **Именование платформы:** в документации и quick reference используется **IPlatform**; **IPlatformBackend** упоминается как алиас для совместимости.
- **Frame hooks и контекст кадра:** в **RunUILoopOptions** добавлены опциональные `onBeginFrame(FrameContext)`, `onFrameAcquired(FrameContext)` (после acquire кадра, с заполненным gpuFrameData), `onEndFrame(FrameContext)` и опциональный **IAppLogic***; в **runUILoop** передаётся **FrameContext**. В **onBeginFrame** кадр GPU ещё не получен — `gpuFrameData == nullptr`; в **onFrameAcquired** и **onEndFrame** контекст заполнен (см. **run_ui_loop.hpp**, **i_cpu_bridge.hpp**).
- **FrameContext:** единая структура кадра в **core/interfaces/i_cpu_bridge.hpp**; передаётся в колбэки и в **IAppLogic::onBeginFrame(FrameContext)** / **onEndFrame(FrameContext)** (при реализации интерфейса).
- **Опциональный ICpuServices:** в **RunUILoopOptions** и в **FontRenderApp::setCpuServices()**; передаётся в **VulkanUiPainter::setCpuServices()** из runUILoop; используется для **getOrCompileOnPool** при загрузке шейдеров (FileShaderCache + пул потоков), когда задан.
- **Ring staging:** в **StagingManager** реализован ring buffer слотов с fence; переиспользование буферов без смены API **uploadBuffer** / **uploadImage** / batch (см. **resource/staging.hpp**, **staging.cpp**).
- **Опциональная transfer-очередь:** **StagingManager::init()** принимает опциональные `transferQueue` и `transferPool`; **VulkanUiPainter** и **FontRenderApp** создают пул команд для transfer-очереди (если устройство её предоставляет) и передают в staging для разгрузки graphics-очереди.
- **Общий строитель бэкенда:** **createAndInitVulkanBackend()** в **render/run_context.hpp** используется в **runUILoop** и **FontRenderApp** для единообразной инициализации Vulkan backend.

- **Асинхронная загрузка ассетов:** **IAssetLoader** и **createSimpleAssetLoader()** (core/asset_loader.hpp) дают загрузку файла в потоке с колбэком; для текстур в колбэке выполните createImage + StagingManager::uploadImage на главном потоке (см. Recommendations п. 4).

### Отложено (без изменений в текущей версии)

- **Overlap CPU/GPU** (подготовка кадра N+1 пока рендерится N): добавлен колбэк **onPrepareNextFrame** в RunUILoopOptions; полная модель с двойным буфером данных кадра — по желанию в приложении.

## Recommendations

### Приоритет 1 (архитектура и контракты)

1. **Реализации интерфейсов:** при добавлении реальных бэкендов для заглушек (`IProfiler`, `IShaderSourceCompiler`) при необходимости уточнять сигнатуры и семантику интерфейсов, а не только реализовывать «как есть».
2. **Pipeline cache:** см. **pipeline_cache.md**; при появлении кэша пайплайнов оформить его как опцию в конфиге или в builder’е, не ломая текущие сигнатуры `build()`.
3. **Второй бэкенд платформы:** добавить headless или SDL для проверки абстракции платформы и CI без дисплея.

### Приоритет 2 (удобство и производительность)

4. **Асинхронная загрузка:** рассмотреть интерфейс асинхронного загрузчика и опциональную очередь заданий для ассетов (связка с **ICpuServices::getThreadPool()**).
5. **Батчинг:** при росте нагрузки рассмотреть батчинг draw calls и upload’ов на уровне API (без изменения контрактов там, где это возможно).
6. **Реальный ThreadPool/HostAllocator:** в релизных сборках подключать **ThreadPool** и **HostAllocatorAligned** вместо стабов, чтобы получить параллелизм и контроль над выравниванием.

### Приоритет 3 (тесты и качество)

7. **Headless CI:** оставить возможность исключать тесты, требующие дисплей (см. **testing.md**); при появлении headless-платформы — включить их в CI с заглушкой окна.
8. **Золотые изображения:** при необходимости расширить **test_render_correctness** сравнением с эталонным изображением (golden/screenshot).
9. **Аудит зависимостей:** периодически проверять GLFW и Vulkan SDK на известные CVE (см. также **docs/security_layer.ru.md**).
