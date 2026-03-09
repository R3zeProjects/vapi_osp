/** @file i_font_source.hpp
 *  @brief Интерфейс источника глифов: загрузка, метрики, растр по кодовой точке.
 *  Реализации: StbFontSource (TTF/OTF), AsciiBezierFontSource (8×8 → Безье → растр). */

#ifndef VAPI_FONT_I_FONT_SOURCE_HPP
#define VAPI_FONT_I_FONT_SOURCE_HPP

#include "font/font_types.hpp"
#include "core/types.hpp"
#include <vector>
#include <cstddef>
#include <span>

namespace vapi {

/// Rasterized glyph: single-channel alpha bitmap (row-major, top-left origin).
struct GlyphBitmap {
    std::vector<u8> pixels;
    s32             width{0};
    s32             height{0};
    s32             stride{0};   ///< Bytes per row
    f32             advanceX{0.f};
    f32             bearingX{0.f};
    f32             bearingY{0.f};
};

/** Abstract font source: load from buffer, query metrics, rasterize glyphs.
 *  Implementations: StbFontSource (STB TrueType), or custom (e.g. system API). */
class IFontSource {
public:
    virtual ~IFontSource() = default;

    /// Load font from memory (e.g. .ttf/.otf file contents). fontIndex for TTC.
    [[nodiscard]] virtual Result<void> loadFromMemory(std::span<const u8> data, u32 fontIndex = 0) = 0;

    /// Scale for given pixel height. Call before getMetrics/getGlyphBitmap.
    virtual void setPixelHeight(f32 pixelHeight) = 0;

    [[nodiscard]] virtual f32 lineHeight() const = 0;
    [[nodiscard]] virtual f32 ascender() const = 0;
    [[nodiscard]] virtual f32 descender() const = 0;
    /// Высота строчных без выносных (x-height). По умолчанию ≈ 0.7*ascender, если не переопределено.
    [[nodiscard]] virtual f32 xHeight() const { return ascender() * 0.7f; }

    /// Advance width for codepoint (in pixels).
    [[nodiscard]] virtual f32 getAdvance(CodePoint cp) const = 0;

    /// Kern advance between two codepoints (optional; 0 if no kerning).
    [[nodiscard]] virtual f32 getKernAdvance(CodePoint left, CodePoint right) const = 0;

    /// Rasterize one glyph to alpha bitmap. Returns empty bitmap on missing glyph.
    [[nodiscard]] virtual GlyphBitmap getGlyphBitmap(CodePoint cp) = 0;

    /// Whether this codepoint is supported (has glyph).
    [[nodiscard]] virtual bool hasGlyph(CodePoint cp) const = 0;

    /// Есть ли у глифа выносной элемент ниже базовой линии (р, у, g, y, p, q и т.п.).
    [[nodiscard]] virtual bool hasDescender(CodePoint cp) const { (void)cp; return false; }
};

} // namespace vapi

#endif // VAPI_FONT_I_FONT_SOURCE_HPP
