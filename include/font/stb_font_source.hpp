/** @file stb_font_source.hpp
 *  @brief STB TrueType implementation of IFontSource (public domain, no attribution). */

#ifndef VAPI_FONT_STB_FONT_SOURCE_HPP
#define VAPI_FONT_STB_FONT_SOURCE_HPP

#include "font/i_font_source.hpp"
#include <vector>

namespace vapi {

/// Font source backed by STB TrueType. Load .ttf/.otf from memory.
class StbFontSource : public IFontSource {
public:
    StbFontSource();
    ~StbFontSource() override;

    [[nodiscard]] Result<void> loadFromMemory(std::span<const u8> data, u32 fontIndex = 0) override;
    void setPixelHeight(f32 pixelHeight) override;

    [[nodiscard]] f32 lineHeight() const override;
    [[nodiscard]] f32 ascender() const override;
    [[nodiscard]] f32 descender() const override;
    [[nodiscard]] f32 xHeight() const override;
    [[nodiscard]] f32 getAdvance(CodePoint cp) const override;
    [[nodiscard]] f32 getKernAdvance(CodePoint left, CodePoint right) const override;
    [[nodiscard]] GlyphBitmap getGlyphBitmap(CodePoint cp) override;
    [[nodiscard]] bool hasGlyph(CodePoint cp) const override;
    [[nodiscard]] bool hasDescender(CodePoint cp) const override;

private:
    std::vector<u8> m_data;
    void*           m_fontInfo{nullptr};  // stbtt_fontinfo (opaque to avoid including stb in header)
    f32             m_scale{1.f};
    f32             m_lineHeight{0.f};
    f32             m_ascent{0.f};
    f32             m_descent{0.f};
};

} // namespace vapi

#endif // VAPI_FONT_STB_FONT_SOURCE_HPP
