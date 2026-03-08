# Модуль отрисовки шрифтов (vapi_font)

Векторные шрифты, системные пути, растеризация и отрисовка **UI-текста**. Layout по метрикам (динамическая ширина/высота), затем paint в квады и атлас. Модульный и заменяемый (подмена `IFontSource` без смены API).

## Слои модуля

| Слой | Файлы | Назначение |
|------|--------|------------|
| Типы | `font_types.hpp` | FontId, CodePoint, GlyphQuad, TextLayout, **TextGeometry** (результат paint) |
| Интерфейс | `i_font_source.hpp` | Источник глифов: растр + метрики |
| Источники | `stb_font_source`, `ascii_bezier_font_source` | TTF/OTF (STB), 8×8 → Безье → растр (ASCII + **кириллица**) |
| Безье | `cubic_bezier_horner`, `glyph_outline`, `outline_rasterizer` | Контуры, растеризация |
| Атлас | `glyph_atlas.hpp` | Упаковка глифов в одну текстуру |
| Paint | `text_painter.hpp` | Layout + paint в **TextGeometry** |
| Загрузка | `font_service`, `system_font_paths` | Загрузка по пути, поиск системных шрифтов |
| Draw list | `font_draw_list.hpp` | UI-список: **AddText** / **AddImage**, один атлас |

## Основное

- **Векторные шрифты**: TTF/OTF через STB; масштаб по высоте, динамическая ширина (advance + kerning) и высота строки.
- **Layout и paint**: `measureText` / `layoutText` → `TextLayout`; `paint` / `paintFromLayout` → **TextGeometry** (вершины + атлас).
- **Subpixel**: `setSubpixelScale(4)` для сглаживания.
- **Системные шрифты**: `getSystemFontDirectories()`, `getDefaultSystemFontPath()`, `getSystemFontPathsToTry()`.
- **Выход**: **TextGeometry** (вершины + битмап атласа). Вершины в формате `FontVertex` (= `render::VertexP2UC`).
- **Русский язык**: `AsciiBezierFontSource` поддерживает кириллицу (Ё, А–Я, а–я, ё). Строки в UTF-8; `TextPainter` декодирует кодпоинты и подставляет глифы.

## Использование

### TextPainter → TextGeometry

```cpp
#include "font/font.hpp"

vapi::FontService fonts;
auto id = fonts.loadFont("");  // системный шрифт по умолчанию
if (!id) return;

vapi::IFontSource* font = fonts.getFont(*id);
vapi::TextPainter painter(*font);
painter.setPixelHeight(24.f);

vapi::TextGeometry geom;
painter.paint("Hello, World!", 10.f, 50.f, vapi::color4::white(), geom);
// geom.vertices, geom.atlasPixels, geom.atlasWidth, geom.atlasHeight
```

### FontDrawList (UI-текст, один атлас)

```cpp
vapi::FontDrawList list(1024, 1024);
list.setFont(&someFont, 48.f, 2u);
list.addText(50.f, 100.f, "Line 1", vapi::color4::white());
list.addText(50.f, 160.f, "Line 2", vapi::color4::white());

const auto& vertices = list.getVertices();
std::vector<u8> rgba = list.getAtlasRgba();
// один атлас, один буфер вершин, один draw call
```

## UI-текст (без внешних зависимостей)

Пайплайн собственный: один атлас глифов, батчинг квадов, layout по метрикам, базовая линия, subpixel, степень альфы в шейдере. Подробнее: **`docs/font_ui_text.md`**. Тест: **`test_font_ui`**.

## Заменяемость

- Подмена `StbFontSource` на другую реализацию `IFontSource` (например FreeType) без изменения `FontService` и `TextPainter`.
- `FontService` можно заменить другим загрузчиком, возвращающим `IFontSource*`.
