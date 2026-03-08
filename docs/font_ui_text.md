# UI-текст (vapi_font)

Собственный пайплайн отрисовки текста в UI: без зависимостей от ImGui и Chromium. Один атлас глифов, батчинг квадов, один draw call.

## 1. Список отрисовки (FontDrawList)

| Элемент | Описание |
|--------|----------|
| **Список примитивов** | `FontDrawList::addText`, `addImage` — накопление текста и картинок |
| **Один атлас** | Один `GlyphAtlas` в списке; все глифы в одной текстуре |
| **Формат вершины** | `FontVertex`: position (vec2), uv (vec2), color (color4) |
| **Один draw call** | Все вершины из `getVertices()` рисуются одним `vkCmdDraw` с текстурой атласа |

Использование: несколько вызовов `addText` / `addImage`, затем загрузить `getVertices()` и `getAtlasRgba()` в буфер и текстуру и отрисовать одним draw call.

## 2. Layout и метрики (TextPainter)

| Элемент | Описание |
|--------|----------|
| **Layout** | `measureText`, `layoutText` → `TextLayout` по метрикам шрифта |
| **Базовая линия** | Все глифы выровнены по baseline; квад строится по ascent/descent |
| **Subpixel** | `setSubpixelScale(4)` — четверть пикселя для сглаживания |
| **Кернинг** | `IFontSource::getKernAdvance(left, right)` используется в `drawText` / `appendText` |
| **Ширина/высота** | Динамические по advance и line height |
| **Альфа в шейдере** | В `font_quad.frag`: `pow(alpha, 0.92)` для чётких краёв глифа |

## 3. Пайплайн

1. **Шрифт**: `FontService::loadFont` или `AsciiBezierFontSource` (ASCII + кириллица).
2. **Список**: `FontDrawList list; list.setFont(...); list.addText(x, y, "Hello", color);` — один атлас, один буфер вершин.
3. **Вершины**: `FontVertex` = pos.xy, uv.xy, color.
4. **Шейдер**: `font_quad.vert` (экран → NDC, Y вниз), `font_quad.frag` (атлас + степень альфы 0.92).
5. **Отрисовка**: один draw call на все квады, блендинг с альфой.

## 4. Тестирование

- `test_font` — загрузка шрифта, measure + paint, непустая геометрия и атлас.
- `test_font_ui` — 6 вершин на глиф, непустой атлас, FontDrawList и TextPainter, базовая линия.

Запуск: `ctest` или запуск исполняемых файлов тестов.
