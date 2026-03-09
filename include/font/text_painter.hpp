/** @file text_painter.hpp
 *  @brief High-level API: Chromium-style text — vector fonts, layout (dynamic width/height), then paint.
 *  Layout phase uses font metrics only; paint phase rasterizes at current pixel height. */

#ifndef VAPI_FONT_TEXT_PAINTER_HPP
#define VAPI_FONT_TEXT_PAINTER_HPP

#include "font/font_types.hpp"
#include "font/glyph_atlas.hpp"
#include "font/i_font_source.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace vapi {

/// UI-текст: layout по метрикам шрифта (динамическая ширина/высота), paint в квады + атлас.
/// Выравнивание как в Codium: baseline = startY + ascent, одна базовая линия на строку, выносные (g,y,p,q) ниже baseline,
/// lineHeight = ascent+|descent|+lineGap; опционально привязка к 0.5 px (setSubpixelScale(2)).
class TextPainter {
public:
    /** @param font Источник шрифта; должен быть валидным на время жизни TextPainter. */
    explicit TextPainter(IFontSource& font);
    ~TextPainter() = default;

    /// Vector font scale: any pixel height (rasterize at this size).
    void setPixelHeight(f32 pixels);
    [[nodiscard]] f32 lineHeight() const;
    [[nodiscard]] f32 ascender() const;
    [[nodiscard]] f32 descender() const;

    /// Subpixel: 1 = по пикселям, 4 = четверть пикселя (сглаживание).
    void setSubpixelScale(f32 scale) { m_subpixelScale = scale <= 0.f ? 1.f : scale; }
    [[nodiscard]] f32 subpixelScale() const { return m_subpixelScale; }

    /// Oversampling: 1 = 1:1, 2 = растеризация в 2x для сглаживания при линейной выборке.
    void setOversample(u32 factor) { m_oversample = factor < 1u ? 1u : factor; }
    [[nodiscard]] u32 oversample() const { return m_oversample; }

    /// Одинаковое расстояние между глифами (фиксированный шаг = max advance по строке).
    void setEqualSpacing(bool on) { m_equalSpacing = on; }
    [[nodiscard]] bool equalSpacing() const { return m_equalSpacing; }

    /// Дополнительный зазор между глифами в пикселях. По умолчанию 0.
    void setExtraGlyphSpacing(f32 px) { m_extraGlyphSpacing = px < 0.f ? 0.f : px; m_extraGlyphSpacingRatio = 0.f; }
    [[nodiscard]] f32 extraGlyphSpacing() const { return m_extraGlyphSpacing; }
    /// Зазор пропорционально размеру шрифта: effective = ratio * pixelHeight (если ratio > 0, иначе используется setExtraGlyphSpacing).
    void setExtraGlyphSpacingRatio(f32 ratio) { m_extraGlyphSpacingRatio = ratio < 0.f ? 0.f : ratio; m_extraGlyphSpacing = 0.f; }
    [[nodiscard]] f32 extraGlyphSpacingRatio() const { return m_extraGlyphSpacingRatio; }
    /// Зазор между глифами пропорционально длине глифа в шрифте: extra += ratio * advance (advance из шрифта).
    void setExtraGlyphSpacingRatioToWidth(f32 ratio) { m_extraGlyphSpacingRatioToWidth = ratio < 0.f ? 0.f : ratio; }
    [[nodiscard]] f32 extraGlyphSpacingRatioToWidth() const { return m_extraGlyphSpacingRatioToWidth; }

    /// Междустрочный интервал: если > 0, используется вместо lineHeight() шрифта при переходе на новую строку.
    void setLineSpacing(f32 px) { m_lineSpacingOverride = px < 0.f ? 0.f : px; }
    [[nodiscard]] f32 lineSpacing() const { return m_lineSpacingOverride; }

    /// Установить/получить все опции текста одной структурой (согласованность с FontDrawList).
    void setOptions(const TextPaintOptions& opts);
    [[nodiscard]] TextPaintOptions getOptions() const;

    /** Measure only: dynamic width (max line width), height (lineCount * lineHeight).
     *  @param utf8 Текст в UTF-8. Пустая строка: возвращает width/height 0, lineCount 0.
     *  @return Метрики текста без растеризации. */
    [[nodiscard]] TextMeasure measureText(std::string_view utf8) const;

    /** Layout only: positioned glyphs and bounds from metrics (no rasterization).
     *  @param utf8 Текст в UTF-8. Пустая строка: возвращает пустой layout (glyphs.empty(), bounds 0).
     *  @param x,y Начальная позиция (левый край, верх первой строки).
     *  @return Размещённые глифы и границы. */
    [[nodiscard]] TextLayout layoutText(std::string_view utf8, f32 x, f32 y) const;

    /** Paint from layout: rasterize at current pixel height, fill vertices + atlas.
     *  @param layout Результат layoutText. Пустой layout: out не изменяется по вершинам, атлас может быть пустым.
     *  @param out Заполняется вершинами и атласом (предыдущее содержимое перезаписывается в рамках вызова). */
    void paintFromLayout(const TextLayout& layout, color4 color, TextGeometry& out);

    /** Draw text at position (origin = baseline left). Fills vertices and atlas.
     *  @param utf8 Текст в UTF-8. Пустая строка: вершины не добавляются, атлас заполняется существующими глифами.
     *  @param out Вершины дополняются (не очищаются перед вызовом). */
    void drawText(std::string_view utf8, f32 x, f32 y, color4 color, TextGeometry& out);

    /** One-shot: measure + draw into out. Clears out first.
     *  @param utf8 Текст в UTF-8. Пустая строка: out очищается, вершины пусты, атлас пуст.
     *  @post out.vertices и out.atlasPixels содержат результат отрисовки. */
    void paint(std::string_view utf8, f32 x, f32 y, color4 color, TextGeometry& out);

    /** Дописать текст в внешний атлас и список вершин (для FontDrawList: несколько AddText в один список).
     *  @param utf8 Текст в UTF-8. Пустая строка: ничего не добавляется.
     *  @param atlas Атлас, в который добавляются глифы; при переполнении глиф может быть пропущен (пустой квад). */
    void appendText(std::string_view utf8, f32 x, f32 y, color4 color,
        GlyphAtlas& atlas, std::vector<FontVertex>& vertices);

private:
    static f32 snapToSubpixel(f32 v, f32 scale);
    [[nodiscard]] f32 getEffectiveExtraGlyphSpacing() const {
        return m_extraGlyphSpacingRatio > 0.f ? (m_extraGlyphSpacingRatio * m_pixelHeight) : m_extraGlyphSpacing;
    }

    IFontSource* m_font;
    GlyphAtlas   m_atlas;
    f32          m_pixelHeight{24.f};
    f32          m_subpixelScale{4.f};  ///< 4 = четверть пикселя
    u32          m_oversample{2u};     ///< 2 = 2x растеризация для сглаживания
    bool         m_equalSpacing{false}; ///< true = фиксированный шаг; false = динамическая ширина по ширине чернил глифа
    f32          m_extraGlyphSpacing{0.f};  ///< доп. пиксели между глифами (если ratio == 0)
    f32          m_extraGlyphSpacingRatio{0.f};  ///< если > 0 — зазор = ratio * pixelHeight (пропорционально размеру)
    f32          m_extraGlyphSpacingRatioToWidth{0.f}; ///< зазор += ratio * advance (длина глифа в шрифте)
    f32          m_lineSpacingOverride{0.f}; ///< если > 0 — междустрочный интервал в px вместо lineHeight()
};

} // namespace vapi

#endif // VAPI_FONT_TEXT_PAINTER_HPP
