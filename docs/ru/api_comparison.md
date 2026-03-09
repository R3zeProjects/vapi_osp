# Сравнение API VAPI с другими решениями

Краткое сопоставление VAPI с сырым Vulkan, Vulkan-Hpp, vk-bootstrap, VMA, bgfx и Diligent Engine по целям, уровню абстракции и стилю API.

---

## 1. Позиционирование

| Решение | Назначение | Язык/API |
|--------|-------------|-----------|
| **Raw Vulkan** | Низкоуровневый графический API | C |
| **Vulkan-Hpp** | Официальная C++ обёртка над Vulkan (типы, RAII, STL) | C++, заголовки из спецификации |
| **vk-bootstrap** | Упрощение создания instance / device / swapchain | C++14, только инициализация |
| **VMA** | Аллокация памяти для буферов и изображений Vulkan | C API, реализация на C++ |
| **bgfx** | Кроссплатформенный рендер «принеси свой движок», много бэкендов | C++, единый API поверх D3D/GL/Metal/Vulkan/WebGPU |
| **Diligent Engine** | Кроссплатформенная абстракция под движки и приложения | C++, единый API, HLSL → платформенные шейдеры |
| **VAPI** | Vulkan-first библиотека: платформа, контекст, ресурсы, рендер, без сокрытия Vulkan | C++, модули core / platform / gpu / render / resource |

VAPI не заменяет Vulkan и не скрывает его: даёт фасады (менеджеры по ID, билдеры пайплайнов), но типы и контракты остаются в духе Vulkan. **Кросс-API:** интерфейсы и ABI уже есть (см. ниже); реализация и модули render/resource пока Vulkan-only.

**Какие кросс-API интерфейсы есть в VAPI:**

- **core/interfaces/i_gpu_abi.hpp** — типы без привязки к Vulkan: **PixelFormat**, **ShaderStage** / **ShaderStageFlags**, **VertexAttributeDesc**, **VertexBindingDesc**, **VertexInputRate**, **PrimitiveTopology**, **CompareOp**. Предназначены для описания пайплайнов и ресурсов в backend-agnostic виде.
- **gpu/i_gpu_backend.hpp** — **IGpuBackend**: `type()` (GpuBackendType::Vulkan; зарезервировано под D3D12/Metal), `getDevice()` / `getInstance()` / `getSurface()` / `getSwapchain()` как `void*` для работы с любым бэкендом через один интерфейс.
- **gpu/vk_abi_convert.hpp** — конвертация ABI → Vulkan: `toVkFormat()`, `toVkShaderStage()`, `toVkVertexInputAttributeDescription()` и т.д. Для будущих D3D/Metal бэкендов предполагаются свои конвертеры.

Модули **render** и **resource** пока используют Vulkan-типы напрямую (VkFormat, VkPipelineLayout и т.д.); ABI доступен для кода, который хочет описывать конфиг без привязки к API, и для будущих бэкендов.

### Философия: лучшее из имеющегося + улучшения

По сути API VAPI **берёт лучшее из существующих решений и усиливает его**:

- **Инициализация** — уровень удобства как у vk-bootstrap (один контекст, один init), плюс явный платформенный слой (окна, события) в одном наборе модулей.
- **Ошибки** — Result-подход как у vk-bootstrap, но единый **Result&lt;T&gt;** и свои коды везде, без смеси VkResult и исключений.
- **Ресурсы** — идея менеджеров и работы по ID как в движковых абстракциях (bgfx, Diligent), но без сокрытия Vulkan: остаются VkBuffer/VkImage там, где нужно, плюс перегрузки по BufferId/ImageId для типичных сценариев.
- **Пайплайны** — builder-стиль, как принято в современном C++; опции через структуры (LoadShaderOptions), чтобы не раздувать сигнатуры.
- **Безопасность** — явные проверки путей и размеров, лимиты (SPIR-V, буферы, изображения), чего нет в сыром Vulkan и типичных тонких обёртках.
- **Кросс-API** — задел в виде ABI и интерфейса бэкенда (как в движках), без обязательного перевода всего render/resource на абстракции «здесь и сейчас».

Итог: не «ещё один клон» чего-то одного, а **свод сильных сторон** (удобный init, Result везде, ID-менеджеры, билдеры, безопасность, задел под кросс-API) при сохранении Vulkan-first и без лишних слоёв абстракции.

---

## 2. Ошибки и возвращаемые значения

