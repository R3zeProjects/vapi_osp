/** @file ascii_bezier_font_source.hpp
 *  @brief IFontSource that builds glyphs from 8x8 bitmaps via cubic Bézier outlines.
 *  Outline = rounded rectangles per pixel; rasterization via OutlineRasterizer (Horner + scanline). */

#ifndef VAPI_FONT_ASCII_BEZIER_FONT_SOURCE_HPP
#define VAPI_FONT_ASCII_BEZIER_FONT_SOURCE_HPP

#include "font/i_font_source.hpp"
#include "core/types.hpp"

namespace vapi {

/// Font source: ASCII 32..126 + кириллица (Ё, А-Я, а-я, ё) из 8x8 битмапов → контуры Безье → растр.
class AsciiBezierFontSource : public IFontSource {
public:
    [[nodiscard]] Result<void> loadFromMemory(std::span<const u8> /*data*/, u32 /*fontIndex*/ = 0) override;
    void setPixelHeight(f32 pixelHeight) override;
    [[nodiscard]] f32 lineHeight() const override;
    [[nodiscard]] f32 ascender() const override;
    [[nodiscard]] f32 descender() const override;
    [[nodiscard]] f32 getAdvance(CodePoint cp) override;
    [[nodiscard]] f32 getKernAdvance(CodePoint left, CodePoint right) override;
    [[nodiscard]] GlyphBitmap getGlyphBitmap(CodePoint cp) override;
    [[nodiscard]] bool hasGlyph(CodePoint cp) const override;
    [[nodiscard]] bool hasDescender(CodePoint cp) const override;

private:
    f32 m_pixelHeight{24.f};
};

} // namespace vapi

#endif // VAPI_FONT_ASCII_BEZIER_FONT_SOURCE_HPP
