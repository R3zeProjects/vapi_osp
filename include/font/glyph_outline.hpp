/** @file glyph_outline.hpp
 *  @brief Outline representation for glyphs: contours of cubic Bézier segments.
 *  Used by OutlineRasterizer to produce alpha bitmaps. */

#ifndef VAPI_FONT_GLYPH_OUTLINE_HPP
#define VAPI_FONT_GLYPH_OUTLINE_HPP

#include "font/cubic_bezier_horner.hpp"
#include "core/types.hpp"
#include <vector>

namespace vapi::font {

/// One closed contour = list of cubic segments.
using GlyphContour = std::vector<CubicSegment>;
/// Full outline = list of contours (e.g. main shape + holes).
using GlyphOutline = std::vector<GlyphContour>;

/// Append one rounded rectangle contour to outline. r <= 0: plain rectangle (4 linear segments).
void addRoundedRect(GlyphOutline& outline, f32 x0, f32 y0, f32 x1, f32 y1, f32 r);

} // namespace vapi::font

#endif // VAPI_FONT_GLYPH_OUTLINE_HPP
