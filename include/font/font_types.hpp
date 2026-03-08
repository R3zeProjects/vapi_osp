/** @file font_types.hpp
 *  @brief Типы модуля шрифтов: идентификаторы, метрики, layout, геометрия для отрисовки.
 *  Не зависят от render/GPU для заменяемости бэкенда. */

#ifndef VAPI_FONT_FONT_TYPES_HPP
#define VAPI_FONT_FONT_TYPES_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vapi {

// -----------------------------------------------------------------------------
// Идентификаторы и кодовые точки
// -----------------------------------------------------------------------------

/// Идентификатор загруженного шрифта (использовать с FontService). Невалидный ID: kInvalidFontId (0).
using FontId = u32;
constexpr FontId kInvalidFontId = 0u;

/// Кодовая точка Unicode.
using CodePoint = u32;

// -----------------------------------------------------------------------------
// Метрики и квады глифов
// -----------------------------------------------------------------------------

/// Метрики глифа в единицах шрифта или пикселях.
struct GlyphMetrics {
    f32 advanceX{0.f};   ///< Advance to next glyph
    f32 advanceY{0.f};
    f32 bearingX{0.f};  ///< Left/top bearing
    f32 bearingY{0.f};
    f32 width{0.f};
    f32 height{0.f};
};

/// Один глиф в атласе: UV и пиксельные границы.
struct GlyphQuad {
    f32 uvMinX{0.f}, uvMinY{0.f};
    f32 uvMaxX{0.f}, uvMaxY{0.f};
    f32 pixelMinX{0.f}, pixelMinY{0.f};
    f32 pixelMaxX{0.f}, pixelMaxY{0.f};
    f32 advanceX{0.f};
    /// Пустое пространство слева (первые столбцы битмапа — нули). Ширина глифа = от первого левого пикселя символа.
    f32 leadingEmptyPixels{0.f};
    /// Пустое пространство справа (последние столбцы — нули). Учитывается при зазоре и при расчёте ширины чернил.
    f32 trailingEmptyPixels{0.f};
};

/// Настройки отрисовки текста: spacing, line height и т.д. Используется в TextPainter и FontDrawList для согласованности.
struct TextPaintOptions {
    bool equalSpacing{false};           ///< Одинаковое расстояние между глифами (фиксированный шаг).
    f32  extraGlyphSpacing{0.f};        ///< Доп. зазор между глифами в пикселях.
    f32  extraGlyphSpacingRatio{0.f};   ///< Зазор пропорционально размеру: effective = ratio * pixelHeight (если > 0).
    f32  extraGlyphSpacingRatioToWidth{0.f}; ///< Зазор += ratio * advance (длина глифа в шрифте).
    f32  lineSpacing{0.f};             ///< Междустрочный интервал в px; если > 0 — вместо lineHeight() шрифта.
};

/// Дескриптор создания/загрузки шрифта.
struct FontDesc {
    std::string path;           ///< Path to .ttf/.otf file (or empty for default)
    std::string familyName;     ///< Optional: font family name for lookup
    f32 pixelHeight{24.f};      ///< Size in pixels (e.g. 24)
    u32 atlasWidth{512u};       ///< Atlas texture width
    u32 atlasHeight{512u};     ///< Atlas texture height
    u32 oversampleX{1u};        ///< Oversampling for quality (1 or 2)
    u32 oversampleY{1u};
};

// -----------------------------------------------------------------------------
// Layout текста (метрики без растеризации)
// -----------------------------------------------------------------------------

/// Результат измерения текста (без отрисовки). Ширина/высота из метрик шрифта.
struct TextMeasure {
    f32 width{0.f};       ///< Max line width (pixels)
    f32 height{0.f};      ///< Total height = lineCount * lineHeight
    f32 lineHeight{0.f};  ///< Single line height from font
    u32 lineCount{0u};
};

/// Один глиф, размещённый layout'ом (только метрики; без растеризации).
struct PositionedGlyph {
    CodePoint codepoint{0};
    f32 x{0.f};  ///< Baseline-left, subpixel
    f32 y{0.f};  ///< Базовая линия, subpixel
};

/// Результат фазы layout: размещённые глифы и границы (метрики векторного шрифта).
struct TextLayout {
    std::vector<PositionedGlyph> glyphs;
    f32 boundsWidth{0.f};   ///< Total width (max line width)
    f32 boundsHeight{0.f}; ///< Total height (lineCount * lineHeight)
    f32 lineHeight{0.f};
    u32 lineCount{0u};
};

// -----------------------------------------------------------------------------
// Геометрия для отрисовки (результат paint)
// -----------------------------------------------------------------------------

/// Вершина одного квада (position.xy, uv.xy, color). Совпадает с render::VertexP2UC.
struct FontVertex {
    vec2   position;
    vec2   uv;
    color4 color;
};

/// Результат отрисовки текста: вершины (6 на глиф) и битмап атласа. Выход paint/paintFromLayout.
struct TextGeometry {
    std::vector<FontVertex> vertices;
    std::vector<u8>         atlasPixels;  ///< R8 или RGBA, row-major
    u32                     atlasWidth{0u};
    u32                     atlasHeight{0u};
    u32                     bytesPerPixel{1u};  ///< 1 = R (alpha), 4 = RGBA
};

/// Обратная совместимость: псевдоним для TextGeometry.
using TextDrawList = TextGeometry;

} // namespace vapi

#endif // VAPI_FONT_FONT_TYPES_HPP
