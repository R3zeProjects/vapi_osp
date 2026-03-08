/** @file outline_rasterizer.hpp
 *  @brief Rasterize glyph outlines (cubic Bézier contours) to alpha bitmap.
 *  Uses Horner evaluation, scanline fill, 4x supersampling for anti-aliasing. */

#ifndef VAPI_FONT_OUTLINE_RASTERIZER_HPP
#define VAPI_FONT_OUTLINE_RASTERIZER_HPP

#include "font/glyph_outline.hpp"
#include "core/types.hpp"
#include <vector>

namespace vapi::font {

struct RasterResult {
    std::vector<u8> data;  ///< Alpha, row-major, top-left origin
    u32 width{0u};
    u32 height{0u};
};

/// Rasterize outline to alpha bitmap. heightPx = approximate height; aspect from bbox.
/// Supersampling 6x, padding 3 px с каждой стороны (cursor_windows: рамка для чёткости).
RasterResult rasterize(const GlyphOutline& outline, f32 heightPx);

} // namespace vapi::font

#endif // VAPI_FONT_OUTLINE_RASTERIZER_HPP