| Решение | Модель |
|--------|--------|
| **Vulkan (C)** | `VkResult` на каждый вызов, часто игнорируют. |
| **Vulkan-Hpp** | Исключения по умолчанию или `vk::Result`/`ResultValue`. |
| **vk-bootstrap** | Result-тип (успех или описание ошибки), без исключений. |
| **VMA** | `VkResult`, C-стиль. |
| **bgfx** | Коды возврата и внутренние проверки, без единого Result-типа в стиле expected. |
| **Diligent** | RefCountedObject, проверки; ошибки через возвращаемые коды и иногда исключения. |
| **VAPI** | **Result&lt;T&gt; = std::expected&lt;T, Error&gt;** везде, свои коды в `errors::*`, `errorMessage()`, макросы VAPI_TRY — единый стиль без исключений. |

У VAPI одна из самых последовательных моделей ошибок среди перечисленных: явный optional-подобный тип и централизованные коды.

---

## 3. Инициализация и контекст

| Решение | Подход |
|--------|--------|
| **Vulkan** | Ручные цепочки: instance → physical device → device → queue → swapchain (сотни строк). |
| **Vulkan-Hpp** | То же, но с C++ типами и RAII. |
| **vk-bootstrap** | **InstanceBuilder** → **PhysicalDeviceSelector** → **DeviceBuilder** → **SwapchainBuilder**; один builder-цепочка, минимум кода. |
| **VMA** | Только аллокатор: `vmaCreateAllocator(device, ...)` после создания device. |
| **bgfx** | `bgfx::init()`, платформа передаётся через нативный handle окна; инициализация «под ключ». |
| **Diligent** | Фабрики (EngineFactory), создание device и swapchain через конфиг. |
| **VAPI** | **GpuContext::init(GpuContextConfig)** — одна точка: instance → surface → device → swapchain; **createPlatform()** для окна/ввода. По духу близко к vk-bootstrap, но в одном объекте и с явным разделением платформы и GPU. |

VAPI даёт уровень удобства инициализации сопоставимый с vk-bootstrap, плюс платформенный слой (окна, события) в одном наборе заголовков.

---

## 4. Ресурсы (буферы, изображения, дескрипторы)

| Решение | Модель |
|--------|--------|
| **Vulkan / Vulkan-Hpp** | Ручное создание VkBuffer/VkImage, выделение памяти, привязка; дескрипторы — пулы, layout, write, bind. |
| **VMA** | Только память: `vmaCreateBuffer`/`vmaCreateImage` возвращают allocation; буфер/изображение создаётся приложением, VMA лишь аллоцирует. Не даёт менеджеров «по имени» или ID. |
| **bgfx** | Абстрактные Vertex/Index/Uniform-буферы, текстуры, фреймбуферы; API скрывает Vulkan. |
| **Diligent** | IBuffer, ITexture, IShaderResourceBinding; единый интерфейс поверх всех бэкендов. |
| **VAPI** | **BufferManager** / **ImageManager** — создание по **BufferDesc** / **ImageDesc**, работа по **BufferId** / **ImageId**. **StagingManager**: upload по ID (**uploadBuffer(BufferManager&, BufferId, …)**, **uploadImage(ImageManager&, ImageId, …)**). **DescriptorManager**: layout по **DescLayoutId**, пулы, allocate set, write. Без аллокатора памяти в стиле VMA; жизненный цикл и привязка пайплайн↔дескрипторы — на приложении. |

VAPI занимает промежуточное место: не кросс-API абстракция как bgfx/Diligent, но даёт ID-фасады и staging по ID, тогда как сырой Vulkan и VMA этого не дают.

---

## 5. Шейдеры и пайплайны

| Решение | Подход |
|--------|--------|
| **Vulkan** | SPIR-V загрузка вручную, VkShaderModule, VkPipelineLayout, VkDescriptorSetLayout, VkGraphicsPipeline — всё вручную. |
| **Vulkan-Hpp** | То же с C++ типами и RAII. |
| **vk-bootstrap** | Не касается шейдеров и пайплайнов. |
| **VMA** | Не касается. |
| **bgfx** | Шейдеры в едином формате (перевод в бэкенд); draw-примитивы (submit). |
| **Diligent** | HLSL как единый язык; компиляция в платформенные форматы; пайплайны и ресурсы через интерфейсы. |
| **VAPI** | **ShaderModule** (loadFromFile / loadFromMemory, **LoadShaderOptions**: allowedRoot, maxSpirvBytes), **ShaderProgram** по стадиям, **GraphicsPipelineBuilder** / **ComputePipelineBuilder** (setDescriptorSetLayouts, setPushConstantRange, build). Типы остаются Vulkan (VkPipelineLayout, VkDescriptorSetLayout); связка пайплайн↔дескрипторы — на приложении. |

