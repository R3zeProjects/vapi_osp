# Связка дескрипторов и пайплайна: варианты решения

Сейчас приложение само связывает **DescriptorManager** (layout, pool, sets) и **GraphicsPipelineBuilder** / **ComputePipelineBuilder** (setDescriptorSetLayouts, setPipelineLayout, build). Ниже — рассмотренные варианты.

---

## Решение проекта

**Связку оставляем как есть.** Ответственность за соответствие «пайплайн ↔ дескрипторные layout’ы/sets» лежит на том, кто создаёт API поверх VAPI (слой приложения или промежуточный фреймворк). VAPI не вводит единый объект или хелпер для этой связки; канонический порядок шагов описан в разделе «Текущий поток» и может быть зафиксирован в примерах и детальной документации.

---

## Текущий поток (как есть)

1. **DescriptorManager:** `createLayout(DescriptorLayoutDesc)` → `DescLayoutId`, `getLayout(id)` → `VkDescriptorSetLayout`.
2. **Pipeline builder:** `setDescriptorSetLayouts(layouts, count)` или `setPipelineLayout(VkPipelineLayout)`; `build(device, outLayout)` создаёт пайплайн (и при перегрузке — layout).
3. **DescriptorManager:** `allocateSet(layoutId)`, `writeBuffer(set, binding, ...)`, `writeImage(set, binding, ...)`.
4. В draw: приложение держит в голове соответствие «этот пайплайн использует этот layoutId / эти сеты».

Риск: разъехались layout в дескрипторах и layout в пайплайне; нет одного места, где зафиксирована связь.

---

## Вариант 1. Хелпер «layout из DescriptorManager в builder» (минимально)

Не менять контракты, добавить одну точку входа, которая подставляет layout по **DescLayoutId**:

- В **DescriptorManager** или в отдельном хелпере (например, `resource/pipeline_descriptor_helper.hpp`):
  - **setLayoutsFromDescriptorManager(DescriptorManager&, const DescLayoutId* ids, u32 count)** — по массиву `DescLayoutId` получить `VkDescriptorSetLayout*` и вызвать `builder.setDescriptorSetLayouts(...)`.
- Либо перегрузки в **GraphicsPipelineBuilder** / **ComputePipelineBuilder**:
  - **setDescriptorSetLayouts(DescriptorManager&, std::initializer_list<DescLayoutId>)**  
  - внутри: для каждого id вызывать `descMgr.getLayout(id)`, собирать вектор layout’ов и вызывать текущий `setDescriptorSetLayouts(layouts.data(), n)`.

**Плюсы:** один вызов, меньше шанс перепутать порядок layout’ов; приложение по-прежнему явно указывает, какие layout’ы с каким пайплайном связаны.  
**Минусы:** связь «пайплайн ↔ набор layout’ов» по-прежнему только в коде приложения, не в объекте.

---

## Вариант 2. «Pipeline + layout IDs» в одном месте

Ввести конфиг/структуру, которая хранит связь пайплайна с дескрипторными layout’ами (и при желании — push constants):

- Например, **PipelineLayoutConfig**:
  - `std::vector<DescLayoutId> descriptorLayoutIds`;
  - `std::vector<VkPushConstantRange> pushConstantRanges`;
- **GraphicsPipelineBuilder::setPipelineLayoutConfig(DescriptorManager&, const PipelineLayoutConfig&)**:
  - по `descriptorLayoutIds` получить `VkDescriptorSetLayout` из менеджера;
  - создать `VkPipelineLayout` (или отдать в build);
  - вызвать `setDescriptorSetLayouts` / `setPushConstantRange` как сейчас.

Тогда приложение один раз задаёт «этот пайплайн — эти layout’ы», а не таскает сырые `VkDescriptorSetLayout*` между менеджером и билдером.

**Плюсы:** одна структура описывает layout пайплайна; можно переиспользовать один и тот же конфиг для нескольких пайплайнов (например, разные шейдеры, один и тот же набор дескрипторов).  
**Минусы:** нужен один общий тип конфига и согласие, что layout’ы всегда берутся из DescriptorManager по ID.

---

## Вариант 3. Высокоуровневый «Material» / «PipelineSet»

Ещё один шаг — объект, который владеет и пайплайном, и дескрипторным набором для него:

- **Material** (или **PipelineDescriptorSet**):
  - при создании: передаётся **DescriptorManager**, **DescriptorLayoutDesc** (или уже **DescLayoutId**), опционально конфиг пайплайна (шейдеры, vertex input и т.д.);
  - внутри: создаётся layout в DescriptorManager, создаётся пайплайн с этим layout’ом, аллоцируется хотя бы один **VkDescriptorSet**;
  - API: `getPipeline()`, `getDescriptorSet()` / `getSet(0)`, возможно `writeBinding(binding, buffer)` / `writeBinding(binding, imageView, sampler)` как обёртки над `writeBuffer`/`writeImage` с уже известным set.

Тогда «связка» живёт внутри одного объекта: пайплайн и его дескрипторный set всегда соответствуют друг другу.

**Плюсы:** приложению не нужно помнить соответствие; удобно для простых материалов (один layout, один set на draw).  
**Минусы:** гибкость ниже (несколько set’ов на пайплайн, переиспользование одного layout в разных пайплайнах нужно проектировать отдельно); больше кода в VAPI и больше ответственности (жизненный цикл пайплайна и пула дескрипторов).

---

## Вариант 4. Оставить как есть, формализовать в документации

Оставить текущее разделение: приложение явно создаёт layout в DescriptorManager, подставляет его в builder, аллоцирует sets и пишет binding’и. В **detail_docs** и примерах описать один и тот же канонический порядок:

1. Описать layout (DescriptorLayoutDesc).
2. Создать layout в DescriptorManager → DescLayoutId.
3. В builder: setDescriptorSetLayouts(descMgr.getLayout(id)) или setPipelineLayout(…).
4. build pipeline.
5. allocateSet(layoutId); writeBuffer/writeImage.
6. При draw: bind pipeline, bind descriptor set(s).

И добавить один полный пример (например, в resource.md или render.md) с кодом «от layout до draw», чтобы связка была воспроизводимым паттерном, а не импровизацией.

**Плюсы:** ноль изменений API, полный контроль у приложения.  
**Минусы:** связка по-прежнему только в коде и в головах; при рефакторинге легко разъехаться.

---

## Рекомендация (для слоя поверх VAPI)

Тому, кто строит API поверх VAPI, имеет смысл:

- Зафиксировать у себя канонический порядок: layout в DescriptorManager → передача в builder → build → allocate set → write → bind при draw.
- При желании реализовать **вариант 1** (хелпер по `DescLayoutId`) или **вариант 2** (конфиг с layout IDs) в своём слое, не меняя VAPI.
- При появлении понятия «материал» — вводить объект типа **варианта 3** у себя поверх DescriptorManager и pipeline builder.
