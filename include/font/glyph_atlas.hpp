/** @file glyph_atlas.hpp
 *  @brief Packs glyph bitmaps into a single atlas; produces UV/pixel quads.
 *  GPU-agnostic: outputs CPU bitmap + quad info for rendering. */

#ifndef VAPI_FONT_GLYPH_ATLAS_HPP
#define VAPI_FONT_GLYPH_ATLAS_HPP

#include "font/font_types.hpp"
#include "font/i_font_source.hpp"
#include "core/types.hpp"
#include <unordered_map>
#include <vector>

namespace vapi {

/// UV-границы 1x1 пикселя (для заливки rect в FontDrawList).
struct WhitePixelUV { f32 u0{0.f}, v0{0.f}, u1{0.f}, v1{0.f}; };

/// Packs glyph bitmaps into one atlas; maps CodePoint -> GlyphQuad.
/// Reserves (0,0) as 1x1 white pixel for solid rect drawing (getWhitePixelUVRect).
class GlyphAtlas {
public:
    GlyphAtlas(u32 atlasWidth = 512u, u32 atlasHeight = 512u);
    /** Очистить атлас: все глифы удаляются, память под битмап остаётся. */
    void clear();

    /** UV-прямоугольник для 1x1 белого пикселя в (0,0) — для отрисовки заливки (addRect). */
    [[nodiscard]] WhitePixelUV getWhitePixelUVRect() const;

    /** Ensure glyph is packed; returns quad (UV and pixel bounds).
     *  @param font Источник шрифта для растеризации глифа.
     *  @param cp Кодовая точка Unicode.
     *  @return Квад с UV и пиксельными границами. При переполнении атласа (нет места для нового глифа) возвращается пустой квад (все поля по умолчанию, в т.ч. pixelMaxX <= pixelMinX), и isFull() станет true. Успех можно проверить по (q.pixelMaxX > q.pixelMinX && (q.pixelMinY + q.pixelMaxY) > 0). Вызовите clear() и при необходимости увеличьте размер атласа, если isFull(). */
    [[nodiscard]] GlyphQuad ensureGlyph(IFontSource& font, CodePoint cp);

    /** True, если последний ensureGlyph не смог добавить глиф из-за нехватки места в атласе. Сбрасывается в clear() и при успешном ensureGlyph. Используйте для реакции на переполнение (clear(), новый атлас или больший размер). */
    [[nodiscard]] bool isFull() const { return m_atlasFull; }

    /** @return Битмап атласа (одноканальный alpha, row-major). Пустой, если глифов нет или после clear(). */
    [[nodiscard]] const std::vector<u8>& getAtlasPixels() const { return m_atlas; }
    [[nodiscard]] u32 atlasWidth() const { return m_atlasWidth; }
    [[nodiscard]] u32 atlasHeight() const { return m_atlasHeight; }

    /** @param cp Кодовая точка. Невалидный/отсутствующий глиф: возвращается nullptr.
     *  @return Указатель на квад, если глиф уже упакован в атлас; иначе nullptr. */
    [[nodiscard]] const GlyphQuad* getQuad(CodePoint cp) const;

private:
    u32 m_atlasWidth;
    u32 m_atlasHeight;
    std::vector<u8> m_atlas;
    u32 m_cursorX{0u};
    u32 m_cursorY{0u};
    u32 m_rowHeight{0u};
    std::unordered_map<CodePoint, GlyphQuad> m_quads;
    bool m_atlasFull{false};
};

} // namespace vapi

#endif // VAPI_FONT_GLYPH_ATLAS_HPP