VAPI уменьшает бойлерплейт пайплайнов и загрузки шейдеров по сравнению с сырым Vulkan, но не вводит свой шейдерный язык или кросс-API слой.

---

## 6. Платформа и окна

| Решение | Подход |
|--------|--------|
| **Vulkan** | Без стандарта: приложение само создаёт окно (GLFW, SDL, Win32…) и передаёт handle. |
| **vk-bootstrap** | Окно не входит в API; передаётся уже созданный surface/handle. |
| **bgfx** | Инициализация с нативным handle окна; бэкенд сам создаёт контекст. |
| **Diligent** | Интеграция с разными оконными системами через фабрики. |
| **VAPI** | **IPlatform** (createPlatform), **createWindow** / **destroyWindow**, **pollEvents**, колбэки; сейчас одна реализация (GLFW). Явное разделение: платформа в core/platform, GPU в gpu. |

Платформа в VAPI выделена в интерфейс и одну точку входа — ближе к «движковому» подходу, чем vk-bootstrap.

---

## 7. Безопасность и границы

| Решение | Особенности |
|--------|-------------|
| **Vulkan / VMA** | Минимум встроенных проверок; полагаются на validation layers и дисциплину вызывающего. |
| **bgfx / Diligent** | Внутренние проверки и контракты; детали зависят от бэкенда. |
| **VAPI** | **resolveSecure**, **isPathKeySafe**, проверки границ в **map**/upload, лимиты на размер SPIR-V и буферов/изображений, опциональный **allowedRoot** при загрузке файлов. Явный слой безопасности для путей и размеров. |

В сравнении с типичными Vulkan-библиотеками у VAPI сильнее акцент на контролируемых путях и размерах ресурсов.

---

## 8. Итоговая таблица

| Критерий | Raw Vulkan | Vulkan-Hpp | vk-bootstrap | VMA | bgfx | Diligent | VAPI |
|----------|------------|------------|--------------|-----|------|----------|------|
| Уровень | C API | C++ обёртка | Инициализация | Память | Кросс-API рендер | Кросс-API движок | Vulkan-first фасады |
| Ошибки | VkResult | Exception/Result | Result | VkResult | Коды | Коды/исключения | **Result&lt;T&gt;** везде |
| Контекст/init | Вручную | Вручную | Builder’ы | После device | init(desc) | Фабрики | **GpuContext::init** |
| Ресурсы по ID | — | — | — | — | Да | Да | **BufferId/ImageId**, staging по ID |
| Пайплайны | Вручную | Вручную | — | — | Скрыто | Интерфейсы | **Builder’ы**, Vulkan-типы |
| Платформа/окно | Своё | Своё | Своё | — | Handle | Фабрики | **IPlatform**, createWindow |
| Безопасность путей/размеров | Нет | Нет | Нет | Нет | Частично | Частично | **Да** (опции, лимиты) |
| Кросс-API | Нет | Нет | Нет | Нет | Да | Да | Интерфейсы/ABI есть; реализация пока Vulkan-only |

---

## 9. Вывод

- **VAPI** ближе к **vk-bootstrap + менеджеры ресурсов + билдеры пайплайнов**: упрощение инициализации и типичных сценариев без ухода от Vulkan-модели. От **VMA** отличается тем, что даёт именно менеджеры и ID, а не только аллокатор памяти.
- По согласованности ошибок (**Result&lt;T&gt;** без исключений) и явной безопасности (пути, размеры) VAPI выделяется среди типичных «тонких» Vulkan-обёрток.
- По сравнению с **bgfx** и **Diligent** VAPI пока не даёт готовых бэкендов под D3D/Metal; при этом **интерфейсы под кросс-API есть** (IGpuBackend, i_gpu_abi + vk_abi_convert). Модули render/resource используют Vulkan-типы напрямую; связку пайплайн↔дескрипторы оставляет на слой поверх — осознанный выбор в пользу Vulkan-first при заделе под расширение.

Удобство выше, чем у сырого Vulkan и голого Vulkan-Hpp; гибкость и «прозрачность» Vulkan выше, чем у bgfx/Diligent. Подходит тем, кто хочет один стек (C++, Vulkan), меньше бойлерплейта и предсказуемые ошибки без обязательного кросс-API слоя.
