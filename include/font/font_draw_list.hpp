/** @file font_draw_list.hpp
 *  @brief UI-список отрисовки текста: AddText / AddImage → один атлас, один буфер вершин.
 *  Результат (getVertices, getAtlasRgba) загружается в GPU и рисуется одним draw call. */

#ifndef VAPI_FONT_FONT_DRAW_LIST_HPP
#define VAPI_FONT_FONT_DRAW_LIST_HPP

#include "font/font_types.hpp"
#include "font/glyph_atlas.hpp"
#include "font/i_font_source.hpp"
#include "font/text_painter.hpp"
#include "core/types.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vapi {

/// Прямоугольник в экранных координатах (p_min, p_max).
struct DrawRect {
    vec2 pmin{0.f, 0.f};
    vec2 pmax{0.f, 0.f};
};

/// UI-список отрисовки: AddText / AddImage, один атлас, один буфер вершин (один draw call).
class FontDrawList {
public:
    FontDrawList(u32 atlasWidth = 1024u, u32 atlasHeight = 1024u);

    /** Шрифт и размер для последующих AddText.
     *  @param font Указатель на источник шрифта; должен оставаться валидным на всё время использования списка (addText, getVertices и т.д.). Владение не передаётся.
     *  @param pixelHeight Высота шрифта в пикселях.
     *  @param oversample Oversampling для растеризации (1 или 2).
     *  @pre font может быть nullptr; тогда AddText не будет добавлять текст до следующего setFont с ненулевым указателем.
     *  @post При смене шрифта старые вершины и данные атласа остаются до явного вызова clear(). */
    void setFont(IFontSource* font, f32 pixelHeight, u32 oversample = 2u);

    /// Одинаковое расстояние между глифами (фиксированный шаг). По умолчанию true.
    void setEqualSpacing(bool on);
    [[nodiscard]] bool equalSpacing() const;

    /// Дополнительный зазор между глифами в пикселях. По умолчанию 0.
    void setExtraGlyphSpacing(f32 px);
    [[nodiscard]] f32 extraGlyphSpacing() const;
    /// Зазор пропорционально размеру шрифта: effective = ratio * pixelHeight.
    void setExtraGlyphSpacingRatio(f32 ratio);
    [[nodiscard]] f32 extraGlyphSpacingRatio() const;
    /// Зазор между глифами пропорционально длине глифа в шрифте: extra += ratio * advance.
    void setExtraGlyphSpacingRatioToWidth(f32 ratio);
    [[nodiscard]] f32 extraGlyphSpacingRatioToWidth() const;

    /// Междустрочный интервал в пикселях; если > 0 — используется вместо lineHeight шрифта. Выбираемый.
    void setLineSpacing(f32 px);
    [[nodiscard]] f32 lineSpacing() const;

    /// Установить/получить все опции текста одной структурой (согласованность с TextPainter).
    void setOptions(const TextPaintOptions& opts);
    [[nodiscard]] TextPaintOptions getOptions() const;

    /** Добавить текст в список. Накапливает вершины и глифы в атлас.
     *  Выравнивание по baseline: (x,y) — левый край, верх первой строки; baseline = y + ascent.
     *  @param x,y Позиция (левый край, верх первой строки).
     *  @param text Текст в UTF-8. Пустая строка: вызов не добавляет вершины.
     *  @pre setFont() должен быть вызван с ненулевым указателем; иначе addText не выполняет отрисовку.
     *  @post Вершины и атлас дополняются; для сброса вызвать clear(). */
    void addText(f32 x, f32 y, std::string_view text, color4 color);

    /** Добавить текстурный квад по экранному прямоугольнику и UV (атлас общий, UV в [0,1]).
     *  @param screenRect Экранные координаты квада (pmin, pmax).
     *  @param uvRect UV в атласе [0,1].
     *  @param color Цвет. */
    void addImage(const DrawRect& screenRect, const DrawRect& uvRect, color4 color);

    /** Заливка прямоугольника цветом (использует 1x1 белый пиксель атласа). */
    void addRect(const rect& r, color4 color);

    /** Очистить вершины и атлас. Шрифт (указатель и размер) не сбрасывается.
     *  @post getVertices().empty(), атлас пуст; последующие addText используют тот же шрифт. */
    void clear();

    /** Clear only vertices, keeping the glyph atlas intact across frames.
     *  Cached glyphs will be reused by subsequent addText calls without re-rasterization. */
    void clearVertices();

    /** Зарезервировать место под вершины перед серией addText (снижает реаллокации).
     *  Оценка: до 6 вершин на символ UTF-8 (верхняя граница). */
    void reserveForText(std::string_view text);

    /** Высота строки в экранных пикселях (Codium-style: для размещения следующих строк).
     *  @return lineHeight при текущем шрифте; 0 если setFont не вызывался или передан nullptr. */
    [[nodiscard]] f32 getLineHeight() const;

    /// Результат для загрузки на GPU и отрисовки.
    [[nodiscard]] const std::vector<FontVertex>& getVertices() const { return m_vertices; }
    /** Заполняет out RGBA-данными атласа (alpha → R=G=B=255, A=alpha). Переиспользует буфер, меняет размер при необходимости. Предпочтительно для горячего пути. */
    void fillAtlasRgba(std::vector<u8>& out) const;
    /** Возвращает новый вектор с RGBA (удобно для разовых вызовов; в цикле кадров предпочтительнее fillAtlasRgba). */
    [[nodiscard]] std::vector<u8> getAtlasRgba() const;
    [[nodiscard]] u32 getAtlasWidth() const { return m_atlas.atlasWidth(); }
    [[nodiscard]] u32 getAtlasHeight() const { return m_atlas.atlasHeight(); }
    [[nodiscard]] bool empty() const { return m_vertices.empty(); }
    [[nodiscard]] u64 atlasGeneration() const { return m_atlas.generation(); }

private:
    GlyphAtlas m_atlas;
    std::vector<FontVertex> m_vertices;
    IFontSource* m_font{nullptr};
    f32 m_pixelHeight{24.f};
    u32 m_oversample{2u};
    f32 m_lineHeightOutput{0.f};  ///< lineHeight в экранных пикселях (при setFont)
    std::unique_ptr<TextPainter> m_painter;
};

} // namespace vapi

#endif // VAPI_FONT_FONT_DRAW_LIST_HPP
