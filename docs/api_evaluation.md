# Оценка текущего API VAPI

Краткая оценка публичного API: сильные стороны, слабые места, согласованность и рекомендации.

---

## 1. Сильные стороны

### Единый стиль ошибок
- **Result&lt;T&gt; = std::expected&lt;T, Error&gt;** везде: init, загрузка файлов, создание буферов, шейдеры, пайплайны.
- Константы в `errors::` и `errors::file::` — один namespace, предсказуемые коды.
- **errorMessage(Error)** и макросы **VAPI_TRY** / **VAPI_TRY_ASSIGN** упрощают обработку.

### Чёткое разделение модулей
- **vapi.hpp** — core + platform + gpu; **render/render.hpp** и **resource/resource.hpp** подключаются отдельно.
- Платформа не тянет GLFW в корневой include; зависимость только там, где нужна реализация.
- Документация по модулям (core, platform, gpu, render, resource) соответствует структуре заголовков.

### Удобные фасады
- **GpuContext** — один init (instance → surface → device → swapchain), один shutdown, доступ к обёрткам.
- **createPlatform()** / **createPlatformShared()** — одна точка входа для платформы.
- **BufferManager**, **ImageManager**, **StagingManager** — ID-based API, без ручного хранения сырых Vulkan-объектов там, где это не нужно.

### Типы и контракты
- Собственные алиасы (**u32**, **usize**, **f32**) и типы (**vec2**, **rect**, **color4**, **SizeElement**) — единый стиль.
- **Handle&lt;T&gt;** и **HandlePool&lt;T&gt;** — явная модель инвалидации (generation).
- Дескрипторы ресурсов (**BufferDesc**, **ImageDesc**, **SamplerDesc**) задают конфиг в одном месте.

### Builder-стиль где уместно
- **GraphicsPipelineBuilder** / **ComputePipelineBuilder** — цепочка вызовов, опциональные настройки без перегруженных конструкторов.
- **ShaderProgram** — набор модулей по стадиям, **stages(entry)** отдаёт готовый вектор для пайплайна.

### Безопасность
- Введены **resolveSecure**, **isPathKeySafe**, проверки границ в **map**/upload, настраиваемые лимиты (SPIR-V, буферы, размеры изображений).
- Опциональный **allowedRoot** в **loadFromFile** — безопасность без ломания старых вызовов.

---

## 2. Слабые места и несогласованности

### Смешение уровней абстракции
- **resource**: типы в духе Vulkan, идентификаторы — свои (**BufferId**, **ImageId**). Есть перегрузка **uploadBuffer(BufferManager&, BufferId, data, offset)** — для типичного сценария можно работать только по ID; низкоуровневый **uploadBuffer(VkBuffer, ..., dstSize)** остаётся для случаев без менеджера.
- **Pipeline builders** принимают **VkPipelineLayout**, **VkDescriptorSetLayout** — пользователь собирает их сам; **DescriptorManager** при этом свой. Связка «дескрипторы → пайплайн» остаётся на стороне приложения.

### Опциональные параметры
- Много параметров с значениями по умолчанию; для шейдеров введена **LoadShaderOptions** — перегрузки `loadFromFile(..., opts)` и `loadFromMemory(..., opts)` снимают проблему длинных сигнатур при расширении опций.

### Именование
- **IPlatformBackend = IPlatform** — в документации и quick reference зафиксировано использование **IPlatform**; второй алиас оставлен для совместимости.
- **BufferId** / **ImageId** — просто **u32**; **Handle&lt;T&gt;** с generation — другая модель. В resource везде ID без generation; при желании усилить защиту от use-after-free можно было бы сблизить с **Handle** (архитектурное решение на будущее).

### Два «корневых» include
- **vapi.hpp** — в комментарии явно указано: для рендера `render/render.hpp`, для ресурсов `resource/resource.hpp`.

### Документация API
- **detail_docs/** — пошаговые примеры; **quick_reference.md** — одна страница с таблицей основных классов и методов и отсылками к detail_docs.

---

## 3. Согласованность

| Аспект | Оценка |
|--------|--------|
| Возврат ошибок | Высокая — везде **Result&lt;T&gt;** или **void** с явным отказом. |
| Инициализация | **init(config)** / **shutdown()** у контекстов и менеджеров. |
| Идентификаторы | **BufferId**, **ImageId**, **SamplerId** — один стиль; **WindowId** в платформе. |
| Конфиги | **GpuContextConfig**, **BufferDesc**, **ImageDesc** — структуры с разумными default. |
| Vulkan-типы | Явно в API (VkDevice, VkBuffer в staging, VkPipeline в builders) — заявленный Vulkan-first соблюдён. |

---

## 4. Удобство использования

- **Плюсы:** мало магии, предсказуемый порядок init (platform → window → gpu → resource → render), примеры в learning.md и detail_docs дают полный цикл.
- **Минусы:** для простого «нарисовать треугольник» нужно пройти много слоёв (platform, context, swapchain, shaders, pipeline, buffers, descriptors, render loop). Это следствие явного контроля над Vulkan, а не недостаток API как такового.
- **Безопасность:** опции вроде **allowedRoot** и **maxSpirvBytes** по умолчанию не мешают; продвинутые сценарии получают контроль без смены существующего кода.

---

## 5. Рекомендации (приоритет)

| # | Рекомендация | Статус |
|---|------------------------------|--------|
| 1 | **LoadShaderOptions** для опций загрузки шейдеров | **Выполнено** - struct + перегрузки loadFromFile/loadFromMemory. |
| 2 | **uploadBuffer** по BufferId с проверкой границ | **Выполнено** - перегрузка uploadBuffer(BufferManager&, BufferId, data, offset). |
| 3 | **Quick reference** - одна страница с классами/методами | **Выполнено** - docs/quick_reference.md. |
| 4 | **IPlatform** как основное имя в документации | **Выполнено** - quick_reference и platform.md. |
| 5 | **Pipeline cache** - опция в конфиге/builder при появлении | В плане (shortcomings.md). |

---

## 6. Итог

API последовательный: единый **Result**/Error, разделение модулей, фасады (GpuContext, createPlatform), слой безопасности, связка с CPU (ICpuServices, IAppLogic). Ранее: смешение ID и handle, длинные сигнатуры, отсутствие справочника — частично устранены. Часть прежних слабых мест закрыта: **LoadShaderOptions**, **uploadBuffer(BufferManager&, BufferId, ...)**, **quick_reference.md**, комментарий в vapi.hpp, именование IPlatform. Оставшиеся нюансы соответствуют Vulkan-first. Оценка: **хорошее и расширяемое** API; удобство и документирование выше, чем в исходной оценке.
